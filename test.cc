extern "C"
{
#include <stdio.h>
}

#include "libskein_skein.h"

static void test1(void)
{
  char E[32];
  char K[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char P[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char T[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  size_t i = 0;

  memset(E, 0, sizeof(E));
  libskein_threefish(E, K, T, P, (size_t) 256);
  printf("Test #1, 256-bit Threefish: ");

  for(i = 0; i < sizeof(E) / sizeof(E[0]); i++)
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
  size_t i = 0;

  memset(E, 0, sizeof(E));
  libskein_threefish(E, K, T, P, (size_t) 512);
  printf("Test #2, 512-bit Threefish: ");

  for(i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf(".\n");
}

static void test3(void)
{
  char H[32];
  char M[1];
  size_t i = 0;

  memset(H, 0, sizeof(H));
  M[0] = (char) 0xff;
  libskein_simplehash(H, 32, 256, M, 1, 256);
  printf("Test #3, 256-bit Simple Skein: ");

  for(i = 0; i < sizeof(H) / sizeof(H[0]); i++)
    printf("%02x", H[i] & 0xff);

  printf(".\n");
}

int main(void)
{
  test1();
  test2();
  test3();
  return EXIT_SUCCESS;
}
