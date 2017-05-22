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

typedef struct		s_flag
{
	char	*flags;
}					t_flag;

#endif