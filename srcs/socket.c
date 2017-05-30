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

void				add_to_times(float new_time)
{
	t_data		*data;
	int			i;

	i = 0;
	data = get_data();
	while (data->times[i])
		i++;
	data->times[i] = new_time;
}

float				get_min(void)
{
	float		min;
	int			i;
	t_data		*data;

	data = get_data();
	i = 0;
	min = data->times[0];
	while (data->times[i])
	{
		if (data->times[i] < min)
			min = data->times[i];
		i++;
	}
	return (min);
}

float				get_average(void)
{
	float		total;
	int			i;
	t_data		*data;

	data = get_data();
	i = 0;
	total = 0.00;
	while (data->times[i])
		total += data->times[i++];
	return (total / i);
}

float				get_max(void)
{
	float		max;
	int			i;
	t_data		*data;

	data = get_data();
	i = 0;
	max = data->times[0];
	while (data->times[i])
	{
		if (data->times[i] > max)
			max = data->times[i];
		i++;
	}
	return (max);
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
	printf("round-trip min/avg/max = %.3f/%.3f/%.3f ms\n", get_min(), get_average(), get_max());
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
		if (get_flags(FLAG_v) != NULL && received < data->last_sent)
			printf("packet too short (%d bytes) from %s\n", received, data->host);
		else
		{
			if (data->default_host != data->host)
				printf("%d bytes from %s (%s): seq=%d ttl=%d time=%.3f ms\n", (received - 20), data->default_host, data->host, \
				data->sequence, ip_header->ip_ttl, time_value);
			else
				printf("%d bytes from %s: seq=%d ttl=%d time=%.3f ms\n", (received - 20), data->host,\
				data->sequence, ip_header->ip_ttl, time_value);
		}
		data->total_time += time_value;
		data->received++;
		add_to_times(time_value);
	}
	data->sequence++;
}

void				check_flags(t_data *data)
{
	t_flag				*h;
	t_flag				*G;
	int					G_value = 0;
	if ((h = get_flags(FLAG_H)) != NULL)
		data->payload += ft_atoi(h->value);
	if (data->payload > (MAX_RAW_SIZE - sizeof(struct icmp*)))
		data->payload = (MAX_RAW_SIZE - sizeof(struct icmp*));
}

void				send_with_address_header_icmp(void)
{
	int					packet_size = 0, G_value = 0;
	t_flag				*G;
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
	if ((data->last_sent = sendto(data->fd, packet, packet_size, 0, (struct sockaddr *)&data->sin, sizeof(struct sockaddr))) < 0) // Send packet
	{
		printf("sendto() failed : Can't send raw data\n");
		exit(EXIT_FAILURE);
	}
	check_flags(data);
	gettimeofday (&before, NULL);
	data->sent++;
	receive_data(data, before);
	if ((G = get_flags(FLAG_G)) != NULL)
	{
		G_value = ft_atoi(G->value);
		if (data->payload > G_value)
			print_statistics(0);
	}
	if (packet_size > MAX_RAW_SIZE)
	{
		print_statistics(0);
		return ;
	}
}

void				send_without_address_header_icmp(void)
{
	t_data					*data;
	void					*buffer = NULL;
	int						packet_size = 0, G_value = 0;
	struct timeval			before;
	t_flag					*G;

	data = get_data();
	packet_size = sizeof (struct ip) + sizeof (struct icmp) + data->payload;
	if (!(buffer = (char *)malloc(packet_size)))
		return ;
	ft_memset(buffer, 0, packet_size);
	set_icmp_header(buffer);

	ft_memset (&data->sin, 0, sizeof (struct sockaddr_in));
	data->sin.sin_family = AF_INET;
	inet_pton(AF_INET, data->host, &(data->sin.sin_addr));

	struct sockaddr *addr = (struct sockaddr*) &data->sin;
	data->icmp_header->icmp_cksum = checksum((unsigned short *)data->icmp_header, sizeof(struct icmp));
	data->last_sent = sendto(data->fd, data->icmp_header, sizeof(data->icmp_header) + data->payload, MSG_DONTWAIT, addr, sizeof(*addr));
	if (data->last_sent == -1)
	{
		printf("sendto() failed : Can't send raw data.\n");
		exit(EXIT_FAILURE);
	}
	check_flags(data);
	gettimeofday (&before, NULL);
	data->sent++;
	receive_data(data, before);
	if ((G = get_flags(FLAG_G)) != NULL)
	{
		G_value = ft_atoi(G->value);
		if (data->payload > G_value)
			print_statistics(0);
	}
}

void				send_icmp(int sig)
{
	#ifdef __linux__
	if (get_flags(FLAG_H) != NULL || get_flags(FLAG_s))
			send_without_address_header_icmp();
		else
			send_with_address_header_icmp();
	#else
		send_without_address_header_icmp();
	#endif
	alarm(1);
}

void				set_payload(t_data *data)
{
	t_flag	*s = get_flags(FLAG_s);
	t_flag	*h = get_flags(FLAG_H);
	t_flag	*g = get_flags(FLAG_g);

	if (g)
		data->payload = ft_atoi(g->value);
	else if (h)
		data->payload = 0;
	else if (s)
		data->payload = ft_atoi(s->value);
	else
	{
		#ifdef __linux__
			data->payload = 36;
		#else
			data->payload = 56;
		#endif
	}
}

void				start_icmp_connection(void)
{
	t_data				*data;
	BOOL				opt;
	t_flag				*h;
	t_flag				*s;
	opt = TRUE;
	data = get_data();

	check_flags(data);
	#ifdef __linux__ // LINUX
		if ((data->fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) // Raw socket descriptor
		{
			printf("socket() failed : Operation not permitted\n");
			exit(EXIT_FAILURE);
		}
		if ((get_flags(FLAG_H) == NULL && get_flags(FLAG_s) == NULL) && (setsockopt(data->fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt))) < 0) // set flag so socket expects us to provide IPv4 header.
		{
			printf("setsockopt() failed to set IP_HDRINCL\n");
			exit (EXIT_FAILURE);
		}
	#else // OSX
		if ((data->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0) // Raw socket descriptor
		{
			printf("socket() failed : Operation not permitted\n");
			exit(EXIT_FAILURE);
		}
	#endif

	if (setsockopt(data->fd, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt)) < 0) // allow socket to send datagrams to broadcast addresses
    {
		printf("setsockopt() failed to set SO_BROADCAST\n");
		exit (EXIT_FAILURE);
	}
	if (get_flags(FLAG_H) != NULL && get_flags(FLAG_s) != NULL)
	{
		printf("ft_ping: Packet size and ping sweep are mutually exclusive\n");
		exit(0);
	}
	set_payload(data);
	if ((h = get_flags(FLAG_H)) == NULL && (s = get_flags(FLAG_s)) == NULL)
		printf("FT_PING %s (%s) 56 data bytes\n", data->default_host, data->host);
	else
	{
		if (h)
			printf("FT_PING %s (%s) %d data bytes\n", data->default_host, data->host, ft_atoi(h->value));
		else if (s)
			printf("FT_PING %s (%s) %d data bytes\n", data->default_host, data->host, ft_atoi(s->value));
	}
	send_icmp(0);
	while (1);
}
