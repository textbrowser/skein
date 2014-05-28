#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libskein_threefish.h"

static void test1(void)
{
  char E[32];
  char K[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char P[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char T[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  memset(E, 0, sizeof(E));
  libskein_threefish(E, K, T, P, 256);
  printf("Test #1, 256-bit Threefish: ");

  for(size_t i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf(".\n");
}

static void test2(void)
{
  char E[64];
  char K[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char P[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char T[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  memset(E, 0, sizeof(E));
  libskein_threefish(E, K, T, P, 512);
  printf("Test #2, 512-bit Threefish: ");

  for(size_t i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf(".\n");
}

int main(void)
{
  test1();
  test2();
  return EXIT_SUCCESS;
}
