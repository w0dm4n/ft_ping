/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checksum.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmarinh <frmarinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/25 17:15:06 by frmarinh          #+#    #+#             */
/*   Updated: 2017/05/25 17:15:13 by frmarinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "all.h"

unsigned short		checksum(unsigned short *ptr, int nbytes)
{
	
    register long		sum;
    unsigned short				oddbyte;
    register unsigned short	answer;

	sum = 0;
    while (nbytes > 1)
	{
		sum += *ptr++;
		nbytes -= 2;
    }
    if (nbytes == 1)
	{
		oddbyte = 0;
		*((unsigned char *) & oddbyte) = *(unsigned char *) ptr;
		sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}
