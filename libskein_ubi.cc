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

extern "C"
{
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
}

#include <algorithm>
#include <new>

#include "libskein_ubi.h"

static const short UBI_TYPE_KEY = 0;
static const short UBI_TYPE_CFG = 4;
static const short UBI_TYPE_PRS = 8;
static const short UBI_TYPE_PK = 12;
static const short UBI_TYPE_KDF = 16;
static const short UBI_TYPE_NON = 20;
static const short UBI_TYPE_MSG = 48;
static const short UBI_TYPE_OUT = 63;

uint64_t *libskein_ubi(const uint64_t *G,
		       const size_t G_size,
		       const char *M,
		       const size_t M_size,
		       const short Type,
		       const size_t Nb,
		       const size_t bit_count)
{
  /*
  ** Section 3.4.
  */

  char *Mp = 0;
  libskein_tweak T(Type);
  size_t NM = 0;
  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  size_t p = 0;
  uint16_t B = 0;
  uint64_t *H = 0;
  uint64_t *Mi = 0;
  uint64_t *Mpp = 0;
  uint64_t *ubi = 0;

  if(!G || G_size <= 0 || !M || M_size <= 0 || bit_count <= 0)
    return ubi;

  H = new (std::nothrow) uint64_t[G_size];

  if(!H)
    goto done;
  else
    memcpy(H, G, G_size);

  Mi = new (std::nothrow) uint64_t[Nb];

  if(!Mi)
    goto done;

  Mp = new (std::nothrow) char[M_size];

  if(!Mp)
    goto done;
  else
    memcpy(Mp, M, M_size);

  if((bit_count & 7) != 0)
    {
      B = 1;
      Mp[M_size - 1] = (char) (1 << (7 - (bit_count & 7)));
    }
  else
    B = 0;

  NM = M_size;

  if(NM == 0)
    p = Nb;
  else if(NM < Nb)
    p = Nb - NM;
  else
    p = NM % Nb;

  Mpp = new (std::nothrow) uint64_t[NM + p]; /*
					     ** Number of bytes in M' plus
					     ** p.
					     */

  if(!Mpp)
    goto done;

  memset(Mpp, 0, NM + p);
  memcpy(Mpp, Mp, NM);
  k = (NM + p) / Nb;

  for(i = 0; i < k; i++)
    {
      T.setPosition((uint64_t) std::min(NM, (i + 1) * Nb));

      for(j = 0; j < Nb; j++)
	Mi[j] = Mpp[j + i * Nb];
    }

 done:
  delete []H;
  delete []Mi;
  delete []Mp;
  delete []Mpp;
  return ubi;
}
