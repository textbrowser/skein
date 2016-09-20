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

static const uint8_t *Pi = 0;
static const uint8_t *RPi = 0;
static const uint8_t Pi_4[4] = {0, 3, 2, 1};
static const uint8_t Pi_8[8] = {2, 1, 4, 7, 6, 5, 0, 3};
static const uint8_t Pi_16[16] = {0, 9, 2, 13, 6, 11, 4, 15,
				  10, 7, 12, 3, 14, 5, 8, 1};
static const uint8_t RPi_4[4] = {0, 3, 2, 1};
static const uint8_t RPi_8[8] = {6, 1, 0, 7, 2, 5, 4, 3};
static const uint8_t RPi_16[16] = {0, 15, 2, 11, 6, 13, 4, 9,
				   14, 1, 8, 5, 10, 3, 12, 7};
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
static void wordsToBytes(char *B,
			 const uint64_t *words,
			 const size_t words_size);

static void bytesToWords(uint64_t *W,
			 const char *bytes,
			 const size_t bytes_size)
{
  if(!W || !bytes || bytes_size == 0)
    return;

  for(size_t i = 0; i < bytes_size / 8; i++)
    {
      char b[8];

      for(size_t j = 0; j < 8; j++)
	b[j] = bytes[i * 8 + j];

      W[i] = static_cast<uint64_t> (b[0] & 0xff) |
	(static_cast<uint64_t> (b[1] & 0xff) << 8) |
	(static_cast<uint64_t> (b[2] & 0xff) << 16) |
	(static_cast<uint64_t> (b[3] & 0xff) << 24) |
	(static_cast<uint64_t> (b[4] & 0xff) << 32) |
	(static_cast<uint64_t> (b[5] & 0xff) << 40) |
	(static_cast<uint64_t> (b[6] & 0xff) << 48) |
	(static_cast<uint64_t> (b[7] & 0xff) << 56);
    }
}

static void mix(const uint64_t x0,
		const uint64_t x1,
		const size_t d,
		const size_t i,
		uint64_t *y0,
		uint64_t *y1,
		const size_t block_size)
{
  if(!y0 || !y1)
    return;

  /*
  ** Section 3.3.1.
  */

  uint64_t r = 0;

  if(block_size == 256)
    r = R_4[d % 8][i];
  else if(block_size == 512)
    r = R_8[d % 8][i];
  else
    r = R_16[d % 8][i];

  *y0 = x0 + x1;

  /*
  ** Please see https://en.wikipedia.org/wiki/Circular_shift.
  */

  *y1 = ((x1 << r) | (x1 >> (64 - r))) ^ *y0;
}

static void mix_inverse(const uint64_t y0,
			const uint64_t y1,
			const size_t d,
			const size_t i,
			uint64_t *x0,
			uint64_t *x1,
			const size_t block_size)
{
  if(!x0 || !x1)
    return;

  /*
  ** Section 3.3.1.
  */

  uint64_t r = 0;

  if(block_size == 256)
    r = R_4[d % 8][i];
  else if(block_size == 512)
    r = R_8[d % 8][i];
  else
    r = R_16[d % 8][i];

  /*
  ** Please see https://en.wikipedia.org/wiki/Circular_shift.
  */

  *x1 = ((y1 ^ y0) >> r) | ((y1 ^ y0) << (64 - r));
  *x0 = y0 - *x1;
}

static void threefish_decrypt(char *D,
			      const char *K,
			      const char *T,
			      const char *C,
			      const size_t C_size,
			      const size_t block_size)
{
  if(!C || C_size == 0 || !D || !K || !T || block_size == 0)
    return;

  /*
  ** The inverse of section 3.3.
  */

  uint64_t C240 = 0x1bd11bdaa9fc1a22;
  uint64_t *k = new uint64_t[Nw + 1];
  uint64_t kNw = C240; // Section 3.3.2.
  uint64_t **s = new uint64_t*[Nr / 4 + 1];
  uint64_t t[3];
  uint64_t *v = new uint64_t[Nw];

  bytesToWords(k, K, C_size);
  bytesToWords(t, T, 16);
  bytesToWords(v, C, C_size);

  for(size_t i = 0; i < Nw; i++)
    kNw ^= k[i]; // Section 3.3.2.

  k[Nw] = kNw;

  for(size_t i = 0; i < Nr / 4 + 1; i++)
    s[i] = new uint64_t[Nw];

  t[2] = t[0] ^ t[1]; // Section 3.3.2.

  /*
  ** Prepare the key schedule, section 3.3.2.
  */

  for(size_t d = 0; d < Nr / 4 + 1; d++)
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

  for(size_t i = 0; i < Nw; i++)
    v[i] -= s[Nr / 4][i];

  for(size_t d = Nr - 1;; d--)
    {
      uint64_t *f = new uint64_t[Nw];

      for(size_t i = 0; i < Nw; i++)
	f[i] = v[RPi[i]];

      for(size_t i = 0; i < Nw / 2; i++)
	{
	  uint64_t x0 = 0;
	  uint64_t x1 = 0;
	  uint64_t y0 = f[i * 2];
	  uint64_t y1 = f[i * 2 + 1];

	  mix_inverse(y0, y1, d, i, &x0, &x1, block_size);
	  v[i * 2] = x0;
	  v[i * 2 + 1] = x1;
	}

      memset(f, 0, sizeof(*f) * static_cast<size_t> (Nw));
      delete []f;

      if(d % 4 == 0)
	for(size_t i = 0; i < Nw; i++)
	  v[i] -= s[d / 4][i];

      if(d == 0)
	break;
    }

  wordsToBytes(D, v, Nw);
  memset(k, 0, sizeof(*k) * static_cast<size_t> (Nw + 1));
  memset(t, 0, sizeof(t));
  memset(v, 0, sizeof(*v) * static_cast<size_t> (Nw));
  delete []k;

  for(size_t i = 0; i < Nr / 4 + 1; i++)
    {
      memset(s[i], 0, sizeof(*s[i]) * static_cast<size_t> (Nw));
      delete []s[i];
    }

  delete []s;
  delete []v;
}

