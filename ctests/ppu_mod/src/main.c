/*
 * Test the PPU Library and Kernel Module by writing a single tile to the screen and changing the
 *   default background color.
 *
 * Author: Joseph Yankel
 */

#include <fp-game/ppu.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    ppu_enable();

    ppu_set_bgcolor(0xFF0000);
    while (ppu_update() != 0);

    ppu_disable();
    return 0;
}