# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/04/10 17:19:11 by agrumbac          #+#    #+#              #
#    Updated: 2019/06/04 00:21:37 by agrumbac         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

############################## BIN #############################################

NAME = famine

SRC =	main.c \
	utils.c \
	famine.c \
	file_iterator.c \
	infect/infect.c \
	infect/endian.c \
	infect/encrypt.s \
	infect/decrypt.s \
	infect/accessors.c \
	infect/elf64_packer.c \
	infect/elf64_payload.s \
	infect/elf64_iterators.c \
	infect/elf64_find_entry.c \
	infect/elf64_setup_payload.c \
	infect/elf64_copy_to_clone.c \
	infect/elf64_adjust_refernces.c \

CC = clang

AS = nasm

SRCDIR = srcs

OBJDIR = objs

OBJC = $(addprefix ${OBJDIR}/, $(SRC:.c=.o))
OBJ = $(OBJC:.s=.o)

DEP = $(addprefix ${OBJDIR}/, $(SRC:.c=.d))

CFLAGS = -Wall -Wextra -MMD -g

ASFLAGS = -f elf64 -g

LDFLAGS = -Iincludes/

############################## COLORS ##########################################

BY = "\033[33;1m"
BR = "\033[31;1m"
BG = "\033[32;1m"
BB = "\033[34;1m"
BM = "\033[35;1m"
BC = "\033[36;1m"
BW = "\033[37;1m"
Y = "\033[33m"
R = "\033[31m"
G = "\033[32m"
B = "\033[34m"
M = "\033[35m"
C = "\033[36m"
W = "\033[0m""\033[32;1m"
WR = "\033[0m""\033[31;5m"
WY = "\033[0m""\033[33;5m"
X = "\033[0m"
UP = "\033[A"
CUT = "\033[K"

############################## RULES ###########################################

all: art ${NAME}

${NAME}: ${OBJ}
	@echo ${B}Compiling [${NAME}]...${X}
	@${CC} ${LDFLAGS} -o $@ ${OBJ}
	@echo ${G}Success"   "[${NAME}]${X}

${OBJDIR}/%.o: ${SRCDIR}/%.s
	@echo ${Y}Compiling [$@]...${X}
	@/bin/mkdir -p ${OBJDIR} ${OBJDIR}/infect
	@${AS} ${ASFLAGS} -o $@ $<
	@printf ${UP}${CUT}

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@echo ${Y}Compiling [$@]...${X}
	@/bin/mkdir -p ${OBJDIR} ${OBJDIR}/infect
	@${CC} ${CFLAGS} ${LDFLAGS} -c -o $@ $<
	@printf ${UP}${CUT}

############################## GENERAL #########################################

clean:
	@echo ${R}Cleaning"  "[objs]...${X}
	@/bin/rm -Rf ${OBJDIR}

fclean: clean
	@echo ${R}Cleaning"  "[${NAME}]...${X}
	@/bin/rm -f ${NAME}
	@/bin/rm -Rf ${NAME}.dSYM

re: fclean all

############################## DECORATION ######################################

art:
	@echo ${BB}
	@echo "       .-'\`\\-,/^\\ .-."
	@echo "      /    |  \\  ( "${WR}"oo"${X}${BB}"\\   __"
	@echo "     |     |  |__/,--.\`\"\`  \`,"
	@echo "     |    /   .__/    \`\"\"\"\",/"${BG}"__               _"${BB}
	@echo "     |   /    /  |       "${BG}"|  ___|_ _ _ __ ___ (_)_ __   ___ "${BB}
	@echo "    .'.-'    /__/        "${BG}"| |_ / _\` | '_ \` _ \\| | '_ \\ / _ \\ "${BB}
	@echo "   \`\"\`| |';-;_\`          "${BG}"|  _| (_| | | | | | | | | | |  __/"${BB}
	@echo "      |/ /-))))))        "${BG}"|_|  \\__,_|_| |_| |_|_|_| |_|\\___|"${BB}
	@echo ${X}

.PHONY: all clean fclean re art

-include ${DEP}
