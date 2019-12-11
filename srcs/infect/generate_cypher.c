
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <stdbool.h>

#include "accessors.h"

#define CYPHER		0
#define DECYPHER	1

#define IMM_IB		1
#define IMM_IW		2
#define IMM_ID		4
#define IMM_IO		8

#define ENCODE_VALUE(value, mask, shift_bit)	((value & mask) << shift_bit)

enum
{
	I_BASE,
	XOR_RM64_IMM8,		// REX.W + 83 /6 ib XOR r/m64, imm8
	ADD_RM64_IMM8,		// REX.W + 83 /0 ib ADD r/m64, imm8
	SUB_RM64_IMM8,		// REX.W + 83 /5 ib SUB r/m64, imm8

	XOR_RAX_IMM32,		// REX.W + 35 id XOR RAX, imm32
	ADD_RAX_IMM32,		// REX.W + 05 id ADD RAX, imm32
	SUB_RAX_IMM32,		// REX.W + 2D id SUB RAX, imm32
	I_SIZE
};

struct	x86_64_encode
{
	uint8_t		size:8;
	uint8_t		prefix_rex:4;
	uint8_t		prefix_rex_w:1;
	uint8_t		prefix_rex_r:1;
	uint8_t		prefix_rex_x:1;
	uint8_t		prefix_rex_b:1;
	uint8_t		opcode_escape:8;
	uint8_t		opcode_two:8;
	uint8_t		opcode_one:8;
	uint8_t		mod:2;
	uint8_t		reg:3;
	uint8_t		rm:3;
	uint8_t		scale:2;
	uint8_t		index:3;
	uint8_t		base:3;
	uint32_t	displacement:32;
	uint32_t	immediate:32;
}__attribute__((packed));

static int64_t	get_random_inrange(uint64_t *seed, int64_t lower, int64_t upper)
{
	uint64_t	rand = *seed;

	rand += 0xf0760a3c4;
	rand ^= rand << 13;
	rand ^= rand >> 17;
	rand -= 0x6fa624c2;
	rand ^= rand << 5;

	*seed = rand;

	return (rand % (upper - lower + 1)) + lower;
}

static int64_t	get_random_exrange(uint64_t *seed, int64_t lower, int64_t upper)
{
	uint64_t	rand = *seed;

	rand += 0xf0760a3c4;
	rand ^= rand << 13;
	rand ^= rand >> 17;
	rand -= 0x6fa624c2;
	rand ^= rand << 5;

	*seed = rand;

	return (rand % (upper - lower - 1)) + lower + 1;
}

static uint8_t	encode_instruction(uint8_t *buffer, uint64_t *seed,
			size_t size, bool operation)
{
	const struct x86_64_encode	instructions[I_SIZE] =
	{
		[XOR_RM64_IMM8] = {4, 0b0100,0b1,0,0,0, 0,0,0x83, 0b11,0b110,0b000, 0,0,0, 0,IMM_IB},
		[ADD_RM64_IMM8] = {4, 0b0100,0b1,0,0,0, 0,0,0x83, 0b11,0b000,0b000, 0,0,0, 0,IMM_IB},
		[SUB_RM64_IMM8] = {4, 0b0100,0b1,0,0,0, 0,0,0x83, 0b11,0b101,0b000, 0,0,0, 0,IMM_IB},

		[XOR_RAX_IMM32] = {6, 0b0100,0b1,0,0,0, 0,0,0x35, 0,0,0, 0,0,0, 0,IMM_ID},
		[ADD_RAX_IMM32] = {6, 0b0100,0b1,0,0,0, 0,0,0x05, 0,0,0, 0,0,0, 0,IMM_ID},
		[SUB_RAX_IMM32] = {6, 0b0100,0b1,0,0,0, 0,0,0x2d, 0,0,0, 0,0,0, 0,IMM_ID},
	};
	const int			instructions_match[I_SIZE] =
	{
		[XOR_RM64_IMM8] = XOR_RM64_IMM8,
		[ADD_RM64_IMM8] = SUB_RM64_IMM8,
		[SUB_RM64_IMM8] = ADD_RM64_IMM8,

		[XOR_RAX_IMM32] = XOR_RAX_IMM32,
		[ADD_RAX_IMM32] = SUB_RAX_IMM32,
		[SUB_RAX_IMM32] = ADD_RAX_IMM32
	};

	uint8_t		instruction = get_random_exrange(seed, I_BASE, I_SIZE);
	uint64_t	immediate   = get_random_inrange(seed, 0x1, 0x7fffffff);

	struct x86_64_encode	i;

	i = (operation == CYPHER) ? instructions[instruction] : instructions[instructions_match[instruction]];

	if (i.size > size)
	{
		buffer -= operation == CYPHER ? 0 : 1;
		*buffer = 0x90;
		return 1;
	}
	else if (operation == DECYPHER)
		buffer -= i.size;

	uint8_t			operand_size = i.immediate;
	switch (operand_size)
	{
		case IMM_IB: immediate &= 0x7f; break;
		case IMM_IW: immediate &= 0x7fff; break;
		case IMM_ID: immediate &= 0x7fffffff; break;
		case IMM_IO: immediate &= 0x7fffffffffffffff; break;
		default: immediate &= 0x7fffffffffffffff;
	}

	uint8_t			*current_byte = (uint8_t*)buffer;

	/* REX prefix */
	*current_byte |= ENCODE_VALUE(i.prefix_rex, 0x07, 0x04);
	*current_byte |= ENCODE_VALUE(i.prefix_rex_w, 0x01, 0x03);
	*current_byte |= ENCODE_VALUE(i.prefix_rex_r, 0x01, 0x02);
	*current_byte |= ENCODE_VALUE(i.prefix_rex_x, 0x01, 0x01);
	*current_byte |= ENCODE_VALUE(i.prefix_rex_b, 0x01, 0x00);
	if (*current_byte) current_byte++;
	/* Opcode */
	*current_byte |= ENCODE_VALUE(i.opcode_escape, 0xff, 0x00);
	if (*current_byte) current_byte++;
	*current_byte |= ENCODE_VALUE(i.opcode_two, 0xff, 0x00);
	if (*current_byte) current_byte++;
	*current_byte |= ENCODE_VALUE(i.opcode_one, 0xff, 0x00);
	if (*current_byte) current_byte++;
	/* MOD/RM */
	*current_byte |= ENCODE_VALUE(i.mod, 0x03, 0x06);
	*current_byte |= ENCODE_VALUE(i.reg, 0x07, 0x03);
	*current_byte |= ENCODE_VALUE(i.rm, 0x07, 0x00);
	if (*current_byte) current_byte++;
	/* SIB */
	*current_byte |= ENCODE_VALUE(i.scale, 0x03, 0x06);
	*current_byte |= ENCODE_VALUE(i.index, 0x07, 0x03);
	*current_byte |= ENCODE_VALUE(i.base, 0x07, 0x00);
	if (*current_byte) current_byte++;
	/* Displacement */
	if (i.displacement)
		memcpy(current_byte, &immediate, i.immediate);
	if (*current_byte) current_byte += operand_size;
	/* Immediate */
	if (i.immediate)
		memcpy(current_byte, &immediate, i.immediate);
	if (*current_byte) current_byte += operand_size;

	return i.size;
}

