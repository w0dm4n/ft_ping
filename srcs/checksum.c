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

uint8_t				*ft_allocate_ustrmem(int len)
{
  void		*tmp;

  if (len <= 0)
  {
	  fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in ft_allocate_ustrmem().\n", len);
	  exit (EXIT_FAILURE);
  }
  tmp = (uint8_t *) malloc (len * sizeof (uint8_t));
  if (tmp != NULL)
  {
	  ft_memset (tmp, 0, len * sizeof (uint8_t));
	  return (tmp);
  }
  else
  {
	  fprintf (stderr, "ERROR: Cannot allocate memory for array ft_allocate_ustrmem().\n");
	  exit (EXIT_FAILURE);
  }
}

unsigned short		checksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;
 
    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
 
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }
 
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
 
    return (answer);
}
