#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libskein_threefish.h"

static void fromHex(const char *hex,
		    const size_t hex_size,
		    char *bytes)
{
  const char *ptr = hex;

  for(size_t i = 0; i < hex_size / 2; i++)
    {
      sscanf(ptr, "%2hhx", &bytes[i]);
      ptr += 2 * sizeof(char);
    }
}

int main(void)
{
  char E[128];
  char *K_ = "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000";
  char K[128];
  char *P_ = "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000";
  char P[128];
  char *T_ = "00000000000000000000000000000000";
  char T[16];

  fromHex(K_, (size_t) 256, K);
  fromHex(P_, (size_t) 256, P);
  fromHex(T_, (size_t) 32, T);
  memset(E, 0, sizeof(E));
  libskein_threefish1024(E, K, T, P);

  for(size_t i = 0; i < 128; i++)
    printf("%02x", E[i]);

  printf("\n");
  return EXIT_SUCCESS;
}
