/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   all.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmarinh <frmarinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/22 21:55:25 by frmarinh          #+#    #+#             */
/*   Updated: 2017/05/22 21:55:33 by frmarinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		ALL_H
# define	ALL_H

# include	"../libft/includes/libft.h"
# include	<stdio.h>
# define	BOOL	int
# define	TRUE	1
# define	FALSE	0
# define	FLAG_H	'h'
# define	FLAG_G	'G'
# define	FLAG_g	'g'
# define	FLAG_v	'v'

typedef struct		s_flag
{
	char			flag;
	char			*value;
	struct s_flag	*next;
	struct s_flag	*prev;
}					t_flag;

typedef struct		s_data
{
	t_flag			*flags;
	char			*host;
}					t_data;

/*
** MAIN
*/
void				print_usage(void);

/*
**	DATA
*/
t_data				*get_data(void);
void				set_host(char *host);
void				parse_data(void);

/*
** FLAGS
*/
void				set_flags(t_flag *flags);
void				add_flag(char *flag, char *value);
BOOL				has_argument(char flag);
BOOL				is_valid(char flag);
#endif

t_data				*g_data;