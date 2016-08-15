extern "C"
{
#include <stdio.h>
}

#include "libskein_skein.h"

static void test1(void)
{
  char D[32];
  char E[32];
  char K[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char P[32] = {'H', 'e', 'l', 'l', 'o', '.', 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char T[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  size_t i = 0;

  memset(E, 0, sizeof(E));
  libskein_threefish_encrypt(E, K, T, P, sizeof(P), 256);
  printf("Test #1, 256-bit Threefish Encryption: ");

  for(i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf(".\n");
  printf("Test #1, 256-bit Threefish Decryption: ");
  memset(D, 0, sizeof(D));
  libskein_threefish_decrypt(D, K, T, E, sizeof(E), 256);

  for(i = 0; i < sizeof(D) / sizeof(D[0]); i++)
    printf("%c", D[i]);

  printf("\n");
}

static void test2(void)
{
  char D[64];
  char E[64];
  char K[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char P[64] = {'!', 'B', 'y', 'e', '!', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char T[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  size_t i = 0;

  memset(E, 0, sizeof(E));
  libskein_threefish_encrypt(E, K, T, P, sizeof(P), 512);
  printf("Test #2, 512-bit Threefish Encryption: ");

  for(i = 0; i < sizeof(E) / sizeof(E[0]); i++)
    printf("%02x", E[i] & 0xff);

  printf(".\n");
  printf("Test #2, 512-bit Threefish Decryption: ");
  memset(D, 0, sizeof(D));
  libskein_threefish_decrypt(D, K, T, E, sizeof(E), 512);

  for(i = 0; i < sizeof(D) / sizeof(D[0]); i++)
    printf("%c", D[i]);

  printf("\n");
}

int main(void)
{
  test1();
  test2();
  return EXIT_SUCCESS;
}
