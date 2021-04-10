/*
 * Test the apu module by playing a song.
 *
 * Author: Andrew Spaulding
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <fp-game/apu.h>
#include <fp-game/drv_apu.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int main(void)
{
	extern const int8_t _binary_bins_champ_bin_start[];
	extern const int8_t _binary_bins_champ_bin_end[];
	size_t size = (size_t) (((uintptr_t) _binary_bins_champ_bin_end)
	            - ((uintptr_t) _binary_bins_champ_bin_start));

	int fd = open(APU_DEV_FILE, O_WRONLY);
	size_t loc = 0;

	while (1) {
		if (write(fd, &_binary_bins_champ_bin_start[loc], MIN(size - loc, APU_BUF_MAX)) == 0) {
			loc += APU_BUF_MAX;
			if (loc >= size) { loc = 0; }
		}
	}

	return 0;
}
