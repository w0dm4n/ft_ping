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
# include	<sys/socket.h>
# include	<netdb.h>
# include	<arpa/inet.h>
# include	<netinet/ip.h>
# include	<netinet/ip_icmp.h>
# include	<unistd.h>
# include	<sys/time.h>
# include	<signal.h>
# define	BOOL			int
# define	TRUE			1
# define	FALSE			0
# define	FLAG_H			'h'
# define	FLAG_G			'G'
# define	FLAG_g			'g'
# define	FLAG_v			'v'
# define	FLAG_s			's'
# define	SOCKET 			int
# define	IP4_HDRLEN		20			// IPv4 header length
# define	ICMP_HDRLEN		8			// ICMP header length for echo request, excludes data
# define	MAX_RAW_SIZE	1480

#ifdef __unix__
	#if BYTE_ORDER == BIG_ENDIAN
		#define htons(n) (n)
	#else
		#define htons(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
	#endif
#endif

typedef struct		s_flag
{
	char			flag;
	char			*value;
	struct s_flag	*next;
	struct s_flag	*prev;
}					t_flag;

typedef struct		s_data
{
	t_flag					*flags;
	char					*host;
	char					*default_host;
	SOCKET					fd;
	struct ip				*header;
	struct icmp				*icmp_header;
	struct sockaddr_in		sin;
	int						sent;
	int						received;
	int						sequence;
	int						payload;
	float					total_time;
	float					times[4096];
}					t_data;

/*
**	MAIN
*/
void				print_usage(void);
int					hostname_to_ip(char *hostname, char *ip);

/*
**	DATA
*/
t_data				*get_data(void);
void				set_host(char *host);
void				parse_data(void);
void				send_icmp(int sig);
void				print_statistics(int sig);

/*
**	FLAGS
*/
void				set_flags(t_flag *flags);
void				add_flag(char *flag, char *value);
BOOL				has_argument(char flag);
BOOL				is_valid(char flag);
t_flag				*get_flags(char flag);

/*
**	ICMP
*/
void				start_icmp_connection(void);

/*
**	CHECKSUM
*/
unsigned short		checksum(unsigned short *ptr, int nbytes);
uint8_t				*ft_allocate_ustrmem(int len);
t_data				*g_data;
#endif