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
** LIBSKEIN IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** LIBSKEIN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
static void bytesToWords(uint64_t *W,
			 const char *bytes,
			 const size_t bytes_size);
static void purge(void *buffer, const size_t buffer_size);
static void wordsToBytes(char *B,
			 const uint64_t *words,
			 const size_t words_size);

static uint64_t *ubi(const uint64_t *G,
		     const char *M,
		     const size_t M_size,
		     const short Type,
		     const size_t Nb,
		     const size_t block_size)
{
  if(!G || !M || M_size <= 0 || Nb <= 0 || block_size <= 0)
    return 0;

  /*
  ** Section 3.4.
  */

  char *Mp = 0;
  libskein_tweak Tweak(Type);
  size_t NM = M_size;
  size_t bit_count = 8 * M_size;
  size_t k = std::max(static_cast<size_t> (1), (NM + Nb - 1) / Nb);
  uint16_t B = 0;
  uint64_t *H = 0;
  uint64_t *Mi = 0;
  uint64_t *Mpp = 0;

  H = new (std::nothrow) uint64_t[Nb];

  if(!H)
    goto done;
  else
    memcpy(H, G, Nb);

  Mi = new (std::nothrow) uint64_t[Nb / 8];

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
      Mp[M_size - 1] = static_cast<char> ((1 << (7 - (bit_count & 7))));
    }
  else
    B = 0;

  Mpp = new (std::nothrow) uint64_t[k * Nb / 8];

  if(!Mpp)
    goto done;

  bytesToWords(Mpp, Mp, M_size);

  for(size_t i = 0; i < k; i++)
    {
      Tweak.setPosition(std::min(NM, (i + 1) * Nb));

      if(i == 0)
	Tweak.setFirst(true);
      else
	Tweak.setFirst(false);

      if(i == k - 1)
	Tweak.setLast(true);
      else
	Tweak.setLast(false);

      if(Tweak.isLast())
	{
	  if(B == 0)
	    Tweak.setPadded(false);
	  else
	    Tweak.setPadded(true);
	}

      for(size_t j = 0; j < Nb / 8; j++)
	Mi[j] = Mpp[j + i * Nb / 8];

      char E[Nb];
      char K[8 * Nb];
      char P[Nb];
      char T[16];

      wordsToBytes(K, H, Nb);
      wordsToBytes(P, Mi, Nb / 8);
      wordsToBytes(T, Tweak.value(), 2);
      libskein_threefish(E, K, T, P, block_size);
      bytesToWords(Mi, E, Nb);

      for(size_t j = 0; j < Nb / 8; j++)
	H[j] = Mi[j] ^ Mpp[j + i * Nb / 8];

      purge(E, sizeof(E));
      purge(K, sizeof(K));
      purge(P, sizeof(P));
      purge(T, sizeof(T));
    }

 done:
  delete []Mi;
  delete []Mp;
  delete []Mpp;
  return H;
}

