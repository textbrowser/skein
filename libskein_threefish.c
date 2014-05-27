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

#include "libskein_threefish.h"

static size_t Nr = 80;
static size_t Nw = 16;
static uint64_t Pi[16] = {0, 9, 2, 13, 6, 11, 4, 15,
			  10, 7, 12, 3, 14, 5, 8, 1};
static uint64_t R[8][8] = {{24, 13, 8, 47, 8, 17, 22, 37},
			   {38, 19, 10, 55, 49, 18, 23, 52},
			   {33, 4, 51, 13, 34, 41, 59, 17},
			   {5, 20, 48, 41, 47, 28, 16, 25},
			   {41, 9, 37, 31, 12, 47, 44, 30},
			   {16, 34, 56, 51, 4, 53, 42, 41},
			   {31, 44, 47, 46, 19, 42, 44, 25},
			   {9, 48, 35, 52, 23, 31, 37, 20}};
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
  if(!W || !bytes || bytes_size <= 0)
    return;

  uint64_t w = 0;

  for(size_t i = 0, j = 0; i < bytes_size; i++)
    {
      w |= ((uint64_t) (bytes[i] & 0xff)) << (i * 8);

      if((i - 7) % 8 == 0)
	{
	  W[j] = w;
	  w = 0;
	  j += 1;
	}
    } 
}

static void mix(const uint64_t x0,
		const uint64_t x1,
		const size_t d,
		const size_t j,
		uint64_t *y0,
		uint64_t *y1)
{
  /*
  ** Section 3.3.1.
  */

  if(d <= 0 || (d % 8) > 7)
    return;
  else if(j <= 0 || j > 7)
    return;
  else if(!y0 || !y1)
    return;

  *y0 = x0 + x1;

  /*
  ** Please see https://en.wikipedia.org/wiki/Circular_shift.
  */

  *y1 = ((x1 << R[d % 8][j]) | (x1 >> (64 - R[d % 8][j]))) ^ *y0;
}

static void threefish1024_E(char *E,
			    const char *K,
			    const char *T,
			    const char *P)
{
  /*
  ** Section 3.3.
  */

  if(!E || !K || !P || !T)
    return;

  uint64_t C240 = 0x1bd11bdaa9fc1a22;
  uint64_t c[Nw];
  uint64_t k[Nw + 1];
  uint64_t kNw = C240; // Section 3.3.2.
  uint64_t p[Nw];
  uint64_t s[Nr / 4 + 1][Nw];
  uint64_t t[3];
  uint64_t v[Nw];

  bytesToWords(k, K, (size_t) 128);
  bytesToWords(p, P, (size_t) 128);
  bytesToWords(t, T, (size_t) 2);

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

  for(size_t d = 0; d < Nr; d++)
    {
      uint64_t e[Nw];

      if(d % 4 == 0)
	for(size_t i = 0; i < Nw; i++)
	  e[i] = v[i] + s[d / 4][i];
      else
	for(size_t i = 0; i < Nw; i++)
	  e[i] = v[i];

      uint64_t f[Nw];

      for(size_t j = 0; j < Nw / 2; j++)
	{
	  uint64_t x0 = e[j * 2];
	  uint64_t x1 = e[j * 2 + 1];
	  uint64_t y0 = 0;
	  uint64_t y1 = 0;

	  mix(x0, x1, d, j, &y0, &y1);
	  f[j * 2] = y0;
	  f[j * 2 + 1] = y1;
	}

      for(size_t i = 0; i < Nw; i++)
	v[i] = f[Pi[i]];
    }

  for(size_t i = 0; i < Nw; i++)
    c[i] = v[i] + s[Nr / 4][i];

  wordsToBytes(E, c, Nw);
  memset(c, 0, sizeof(c));
  memset(k, 0, sizeof(k));
  memset(p, 0, sizeof(p));
  memset(s, 0, sizeof(s));
  memset(t, 0, sizeof(t));
  memset(v, 0, sizeof(v));
}

static void wordsToBytes(char *B,
			 const uint64_t *words,
			 const size_t words_size)
{
  if(!B || !words || words_size <= 0)
    return;

  for(size_t i = 0; i < words_size; i++)
    {
      uint64_t w = words[i];

      for(size_t j = 0; j < 8; j++)
	{
	  B[i * 8 + j] = (char) (0xff & w);
	  w >>= 8;
	}
    }
}

void libskein_threefish1024(char *E,
			    const char *K,
			    const char *T,
			    const char *P)
{
  threefish1024_E(E, K, T, P);
}
