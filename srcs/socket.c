/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmarinh <frmarinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/25 15:57:18 by frmarinh          #+#    #+#             */
/*   Updated: 2017/05/25 15:57:30 by frmarinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "all.h"

static void			set_address_header(void)
{
	t_data		*data;
	int			data_len = 4;

	data = get_data();
	data->header.ip_src.s_addr = INADDR_ANY;
	/*if (!(inet_pton(AF_INET, src_address, &(data->header.ip_src))))
	{
		printf("ft_ping: Can't set source network address\n");
		exit(0);
	}*/
	if (!(inet_pton(AF_INET, data->host, &(data->header.ip_dst))))
	{
		printf("ft_ping: Can't set destination network address\n");
		exit(EXIT_FAILURE);
	}
	data->header.ip_ttl = 255; // Time to live
	data->header.ip_p = IPPROTO_ICMP; // ICMP Protocol
	data->header.ip_v = 4; // IPv4
	data->header.ip_hl = sizeof(struct ip) >> 2; // IPv4 header length (4 bits)
	data->header.ip_tos = 0;
	data->header.ip_len = htons(IP4_HDRLEN + ICMP_HDRLEN + data_len);
	data->header.ip_off = 0;
	data->header.ip_sum = 0; // set checksum to zero to calculate
	data->header.ip_sum = checksum((uint16_t *) &data->header, IP4_HDRLEN); // calculate checksum
}

static void			set_icmp_header(void)
{
	t_data			*data;

	data = get_data();
	data->icmp_header.icmp_type = ICMP_ECHO; // ECHO REQUEST
	data->icmp_header.icmp_code = 0;
	data->icmp_header.icmp_id = htons (1000);
	data->icmp_header.icmp_seq = htons(0);
	data->icmp_header.icmp_cksum = 0; // set checksum to zero to calculate
}

void				start_icmp_connection(void)
{
	t_data				*data;
	uint8_t				*packet;
	int					data_len = 4;
	char				data_send[4];
	struct sockaddr_in	sin;
	SOCKET				sd;
	BOOL				opt;
	//char			src_address[16] = "10.11.10.10\0";

	opt = TRUE;
	data = get_data();
	set_address_header();
	set_icmp_header();
	data_send[0] = 'T';
	data_send[1] = 'e';
	data_send[2] = 's';
	data_send[3] = 't';
	packet = ft_allocate_ustrmem(IP_MAXPACKET);
	ft_memcpy(packet, &data->header, IP4_HDRLEN); // First part IPv4
	ft_memcpy((packet + IP4_HDRLEN), &data->icmp_header, ICMP_HDRLEN); // Upper layer protocol header
	ft_memcpy(packet + + IP4_HDRLEN + ICMP_HDRLEN, data_send, data_len); // ICMP data
	data->icmp_header.icmp_cksum = checksum ((uint16_t *) (packet + IP4_HDRLEN), ICMP_HDRLEN + data_len); // generate checksum

	ft_memset (&sin, 0, sizeof (struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = data->header.ip_dst.s_addr;
	if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) // Raw socket descriptor
	{
		printf("socket() failed : Operation not permitted\n");
		exit(EXIT_FAILURE);
	}
	if (setsockopt (sd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof (opt)) < 0) // set flag so socket expects us to provide IPv4 header.
	{
		printf("setsockopt() failed to set IP_HDRINCL\n");
		exit (EXIT_FAILURE);
	}
	if (sendto (sd, packet, IP4_HDRLEN + ICMP_HDRLEN + data_len, 0, (struct sockaddr *) &sin, sizeof (struct sockaddr)) < 0) // Send packet
	{
		printf("sendto() failed : Can't send raw data");
		exit(EXIT_FAILURE);
	}
	close (sd);
}