static void bytesToWords(uint64_t *W,
			 const char *bytes,
			 const size_t bytes_size)
{
  if(!W || !bytes || bytes_size <= 0)
    return;

  for(size_t i = 0; i < bytes_size / 8; i++)
    {
      char b[8];

      for(size_t j = 0; j < 8; j++)
	b[j] = bytes[i * 8 + j];

      W[i] = static_cast<uint64_t> (b[0]) |
	(static_cast<uint64_t> (b[1]) << 8) |
	(static_cast<uint64_t> (b[2]) << 16) |
	(static_cast<uint64_t> (b[3]) << 24) |
	(static_cast<uint64_t> (b[4]) << 32) |
	(static_cast<uint64_t> (b[5]) << 40) |
	(static_cast<uint64_t> (b[6]) << 48) |
	(static_cast<uint64_t> (b[7]) << 56);
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

  char *b = static_cast<char *> (buffer);

  for(size_t i = 0; i < buffer_size; i++)
    {
      *b = 0;
      b += 1;
    }
}

static void threefish(char *E,
		      const char *K,
		      const char *T,
		      const char *P,
		      const size_t block_size)
{
  if(!E || !K || !T || !P || block_size <= 0)
    return;

  /*
  ** Section 3.3.
  */

  uint64_t C240 = 0x1bd11bdaa9fc1a22;
  uint64_t c[Nw];
  uint64_t k[Nw + 1];
  uint64_t kNw = C240; // Section 3.3.2.
  uint64_t p[Nw];
  uint64_t s[Nr / 4 + 1][Nw];
  uint64_t t[3];
  uint64_t v[Nw];

  bytesToWords(k, K, block_size / 8);
  bytesToWords(p, P, block_size / 8);
  bytesToWords(t, T, 16);

  for(size_t i = 0; i < Nw; i++)
    kNw ^= k[i]; // Section 3.3.2.

  k[Nw] = kNw;

  for(size_t i = 0; i < Nw; i++)
    v[i] = p[i];

  t[2] = t[0] ^ t[1]; // Section 3.3.2.

  /*
  ** Prepare the key schedule, section 3.3.2.
  */

  for(size_t d = 0; d < Nr / 4 + 1; d++) // d rounds.
    for(size_t i = 0; i < Nw; i++)
      {	
	s[d][i] = k[(d + i) % (Nw + 1)];

	if(i == Nw - 1)
	  s[d][i] += d;
	else if(i == Nw - 2)
	  s[d][i] += t[(d + 1) % 3];
	else if(i == Nw - 3)
	  s[d][i] += t[d % 3];
      }

  for(size_t d = 0; d < Nr; d++) // d rounds.
    {
      uint64_t e[Nw];

      for(size_t i = 0; i < Nw; i++)
	e[i] = (d % 4 == 0) ? v[i] + s[d / 4][i] : v[i];

      uint64_t f[Nw];

      for(size_t j = 0; j < Nw / 2; j++)
	{
	  uint64_t x0 = e[j * 2];
	  uint64_t x1 = e[j * 2 + 1];
	  uint64_t y0 = 0;
	  uint64_t y1 = 0;

	  mix(x0, x1, d, j, &y0, &y1, block_size);
	  f[j * 2] = y0;
	  f[j * 2 + 1] = y1;
	}

      for(size_t i = 0; i < Nw; i++)
	v[i] = f[Pi[i]];

      purge(e, sizeof(e));
      purge(t, sizeof(t));
    }

  for(size_t i = 0; i < Nw; i++)
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

  for(size_t i = 0; i < words_size; i++)
    {
      B[i * 8 + 0] = static_cast<char> (words[i]);
      B[i * 8 + 1] = static_cast<char> ((words[i] >> 8) & 0xff);
      B[i * 8 + 2] = static_cast<char> ((words[i] >> 16) & 0xff);
      B[i * 8 + 3] = static_cast<char> ((words[i] >> 24) & 0xff);
      B[i * 8 + 4] = static_cast<char> ((words[i] >> 32) & 0xff);
      B[i * 8 + 5] = static_cast<char> ((words[i] >> 40) & 0xff);
      B[i * 8 + 6] = static_cast<char> ((words[i] >> 48) & 0xff);
      B[i * 8 + 7] = static_cast<char> ((words[i] >> 56) & 0xff);
    }
}

void libskein_simplehash(char *H,
			 const size_t Nb,
			 const size_t No,
			 const char *M,
			 const size_t M_size,
			 const size_t block_size)
{
  if(!H || !M || M_size <= 0 || Nb <= 0 || No <= 0 || block_size <= 0)
    return;

  if(!(Nb == 32 || Nb == 64 || Nb == 128))
    return;

  /*
  ** Section 3.5.
  */

  char C[32];
  uint64_t *G0 = 0;
  uint64_t *G1 = 0;
  uint64_t K_p[Nb];

  memset(C, 0, sizeof(C));
  memcpy(C, "SHA3", 4);
  C[4] = 1;
  memcpy(&C[8], &No, 8);
  memset(K_p, 0, Nb);
  G0 = ubi(K_p, C, sizeof(C), UBI_TYPE_CFG, Nb, block_size);
  G1 = ubi(G0, M, M_size, UBI_TYPE_MSG, Nb, block_size);
  delete []G0;
  delete []G1;
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

  threefish(E, K, T, P, block_size);
}
