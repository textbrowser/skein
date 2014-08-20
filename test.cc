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
  libskein_threefish(E, K, T, P, 32, 256);
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
  libskein_threefish(E, K, T, P, 64, 512);
  printf("Test #2, 512-bit Threefish: ");

  for(i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf(".\n");
}

static void test3(void)
{
  char H[64];
  char M[] = {0x54, 0x65, 0x73, 0x74, 0x2e}; // Test.
  size_t i = 0;

  memset(H, 0, sizeof(H));
  libskein_simplehash(H, sizeof(H), 512, M, 5, 512);
  printf("Test #3, 512-512-bit Simple Skein: ");

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
