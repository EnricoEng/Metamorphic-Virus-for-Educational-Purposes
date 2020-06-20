/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 03:38:38 by agrumbac          #+#    #+#             */
/*   Updated: 2020/06/20 14:52:03 by ichkamo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOADER_H
# define LOADER_H

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

/*
** the virus header and its position in the loader's code
*/

struct			virus_header
{
	uint64_t	seed[2];
}__attribute__((packed));

void		virus_header_struct(void);

/*
** loader
*/

void		loader_entry(void);
void		jump_back_to_client(void);
void		loader_exit(void);

/*
** end of virus (cf Makefile)
*/

void		_start(void);

#endif
