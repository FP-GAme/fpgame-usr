/*
 * Test the PPU Library and Kernel Module by writing a single tile to the screen and changing the
 *   default background color.
 *
 * Author: Joseph Yankel
 */

#include <fp-game/ppu.h>

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

int main(void)
{
    uint16_t tile_data_array[4] = {
        0x40,
        0x41,
        0x42,
        0x43,
    }; // Holds 4 flipped versions of a single tile

    uint32_t palette_data[16] = {
        0x000000, 
        0xFF0000,
        0xff5e00,
        0xffbb00,
        0xe5ff00,
        0x88ff00,
        0x2bff00,
        0x00ff33,
        0x00ff91,
        0x00ffee,
        0x00b3ff,
        0x0055ff,
        0x0800ff,
        0x6600ff,
        0xc300ff,
        0xff00dd,
    }; // Holds 1 palette's worth of palette data

    uint8_t pattern_data[32]  = {
        0x11, 0x22, 0x33, 0x44,
        0x0F, 0x00, 0x00, 0x50,
        0x0F, 0x00, 0x00, 0x60,
        0x0E, 0x00, 0x00, 0x60,
        0x0E, 0x00, 0x00, 0x70,
        0x0D, 0x00, 0x00, 0x80,
        0x0D, 0x00, 0x00, 0x80,
        0xCC, 0xBB, 0xAA, 0x99,
    }; // Holds 1 tile's worth of pattern data

    ppu_enable();

    ppu_set_bgcolor(0xA0A0A0);
    ppu_set_layer_enable(0x1);
    
    ppu_write_vram(&pattern_data, 32, 16384 + 32);
    ppu_write_vram(&palette_data, 64, 49152);
    while (ppu_update() != 0);

    unsigned tile_row;
    unsigned tile_col;
    unsigned tile_id = 0;
    for (tile_row = 1; tile_row < 62; tile_row += 2)
    {
        for (tile_col = 1; tile_col < 62; tile_col += 2)
        {
            sleep(1);
            ppu_write_vram(&tile_data_array[tile_id], 2, (tile_row*64 + tile_col) * 2);
            while (ppu_update() != 0);

            if ( (tile_id += 1) == 4 )
                tile_id = 0;
        }
    }

    ppu_disable();
    return 0;
}