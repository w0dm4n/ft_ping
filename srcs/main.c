/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmarinh <frmarinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/20 18:32:46 by frmarinh          #+#    #+#             */
/*   Updated: 2017/05/20 18:34:06 by frmarinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "all.h"

void			print_usage(void)
{
	printf("usage: ft_ping [-v] [-g sweepminsize] [-G sweepmaxsize]");
	printf(" [-h sweepincrsize]\n");
}

static char		*subchar(char *arg, int toSub)
{
	return (arg + toSub);
}

static char		*zero_at(char *arg, int where)
{
	char *tmp = ft_strdup(arg);
	tmp[where] = '\0';
	return tmp;
}

static BOOL		two_args(char *flag)
{
	if (ft_strlen(flag) == 1)
		return (has_argument(flag[0]));
	return (FALSE);
}

int				main(int argc, char **argv)
{
	int		i;
	BOOL	host_set;
	t_data	*data;

	data = get_data();
	host_set = FALSE;
	i = 1;
	if (argc >= 2)
	{
		while (argv[i])
		{
			if (argv[i][0] == '-')
			{
				if (host_set == TRUE)
				{
					print_usage();
					exit(0);
				}
				else
				{
					if (two_args(subchar(argv[i], 1)))
					{
						if ((i + 1) < argc)
							add_flag(subchar(argv[i], 1), argv[i + 1]);
						else
						{
							print_usage();
							exit(0);
						}
						i++;
					}
					else
					{
						if (has_argument(subchar(argv[i], 1)[0]))
							add_flag(zero_at(subchar(argv[i], 1), 1), subchar(argv[i], 2));
						else
							add_flag(subchar(argv[i], 1), NULL);
					}
				}
			}
			else
			{
				set_host(argv[i]);
				host_set = TRUE;
			}
			i++;
		}
		parse_data();
	}
	else
		print_usage();
	return (0);
}
