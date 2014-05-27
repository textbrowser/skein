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

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libskein_threefish.h"

static uint64_t to_int(const char *bytes,
		       const size_t length)
{
  if(!bytes || length <= 0)
    return (uint64_t) 0;

  return (*(uint64_t*)(bytes + length));
}

void libskein_threefish1024(char *E,
			    const char *K,
			    const char *T,
			    const char *P)
{
  if(!E || !K || !P || !T)
    return;

  size_t Nr = 80;
  size_t Nw = 16;
  uint64_t k[Nw];
  uint64_t p[Nw];
  uint64_t t[2];

  for(size_t i = 0; i < Nw; i++)
    k[i] = to_int(&K[i * 8], (size_t) 8);

  t[0] = to_int(&T[0], (size_t) 8);
  t[1] = to_int(&T[8], (size_t) 8);

  for(size_t i = 0; i < Nw; i++)
    p[i] = to_int(&P[i * 8], (size_t) 8);

  (void) Nr;
  memset(k, 0, sizeof(k));
  memset(p, 0, sizeof(p));
  memset(t, 0, sizeof(t));
}
