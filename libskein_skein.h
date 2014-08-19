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

#ifndef LIBSKEIN_SKEIN_H
#define LIBSKEIN_SKEIN_H

extern "C"
{
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
}

#include <algorithm>
#include <new>

#ifdef __cplusplus
extern "C"
{
#endif

void libskein_simplehash(char *H,
			 const size_t Nb,
			 const size_t No,
			 const char *M,
			 const size_t M_size,
			 const size_t block_size);
void libskein_threefish(char *E,
			const char *K,
			const char *T,
			const char *P,
			const size_t block_size);

class libskein_tweak
{
  /*
  **  Please refer to section 3.4 of the official document.
  */

 public:
  libskein_tweak(const short type)
  {
    m_t[0] = 0ULL; // Position.
    m_t[1] = (static_cast<uint64_t> (type)) << 56; // 56 = 120 - 64.
    m_type = type;
  }

  bool isFirst(void) const
  {
    return m_t[1] & (1ULL << 62); // 62 = 126 - 64.
  }

  bool isLast(void) const
  {
    return m_t[1] & (1ULL << 63); // 63 = 127 - 64.
  }

  bool isPadded(void) const
  {
    return m_t[1] & (1ULL << 55); // 55 = 119 - 64.
  }

  const uint64_t *value(void) const
  {
    return m_t;
  }

  void setFirst(const bool first)
  {
    if(first)
      m_t[1] |= (1ULL << 62); // 62 = 126 - 64.
    else
      m_t[1] &= ~(1ULL < 62);
  }

  void setLast(const bool last)
  {
    if(last)
      m_t[1] |= (1ULL << 63); // 63 = 127 - 64.
    else
      m_t[1] &= ~(1ULL < 63);
  }

  void setPadded(const bool padded)
  {
    if(padded)
      m_t[1] |= (1ULL << 55); // 55 = 119 - 64.
    else
      m_t[1] &= ~(1ULL << 55);
  }

  void setPosition(const uint64_t position)
  {
    m_t[0] = position;
  }

 private:
  short m_type;
  uint64_t m_t[2];
};

#ifdef __cplusplus
}
#endif
#endif
