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

static void		print_usage(void)
{
	printf("usage: ft_ping [-v] [-g sweepminsize] [-G sweepmaxsize]");
	printf(" [-h sweepincrsize]\n");
}

static t_flag	*add_to_flags(int i, t_flag *flags)
{
	(void)i;
	return flags;
}

static char		*subchar(char *arg, int toSub)
{
	return (arg + toSub);
}

static char		*zero_at(char *arg, int where)
{
	char *tmp = ft_strdup(arg);
	tmp[where] = '\0';
	// leaks
	return tmp;
}

static BOOL		has_argument(char *flag)
{
	if (flag[0] == FLAG_H || flag[0] == FLAG_g || flag[0] == FLAG_G)
		return (TRUE);
	return (FALSE);
}

static BOOL		two_args(char *flag)
{
	if (ft_strlen(flag) == 1)
		return (has_argument(flag));
	return (FALSE);
}

int				main(int argc, char **argv)
{
	int		i;
	BOOL	host_set;
	t_flag	*flags;

	(void)flags;
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
						printf("flag with separated : (%s) = (%s)\n", subchar(argv[i], 1), argv[i + 1]);
						i++;
					}
					else
					{
						if (has_argument(subchar(argv[i], 1)))
						{
							printf("flag with arg on it: (%s) = (%s)\n", zero_at(subchar(argv[i], 1), 1), subchar(argv[i], 2));
						}
						else
						{
							printf("flag without arg: (%s)\n", subchar(argv[i], 1));
						}
						flags = add_to_flags(i, flags);
					}
				}
			}
			else
			{
				printf("host is set: %s\n", argv[i]);
				host_set = TRUE;
			}
			i++;
		}
	}
	else
		print_usage();
	return (0);
}
