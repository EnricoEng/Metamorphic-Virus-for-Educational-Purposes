/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfortin <jfortin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 22:22:08 by jfortin           #+#    #+#             */
/*   Updated: 2019/06/06 23:19:41 by jfortin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "compiler_utils.h"
#include "syscall.h"

# define SYS_READ       0
# define SYS_WRITE      1
# define SYS_OPEN       2
# define SYS_CLOSE      3
# define SYS_FSTAT      5
# define SYS_MMAP       9
# define SYS_MPROTEC    10
# define SYS_MUNMAP     11
# define SYS_PTRACE     101
# define SYS_GETDENTS64 217
# define SYS_OPENAT     257

# define _u	__unused

# define wrap_syscall(x)	wrapper_syscall(x)

# define wrapper_syscall(x)	asm volatile (".intel_syntax;\n"        \
						"  mov r10, rcx\n"      \
						"  mov rax, " #x "\n"   \
						"  syscall\n"           \
						"  leave\n"             \
						"  ret\n");


inline ssize_t famine_read(_u int fd, _u void *buf, _u size_t count)
{
	wrap_syscall(SYS_READ);
	__builtin_unreachable();
}

inline ssize_t	famine_write(_u int fd, _u const void *buf, _u size_t count)
{
	wrap_syscall(SYS_WRITE);
	__builtin_unreachable();
}

inline int	famine_open(_u const char *pathname, _u int flags, ...)
{
	wrap_syscall(SYS_OPEN);
	__builtin_unreachable();
}

inline int	famine_close(_u int fd)
{
	wrap_syscall(SYS_CLOSE);
	__builtin_unreachable();
}

inline int	famine_fstat(_u int fd, _u struct stat *statbuf)
{
	wrap_syscall(SYS_FSTAT);
	__builtin_unreachable();
}

inline void	*famine_mmap(_u void *addr, _u size_t length, _u int prot, _u int flags, _u int fd, _u
		off_t offset)
{
	wrap_syscall(SYS_MMAP);
	__builtin_unreachable();
}

inline int	famine_mprotect(_u void *addr, _u size_t len, _u int prot)
{
	wrap_syscall(SYS_MPROTEC);
	__builtin_unreachable();
}

inline int	famine_munmap(_u void *addr, _u size_t length)
{
	wrap_syscall(SYS_MUNMAP);
	__builtin_unreachable();
}

inline long	famine_ptrace(_u enum __ptrace_request request,
	_u pid_t pid, _u void *addr, _u void *data)
{
	wrap_syscall(SYS_PTRACE);
	__builtin_unreachable();
}

inline int	famine_getdents64(_u unsigned int fd, _u struct dirent64 *dirp, _u unsigned int count)
{
	wrap_syscall(SYS_GETDENTS64);
	__builtin_unreachable();
}
