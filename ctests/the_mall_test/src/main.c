/* Tests the FP-GAme User Library by letting the user scroll around in "The Mall" at a pixelized 
 *   CMU. Play as CMU mascot Scotty and listen to bagpipe music in the most revolutionary game of
 *   2021.
 * Author: Joseph Yankel
 */

#include <fp-game/apu.h>
#include <fp-game/ppu.h>
#include <fp-game/con.h>

#include <stdlib.h>
#include <stdio.h>

// Delay in periods of 1/60 s in between increments of scrolling while holding a scrolling input
#define BG_SCROLL_DELAY 0

int load_world(void)
{
    palette_t *world_palette;
    tile_t *world_tiles;
    pattern_t *world_patterns;

    // load tilemap
    if ((world_tiles = malloc(64 * 64 * sizeof(tile_t))) == NULL)
    {
        printf("Malloc Tiles Failed!\n");
        return -1;
    }
    ppu_load_tilemap(world_tiles, 64*64, "the_mall_src/world_tilemap.txt");

    ppu_write_vram(world_tiles, 64*64*sizeof(tile_t), 0); // write tilemap to VRAM

    free(world_tiles); // unload tiles to save resources

    // load palettes
    if ((world_palette = malloc(sizeof(palette_t))) == NULL)
    {
        printf("Malloc Palette Failed!\n");
        return -1;
    }
    ppu_load_palette(world_palette, "the_mall_src/world_palette.txt");
    
    ppu_write_palette(world_palette, LAYER_BG, 0); // write palette to VRAM

    free(world_palette); // unload palette to save resources (minimal)

    // load patterns
    if ((world_patterns = malloc(sizeof(pattern_t) * 8)) == NULL)
    {
        printf("Malloc Patterns Failed!\n");
        return -1;
    }

    char filename[32+1]; // +1 for NUL
    for (unsigned i = 0; i < 7; i++)
    {
        sprintf(filename, "the_mall_src/world_pattern%d.txt", i);
        ppu_load_pattern(&world_patterns[i], filename);
    }
    // fill the last pattern with empty (transparent)
    for (unsigned i = 0; i < 8; i++)
    {
        world_patterns[7].pxrow[i] = 0;
    }

    ppu_write_pattern(world_patterns, 4, 2, 0);

    free(world_patterns);

    // Cloud pattern to test foreground layer and ppu_write_tiles functions
    pattern_addr_t cloud_pat_addr = ppu_pattern_addr(1, 0, 0);
    pattern_t *cloud_pattern;
    if ((cloud_pattern = malloc(sizeof(pattern_t))) == NULL)
    {
        printf("Malloc Cloud Pattern Failed!\n");
        return -1;
    }
    ppu_load_pattern(cloud_pattern, "the_mall_src/cloud_pattern.txt");
    ppu_write_pattern(cloud_pattern, 1, 1, cloud_pat_addr);

    tile_t cloud_tiles = ppu_make_tile(cloud_pat_addr, 0, MIRROR_NONE);
    //ppu_write_tiles_horizontal(&cloud_tiles, 1, LAYER_FG, 63, 20, 67);
    //ppu_write_tiles_vertical(&cloud_tiles, 1, LAYER_FG, 20, 28, 40);

    return 0;
}

void handle_scroll_input(int input, unsigned *scroll_x, unsigned *scroll_y)
{
    // For maximum responsiveness, we always want these timers to start at 0 whenever a direction is
    //   not held. As soon as a direction is held, the scroll is updated, and the timer is started.
    static unsigned v_input_held = 0;
    static unsigned h_input_held = 0;

    if (CON_IS_PRESSED(input, CON_BUT_DOWN))
    {   
        if (v_input_held == 0)
        {
            *scroll_y = (*scroll_y == 511) ? 0 : *scroll_y + 1;
            v_input_held = BG_SCROLL_DELAY;
        }
        else
        {
            v_input_held--;
        }
    }
    else if (CON_IS_PRESSED(input, CON_BUT_UP))
    {
        if (v_input_held == 0)
        {
            *scroll_y = (*scroll_y == 0) ? 511 : *scroll_y - 1;
            v_input_held = BG_SCROLL_DELAY;
        }
        else
        {
            v_input_held--;
        }
    }
    else
    {
        // nothing is being held, so reset the delay timer
        v_input_held = 0;
    }

    if (CON_IS_PRESSED(input, CON_BUT_RIGHT))
    {
        if (h_input_held == 0)
        {
            *scroll_x = (*scroll_x == 511) ? 0 : *scroll_x + 1;
            h_input_held = BG_SCROLL_DELAY;
        }
        else
        {
            h_input_held--;
        }
    }
    else if (CON_IS_PRESSED(input, CON_BUT_LEFT))
    {
        if (h_input_held == 0)
        {
            *scroll_x = (*scroll_x == 0) ? 511 : *scroll_x - 1;
            h_input_held = BG_SCROLL_DELAY;
        }
        else
        {
            h_input_held--;
        }
    }
    else
    {
        // nothing is being held, so reset the delay timer
        h_input_held = 0;
    }
}

int main(void)
{
    ppu_enable();

    if (load_world() == -1) return -1;

    // Enable background tile layer
    ppu_set_layer_enable(LAYER_BG | LAYER_FG);

    // Game loop locked to 60Hz
    unsigned exit_button_pressed = 0;
    int input;
    unsigned scroll_x = 0;
    unsigned scroll_y = 0;
    while (!exit_button_pressed)
    {
        // update input
        if ((input = get_con_state()) < 0)
        {
            printf("Input Update Failed!\n");
            return -1;
        }

        // === main logic ===
        // check if the exit button is pressed
        exit_button_pressed = CON_IS_PRESSED(input, CON_BUT_START);

        // update background layer scroll based on input
        handle_scroll_input(input, &scroll_x, &scroll_y);
        while (ppu_set_scroll(LAYER_BG, scroll_x, scroll_y) != 0);
        
        // ==================

        // render changes
        while(ppu_update() != 0);
    }

    // cleanup and exit
    ppu_disable();
    return 0;
}