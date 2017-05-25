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

uint16_t			checksum(uint16_t *addr, int len)
{
  int count = len;
  register uint32_t sum = 0;
  uint16_t answer = 0;

  // Sum up 2-byte values until none or only one byte left.
  while (count > 1) {
	sum += *(addr++);
	count -= 2;
  }

  // Add left-over byte, if any.
  if (count > 0) {
	sum += *(uint8_t *) addr;
  }

  // Fold 32-bit sum into 16 bits; we lose information by doing this,
  // increasing the chances of a collision.
  // sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)
  while (sum >> 16) {
	sum = (sum & 0xffff) + (sum >> 16);
  }

  // Checksum is one's compliment of sum.
  answer = ~sum;
  return (answer);
}