static void threefish_encrypt(char *E,
			      const char *K,
			      const char *T,
			      const char *P,
			      const size_t P_size,
			      const size_t block_size)
{
  if(!E || !K || !T || !P || P_size == 0 || block_size == 0)
    return;

  /*
  ** Section 3.3.
  */

  uint64_t C240 = 0x1bd11bdaa9fc1a22;
  uint64_t *k = new uint64_t[Nw + 1];
  uint64_t kNw = C240; // Section 3.3.2.
  uint64_t **s = new uint64_t*[Nr / 4 + 1];
  uint64_t t[3];
  uint64_t *v = new uint64_t[Nw];

  for(size_t i = 0; i < Nr / 4 + 1; i++)
    s[i] = new uint64_t[Nw];

  bytesToWords(k, K, P_size);
  bytesToWords(t, T, 16);
  bytesToWords(v, P, P_size);

  for(size_t i = 0; i < Nw; i++)
    kNw ^= k[i]; // Section 3.3.2.

  k[Nw] = kNw;
  t[2] = t[0] ^ t[1]; // Section 3.3.2.

  /*
  ** Prepare the key schedule, section 3.3.2.
  */

  for(size_t d = 0; d < Nr / 4 + 1; d++)
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

  for(size_t d = 0; d < Nr; d++)
    {
      if(d % 4 == 0)
	for(size_t i = 0; i < Nw; i++)
	  v[i] += s[d / 4][i];

      uint64_t *f = new uint64_t[Nw];

      for(size_t i = 0; i < Nw / 2; i++)
	{
	  uint64_t x0 = v[i * 2];
	  uint64_t x1 = v[i * 2 + 1];
	  uint64_t y0 = 0;
	  uint64_t y1 = 0;

	  mix(x0, x1, d, i, &y0, &y1, block_size);
	  f[i * 2] = y0;
	  f[i * 2 + 1] = y1;
	}

      for(size_t i = 0; i < Nw; i++)
	v[i] = f[Pi[i]];

      memset(f, 0, sizeof(*f) * static_cast<size_t> (Nw));
      delete []f;
    }

  for(size_t i = 0; i < Nw; i++)
    v[i] += s[Nr / 4][i];

  wordsToBytes(E, v, Nw);
  memset(k, 0, sizeof(*k) * static_cast<size_t> (Nw + 1));
  memset(t, 0, sizeof(t));
  memset(v, 0, sizeof(*v) * static_cast<size_t> (Nw));
  delete []k;

  for(size_t i = 0; i < Nr / 4 + 1; i++)
    {
      memset(s[i], 0, sizeof(*s[i]) * static_cast<size_t> (Nw));
      delete []s[i];
    }

  delete []s;
  delete []v;
}

static void wordsToBytes(char *B,
			 const uint64_t *words,
			 const size_t words_size)
{
  if(!B || !words || words_size == 0)
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

void libskein_threefish_decrypt(char *D,
				const char *K,
				const char *T,
				const char *C,
				const size_t C_size,
				const size_t block_size)
{
  if(!C || C_size == 0 || !D || !K || !T || block_size == 0)
    return;

  if(block_size == 256)
    {
      Nr = 72;
      Nw = 4;
      RPi = RPi_4;
    }
  else if(block_size == 512)
    {
      Nr = 72;
      Nw = 8;
      RPi = RPi_8;
    }
  else if(block_size == 1024)
    {
      Nr = 80;
      Nw = 16;
      RPi = RPi_16;
    }
  else
    return;

  threefish_decrypt(D, K, T, C, C_size, block_size);
}

void libskein_threefish_encrypt(char *E,
				const char *K,
				const char *T,
				const char *P,
				const size_t P_size,
				const size_t block_size)
{
  if(!E || !K || !P || P_size == 0 || !T || block_size == 0)
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

  threefish_encrypt(E, K, T, P, P_size, block_size);
}
