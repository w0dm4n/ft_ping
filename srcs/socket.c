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

static void			set_address_header(void *packet, int packet_size)
{
	t_data		*data;

	data = get_data();
	data->header = (struct ip *) packet;

	data->header->ip_src.s_addr = INADDR_ANY;
	if (!(inet_pton(AF_INET, data->host, &(data->header->ip_dst))))
	{
		printf("ft_ping: Can't set destination network address\n");
		exit(EXIT_FAILURE);
	}
	data->header->ip_ttl = 64; // Time to live
	data->header->ip_p = IPPROTO_ICMP; // ICMP Protocol
	data->header->ip_v = 4; // IPv4
	data->header->ip_hl = sizeof(struct ip) >> 2; // IPv4 header length (4 bits)
	//data->header->ip_hl = 5;
	data->header->ip_tos = 0;
	data->header->ip_len = htons(packet_size);
	data->header->ip_off = 0;
	data->header->ip_sum = 0; // set checksum to zero to calculate
	data->header->ip_sum = checksum((uint16_t *) &data->header, IP4_HDRLEN); // calculate checksum
}

static void			set_icmp_header(void *packet)
{
	t_data			*data;
	int				sequence;

	data = get_data();
	data->icmp_header = (struct icmp *) (packet + sizeof (struct ip));
	data->icmp_header->icmp_type = ICMP_ECHO; // ECHO REQUEST
	data->icmp_header->icmp_code = 0; // 8 bits echo request
	data->icmp_header->icmp_id = htons (getpid()); // identifier usually the pid of the current process - rand number
	data->icmp_header->icmp_seq = htons(data->sequence);
	data->icmp_header->icmp_cksum = 0; // set checksum to zero to calculate
}

void				print_statistics(int sig)
{
	t_data		*data;
	int			percent;

	data = get_data();
	percent = (data->received * 100) / data->sent;
	percent = 100 - percent;
	printf("\n--- %s ping statistics ---\n", data->default_host);
	if (data->total_time > 0)
		printf("%d packets transmitted, %d received, %d%c packet loss, time %.3fms\n", data->sent, data->received, percent, '%', \
		data->total_time);
	else
		printf("%d packets transmitted, %d received, 100%c packet loss\n", data->sent, data->received, '%');
	exit(0);
}

void				receive_data(t_data *data, struct timeval before)
{
	int					received;
	char				buffer[4096];
	struct iovec		iov;
	struct msghdr		msg;
	struct ip			*ip_header;
	struct timeval		after;
	float				ms_time;

	iov.iov_base = &buffer;
	iov.iov_len = sizeof(buffer);
	msg.msg_name = (void*)&data->sin;
	msg.msg_namelen = sizeof(data->sin);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	received = recvmsg(data->fd, &msg, 0);
	if (received > 0)
	{
		ip_header = (struct ip*) iov.iov_base;
		gettimeofday (&after, NULL);
		ms_time = ((after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec) - before.tv_usec;
		float time_value = ms_time / 1000;
		if (data->default_host != data->host)
			printf("%d bytes from %s (%s): seq=%d ttl=%d time=%.3f ms\n", (received - 20), data->default_host, data->host, \
			data->sequence, ip_header->ip_ttl, time_value);
		else
			printf("%d bytes from %s: seq=%d ttl=%d time=%.3f ms\n", (received - 20), data->host,\
			data->sequence, ip_header->ip_ttl, time_value);
		data->total_time += time_value;
		data->received++;
	}
	data->sequence++;
}

void				check_flags(t_data *data)
{
	t_flag				**flags;
	if ((flags[0] = get_flags(FLAG_H)) != NULL)
	{
		data->payload += ft_atoi(flags[0]->value);
	}
}

void				send_icmp(int sig)
{ 
	int					packet_size = 0;
	char				*packet;
	t_data				*data;
	struct timeval		before;

	data = get_data();
	packet_size = sizeof (struct ip) + sizeof (struct icmp) + data->payload;
	if (!(packet = (char *)malloc(packet_size)))
		return ;
	ft_memset(packet, 0, packet_size);
	set_address_header(packet, packet_size);
	set_icmp_header(packet);

	ft_memset (&data->sin, 0, sizeof (struct sockaddr_in));
	data->sin.sin_family = AF_INET;
	data->sin.sin_addr.s_addr = data->header->ip_dst.s_addr;

	ft_memset(packet + sizeof(struct ip) + sizeof(struct icmp), 0, data->payload);
	data->icmp_header->icmp_cksum = checksum((unsigned short *)data->icmp_header, sizeof(struct icmp) + data->payload);
	if (sendto(data->fd, packet, packet_size, 0, (struct sockaddr *)&data->sin, sizeof(struct sockaddr)) < 0) // Send packet
	{
		printf("sendto() failed : Can't send raw data\n");
		exit(EXIT_FAILURE);
	}
	check_flags(data);
	gettimeofday (&before, NULL);
	data->sent++;
	receive_data(data, before);
	if (packet_size > MAX_RAW_SIZE)
	{
		print_statistics(0);
		return ;
	}
	alarm(1);
}

void				start_icmp_connection(void)
{
	t_data				*data;
	BOOL				opt;
	opt = TRUE;
	data = get_data();

	if ((data->fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) // Raw socket descriptor
	{
		printf("socket() failed : Operation not permitted\n");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(data->fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0) // set flag so socket expects us to provide IPv4 header.
	{
		printf("setsockopt() failed to set IP_HDRINCL\n");
		exit (EXIT_FAILURE);
	}
	if (setsockopt(data->fd, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt)) < 0) // allow socket to send datagrams to broadcast addresses
    {
		printf("setsockopt() failed to set SO_BROADCAST\n");
		exit (EXIT_FAILURE);
	}
	printf("FT_PING %s (%s) 56 data bytes\n", data->default_host, data->host);
	send_icmp(0);
	while (1);
}
