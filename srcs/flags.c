/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmarinh <frmarinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 21:48:45 by frmarinh          #+#    #+#             */
/*   Updated: 2017/05/24 21:48:50 by frmarinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "all.h"

BOOL							is_valid(char flag)
{
	if (flag == FLAG_H || flag == FLAG_g || flag == FLAG_G || \
	flag == FLAG_v)
		return (TRUE);
	return (FALSE);
}

BOOL							has_argument(char flag)
{
	if (flag == FLAG_H || flag == FLAG_g || flag == FLAG_G)
		return (TRUE);
	return (FALSE);
}

t_flag							*get_flags(char flag)
{
	t_flag		*flags;

	flags = get_data()->flags;
	while (flags)
	{
		if (flags->flag == flag)
			return flags;
		flags = flags->next;
	}
	return (NULL);
}

static	t_flag					*alloc_flag(char flag, char *value)
{
	t_flag			*new_flag;

	if (!(new_flag = (t_flag*)malloc(sizeof(struct s_flag))))
		return (NULL);
	new_flag->prev = NULL;
	new_flag->next = NULL;
	new_flag->flag = flag;
	new_flag->value = value;
	return (new_flag);
}

static void						add_in_list(t_flag *flag)
{
	t_flag	*flags;

	flags = get_data()->flags;
	if (flags)
	{
		while (flags->next)
			flags = flags->next;
		flags->next = flag;
		flags->prev = flag;
	}
}

void				add_flag(char *flag, char *value)
{
	t_flag		*flags;
	t_flag		*new_flag;

	flags = get_data()->flags;
	new_flag = alloc_flag(flag[0], value);
	if (flags)
		add_in_list(new_flag);
	else
		get_data()->flags = new_flag;
}