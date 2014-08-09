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

#include "libskein_skein.h"

static const short UBI_TYPE_KEY = 0;
static const short UBI_TYPE_CFG = 4;
static const short UBI_TYPE_PRS = 8;
static const short UBI_TYPE_PK = 12;
static const short UBI_TYPE_KDF = 16;
static const short UBI_TYPE_NON = 20;
static const short UBI_TYPE_MSG = 48;
static const short UBI_TYPE_OUT = 63;
static const uint8_t *Pi = 0;
static const uint8_t Pi_4[4] = {0, 3, 2, 1};
static const uint8_t Pi_8[8] = {2, 1, 4, 7, 6, 5, 0, 3};
static const uint8_t Pi_16[16] = {0, 9, 2, 13, 6, 11, 4, 15,
				  10, 7, 12, 3, 14, 5, 8, 1};
static const uint8_t R_4[8][2] = {{14, 16},
				  {52, 57},
				  {23, 40},
				  {5, 37},
				  {25, 33},
				  {46, 12},
				  {58, 22},
				  {32, 32}};
static const uint8_t R_8[8][4] = {{46, 36, 19, 37},
				  {33, 27, 14, 42},
				  {17, 49, 36, 39},
				  {44, 9, 54, 56},
				  {39, 30, 34, 24},
				  {13, 50, 10, 17},
				  {25, 29, 39, 43},
				  {8, 35, 56, 22}};
static const uint8_t R_16[8][8] = {{24, 13, 8, 47, 8, 17, 22, 37},
				   {38, 19, 10, 55, 49, 18, 23, 52},
				   {33, 4, 51, 13, 34, 41, 59, 17},
				   {5, 20, 48, 41, 47, 28, 16, 25},
				   {41, 9, 37, 31, 12, 47, 44, 30},
				   {16, 34, 56, 51, 4, 53, 42, 41},
				   {31, 44, 47, 46, 19, 42, 44, 25},
				   {9, 48, 35, 52, 23, 31, 37, 20}};
static size_t Nr = 0;
static size_t Nw = 0;
static void purge(void *buffer, const size_t buffer_size);
static void wordsToBytes(char *B,
			 const uint64_t *words,
			 const size_t words_size);

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
  (void) B;

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

