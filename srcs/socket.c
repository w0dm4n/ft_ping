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
	data->header->ip_ttl = 255; // Time to live
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

	data = get_data();
	data->icmp_header = (struct icmp *) (packet + sizeof (struct ip));
	data->icmp_header->icmp_type = ICMP_ECHO; // ECHO REQUEST
	data->icmp_header->icmp_code = 0; // 8 bits echo request
	data->icmp_header->icmp_id = htons (getpid()); // identifier usually the pid of the current process - rand number
	data->icmp_header->icmp_seq = htons(get_data()->sequence);
	data->icmp_header->icmp_cksum = 0; // set checksum to zero to calculate
}

void				start_icmp_connection(void)
{
	t_data				*data;
	struct sockaddr_in	sin;
	BOOL				opt;
	int					payload_size = 36;
	int					packet_size = sizeof (struct ip) + sizeof (struct icmp) + payload_size;
	char				*packet;
	opt = TRUE;
	data = get_data();
	packet = NULL;
	
	if (!(packet = (char *)malloc(packet_size)))
		return ;
	ft_memset(packet, 0, packet_size);

	set_address_header(packet, packet_size);
	set_icmp_header(packet);

	ft_memset (&sin, 0, sizeof (struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = data->header->ip_dst.s_addr;

	ft_memset(packet + sizeof(struct ip) + sizeof(struct icmp), 255, payload_size);
	data->icmp_header->icmp_cksum = checksum((unsigned short *)data->icmp_header, sizeof(struct icmp) + payload_size);
	if ((data->fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) // Raw socket descriptor
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
	while (TRUE)
	{
		if (sendto(data->fd, packet, packet_size, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0) // Send packet
		{
			printf("sendto() failed : Can't send raw data");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("FT_PING %s (%s) 56(84) bytes of data.\n", data->default_host, data->host);
		
			int return_status;
			char buffer[4096];
			struct iovec iov[1];                       /* Data array */
			struct msghdr *msg = malloc(sizeof(struct msghdr));                      /* Message header */
			//struct cmsghdr *cmsg;                   /* Control related data */
			//struct icmp icmph;                   /* ICMP header */

				iov[0].iov_base = &buffer;
				iov[0].iov_len = sizeof(buffer);
				msg->msg_name = (void*)&sin;
				msg->msg_namelen = sizeof(sin);
				msg->msg_iov = iov;
				msg->msg_iovlen = 1;
				msg->msg_control = 0;
				msg->msg_controllen = 0;
			return_status = recvmsg(data->fd, msg, 0);
			printf("return : %d\n", return_status);
		}
	}
	close (data->fd);
}
