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

#include "libskein_utils.h"

void libskein_bytesToWords(uint64_t *W,
			   const char *bytes,
			   const size_t bytes_size)
{
  if(!W || !bytes || bytes_size <= 0)
    return;

  size_t i = 0;

  for(i = 0; i < bytes_size / 8; i++)
    {
      char b[8];
      size_t j = 0;

      for(j = 0; j < 8; j++)
	b[j] = bytes[i * 8 + j];

      W[i] = (uint64_t) b[0] |
	((uint64_t) b[1] << 8) |
	((uint64_t) b[2] << 16) |
	((uint64_t) b[3] << 24) |
	((uint64_t) b[4] << 32) |
	((uint64_t) b[5] << 40) |
	((uint64_t) b[6] << 48) |
	((uint64_t) b[7] << 56);
    }
}
