#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libskein_threefish.h"

static void fromHex(const char *hex,
		    const size_t hex_size,
		    char *bytes)
{
  if(!bytes || !hex || hex_size <= 0)
    return;

  const char *ptr = hex;

  for(size_t i = 0; i < hex_size / 2; i++)
    {
      sscanf(ptr, "%2hhx", &bytes[i]);
      ptr += 2 * sizeof(char);
    }
}

int main(void)
{
  char E[32];
  char K[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char P[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char T[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  (void) fromHex(0, 0, 0);
  memset(E, 0, sizeof(E));
  libskein_threefish(E, K, T, P, 256);

  for(size_t i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf("\n");

  return EXIT_SUCCESS;
}
