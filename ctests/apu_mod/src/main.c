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

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static size_t loc = 0;
static size_t count = 0;

/* Helper functions */
void apu_callback(const int8_t **buf, int *len);

int main(void)
{
	apu_enable(apu_callback);

	while (1) {
		if (count < 60) { continue; }
		printf("TICK!\n");
		count = 0;
	}

	return 0;
}

void apu_callback(const int8_t **buf, int *len)
{
	extern const int8_t _binary_bins_clean_bin_start[];
	extern const int8_t _binary_bins_clean_bin_end[];
	size_t size = (size_t) (((uintptr_t) _binary_bins_clean_bin_end)
	            - ((uintptr_t) _binary_bins_clean_bin_start));
	count++;

	*buf = &_binary_bins_clean_bin_start[loc];
	*len = MIN(size - loc, APU_BUF_MAX);
	loc += APU_BUF_MAX;
	if (loc >= size) { loc = 0; }
}