static void bytesToWords(uint64_t *W,
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

static void mix(const uint64_t x0,
		const uint64_t x1,
		const size_t d,
		const size_t j,
		uint64_t *y0,
		uint64_t *y1,
		const size_t block_size)
{
  if(!y0 || !y1)
    return;

  /*
  ** Section 3.3.1.
  */

  *y0 = x0 + x1;

  /*
  ** Please see https://en.wikipedia.org/wiki/Circular_shift.
  */

  if(block_size == 256)
    *y1 = ((x1 << R_4[d % 8][j]) | (x1 >> (64 - R_4[d % 8][j]))) ^ *y0;
  else if(block_size == 512)
    *y1 = ((x1 << R_8[d % 8][j]) | (x1 >> (64 - R_8[d % 8][j]))) ^ *y0;
  else
    *y1 = ((x1 << R_16[d % 8][j]) | (x1 >> (64 - R_16[d % 8][j]))) ^ *y0;
}

static void purge(void *buffer,
		  const size_t buffer_size)
{
  if(!buffer || buffer_size <= 0)
    return;

  char *b = (char *) buffer;
  size_t bs = buffer_size;

  do
    {
      *b = 0;
      b += 1;
      bs -= 1;
    }
  while(bs > 0);
}

static void threefish_E(char *E,
			const char *K,
			const char *T,
			const char *P,
			const size_t block_size)
{
  /*
  ** Section 3.3.
  */

  if(!E || !K || !T || !P || block_size <= 0)
    return;

  size_t d = 0;
  size_t i = 0;
  size_t j = 0;
  uint64_t C240 = 0x1bd11bdaa9fc1a22;
  uint64_t c[Nw];
  uint64_t k[Nw + 1];
  uint64_t kNw = C240; // Section 3.3.2.
  uint64_t p[Nw];
  uint64_t s[Nr / 4 + 1][Nw];
  uint64_t t[3];
  uint64_t v[Nw];

  bytesToWords(k, K, (size_t) block_size / 8);
  bytesToWords(p, P, (size_t) block_size / 8);
  bytesToWords(t, T, (size_t) 16);

  for(i = 0; i < Nw; i++)
    kNw ^= k[i]; // Section 3.3.2.

  k[Nw] = kNw;

  for(i = 0; i < Nw; i++)
    v[i] = p[i];

  t[2] = t[0] ^ t[1]; // Section 3.3.2.

  /*
  ** Prepare the key schedule, section 3.3.2.
  */

  for(d = 0; d < Nr / 4 + 1; d++) // d rounds.
    for(i = 0; i < Nw; i++)
      {	
	s[d][i] = k[(d + i) % (Nw + 1)];

	if(i == Nw - 1)
	  s[d][i] += d;
	else if(i == Nw - 2)
	  s[d][i] += t[(d + 1) % 3];
	else if(i == Nw - 3)
	  s[d][i] += t[d % 3];
      }

  for(d = 0; d < Nr; d++) // d rounds.
    {
      uint64_t e[Nw];

      for(i = 0; i < Nw; i++)
	e[i] = (d % 4 == 0) ? v[i] + s[d / 4][i] : v[i];

      uint64_t f[Nw];

      for(j = 0; j < Nw / 2; j++)
	{
	  uint64_t x0 = e[j * 2];
	  uint64_t x1 = e[j * 2 + 1];
	  uint64_t y0 = 0;
	  uint64_t y1 = 0;

	  mix(x0, x1, d, j, &y0, &y1, block_size);
	  f[j * 2] = y0;
	  f[j * 2 + 1] = y1;
	}

      for(i = 0; i < Nw; i++)
	v[i] = f[Pi[i]];

      purge(e, sizeof(e));
      purge(t, sizeof(t));
    }

  for(i = 0; i < Nw; i++)
    c[i] = v[i] + s[Nr / 4][i];

  wordsToBytes(E, c, Nw);
  purge(c, sizeof(c));
  purge(k, sizeof(k));
  purge(p, sizeof(p));
  purge(s, sizeof(s));
  purge(t, sizeof(t));
  purge(v, sizeof(v));
}

static void wordsToBytes(char *B,
			 const uint64_t *words,
			 const size_t words_size)
{
  if(!B || !words)
    return;

  size_t i = 0;

  for(i = 0; i < words_size; i++)
    {
      B[i * 8 + 0] = (char) words[i];
      B[i * 8 + 1] = (char) ((words[i] >> 8) & 0xff);
      B[i * 8 + 2] = (char) ((words[i] >> 16) & 0xff);
      B[i * 8 + 3] = (char) ((words[i] >> 24) & 0xff);
      B[i * 8 + 4] = (char) ((words[i] >> 32) & 0xff);
      B[i * 8 + 5] = (char) ((words[i] >> 40) & 0xff);
      B[i * 8 + 6] = (char) ((words[i] >> 48) & 0xff);
      B[i * 8 + 7] = (char) ((words[i] >> 56) & 0xff);
    }
}

void libskein_threefish(char *E,
			const char *K,
			const char *T,
			const char *P,
			const size_t block_size)
{
  if(!E || !K || !P || !T || block_size <= 0)
    return;

  if(block_size == 256)
    {
      Nr = 72;
      Nw = 4;
      Pi = Pi_4;
    }
  else if(block_size == 512)
    {
      Nr = 72;
      Nw = 8;
      Pi = Pi_8;
    }
  else if(block_size == 1024)
    {
      Nr = 80;
      Nw = 16;
      Pi = Pi_16;
    }
  else
    return;

  threefish_E(E, K, T, P, block_size);
}
