/*
** Copyright (c) Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from skein without specific prior written permission.
**
** SKEIN IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** SKEIN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libskein_ubi.h"

char *libskein_ubi(const char *G,
		   const size_t G_size,
		   const char *M,
		   const size_t M_size,
		   const char *T,
		   const size_t Nb,
		   const size_t bit_count)
{
  /*
  ** Section 3.4.
  */

  char *H = 0;
  char *Mp = 0;
  char *Mpp = 0;
  char *ubi = 0;
  size_t NM = 0;
  size_t i = 0;
  size_t k = 0;
  size_t p = 0;
  uint16_t B = 0;

  if(!G || G_size <= 0 || !M || M_size <= 0 || !T || bit_count <= 0)
    return ubi;

  Mp = (char *) malloc(M_size);

  if(!Mp)
    return ubi;
  else
    memcpy(Mp, M, M_size);

  if((bit_count & 7) != 0)
    {
      B = 1;
      Mp[M_size - 1] = (char) (1 << (7 - (bit_count & 7)));
    }

  NM = M_size;

  if(NM == 0)
    p = Nb;
  else
    p = NM % Nb;

  Mpp = (char *) malloc(NM + p);
  memset(Mpp, 0, NM + p);
  memcpy(Mpp, Mp, NM);
  k = (NM + p) / Nb;

  for(i = 0; i < k; i++)
    {
    }

  free(H);
  free(Mp);
  free(Mpp);
  return ubi;
}