static void	generate_shuffler(char *cypher, uint64_t seed, size_t size)
{
	uint8_t		*current_cypher = (uint8_t*)cypher;
	uint8_t		cypher_size     = 0;

	bzero(current_cypher, size);

	while (size)
	{
		cypher_size = encode_instruction(current_cypher, &seed, size, CYPHER);
		current_cypher += cypher_size;
		size -= cypher_size;
	}
}

static void	generate_unshuffler(char *decypher, uint64_t seed, size_t size)
{
	uint8_t		*current_decypher = (uint8_t*)decypher;
	uint8_t		decypher_size     = 0;

	bzero(current_decypher, size);
	current_decypher += size;

	while (size)
	{
		decypher_size = encode_instruction(current_decypher, &seed, size, DECYPHER);
		current_decypher -= decypher_size;
		size -= decypher_size;
	}
}

/*
** generate_loop_frame:
**   - writes a header at the beginning of the buffer
**   - writes a footer at the end of the buffer
**   - returns a safe pointer to the middle of the buffer
**   - returns a NULL safe pointer in case size is too small
*/
static struct safe_pointer    generate_loop_frame(char *buffer, size_t size)
{
	int8_t		header[] = {
		                   /* cypher: */
		0x48, 0x85, 0xf6,  /*     test rsi, rsi  */
		0x74, 0x0b,        /*     jz cypher_end  */
		0x48, 0x8b, 0x07   /*     mov rax, [rdi] */
	};
	int8_t		footer[] = {
		0x48, 0x89, 0x07,  /*     mov [rdi], rax */
		0x48, 0xff, 0xce,  /*     dec rsi        */
		0xeb, 0xd2,        /*     jmp cypher     */
		                   /* cypher_end:        */
		0xc3               /*     ret            */
	};

	if (size < sizeof(footer) + sizeof(header))
		return (struct safe_pointer){NULL, 0};

	char	*remaining_buffer = buffer + sizeof(header);
	size_t	remaining_size    = size - sizeof(footer) - sizeof(header);

	memcpy(buffer, header, sizeof(header));
	memcpy(buffer + size - sizeof(footer), footer, sizeof(footer));

	header[4] += remaining_size; /* relative cypher_end */
	footer[7] -= remaining_size; /* relative cypher */

	return (struct safe_pointer){remaining_buffer, remaining_size};
}

bool		generate_cypher(char *buffer, uint64_t seed, size_t size)
{
	struct safe_pointer	frame;

	frame = generate_loop_frame(buffer, size);
	if (frame.ptr == NULL) return false;

	generate_shuffler(frame.ptr, seed, frame.size);
	return true;
}

bool		generate_decypher(char *buffer, uint64_t seed, size_t size)
{
	struct safe_pointer	frame;

	frame = generate_loop_frame(buffer, size);
	if (frame.ptr == NULL) return false;

	generate_unshuffler(frame.ptr, seed, frame.size);
	return true;
}
