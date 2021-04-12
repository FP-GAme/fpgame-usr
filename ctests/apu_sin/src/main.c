/*
 * Test the apu module by playing a song.
 *
 * Author: Andrew Spaulding
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fp-game/apu.h>

static const unsigned long long sin_buf[] = {
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
	0x7d756a5a47311900,
	0x1931475a6a757d7f,
	0x838b96a6b9cfe700,
	0xe7cfb9a6968b8381,
};

/* Helper functions */
void sin_callback(const int8_t **buf, int *len);

int main(void)
{
	apu_enable(sin_callback);
	while (1);
	return 0;
}

void sin_callback(const int8_t **buf, int *len)
{
	*buf = (const int8_t *) sin_buf;
	*len = APU_BUF_MAX;
}
