/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmarinh <frmarinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/24 21:31:40 by frmarinh          #+#    #+#             */
/*   Updated: 2017/05/24 21:31:48 by frmarinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "all.h"

static t_data			*alloc_data(void)
{
	if (!(g_data = (t_data*)malloc(sizeof(struct s_data))))
		return (NULL);
	g_data->flags = NULL;
	g_data->host = NULL;
	g_data->sequence = 0;
	return (g_data);
}

t_data					*get_data(void)
{
	return ((g_data == NULL) ? alloc_data() : g_data);
}

void					set_host(char *host)
{
	t_data			*data;

	data = get_data();
	data->host = host;
	data->default_host = ft_strdup(host);
}

void				parse_data(void)
{
	t_flag		*flags;

	flags = get_data()->flags;
	while (flags)
	{
		if (is_valid(flags->flag))
		{
			if ((flags->value == NULL && has_argument(flags->flag)) || (flags->value != NULL && !has_argument(flags->flag)) \
			|| (flags->value != NULL && flags->value[0] == '-'))
			{
				print_usage();
				exit(0);
			}
		}
		else
		{
			print_usage();
			exit(0);
		}
		flags = flags->next;
	}
	if (get_data()->host == NULL)
	{
		print_usage();
		exit(0);
	}
	hostname_to_ip(get_data()->host, get_data()->host);
	start_icmp_connection();
}
