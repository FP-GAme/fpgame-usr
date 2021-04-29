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

// slightly slower than 8fps
#define ANIM_DELAY 7

int load_world(void)
{
    // === load tilemap ===
    tile_t *world_tiles;
    if ((world_tiles = malloc(64 * 64 * sizeof(tile_t))) == NULL)
    {
        printf("Malloc Tiles Failed!\n");
        return -1;
    }
    ppu_load_tilemap(world_tiles, 64*64, "the_mall_src/the_mall.tilemap");
    ppu_write_vram(world_tiles, 64*64*sizeof(tile_t), 0); // write tilemap to VRAM
    free(world_tiles); // unload tiles to save resources

    // === load palettes ===
    palette_t *palettes;
    if ((palettes = malloc(2*sizeof(palette_t))) == NULL)
    {
        printf("Malloc Palette Failed!\n");
        return -1;
    }
    ppu_load_palette(&palettes[0], "the_mall_src/world_palette.txt");
    ppu_load_palette(&palettes[1], "the_mall_src/scotty.palette");
    ppu_write_palette(&palettes[0], LAYER_BG, 0);
    ppu_write_palette(&palettes[0], LAYER_SPR, 0); // For debugging, also include tile palettes here
    ppu_write_palette(&palettes[1], LAYER_SPR, 1);
    free(palettes); // unload palette to save resources (minimal)

    // === load patterns ===
    pattern_t *world_pattern;
    // First, load all world tiles in one 7x1 chunk located at (1,0) (first tile is transparent)
    if ((world_pattern = malloc(1*1*sizeof(pattern_t))) == NULL)
    {
        printf("Malloc Patterns Failed!\n");
        return -1;
    }
    char filename[32+1]; // +1 for NUL
    for (unsigned i = 0; i < 7; i++)
    {
        sprintf(filename, "the_mall_src/world_pattern%d.txt", i);
        ppu_load_pattern(world_pattern, filename, 1, 1);
        ppu_write_pattern(world_pattern, 1, 1, ppu_pattern_addr(i+1,0));
    }
    free(world_pattern);

    // load scotty frames (4 total)
    pattern_t *scotty_pattern;
    if ((scotty_pattern = malloc(2*2*sizeof(pattern_t))) == NULL)
    {
        printf("Malloc Cloud Pattern Failed!\n");
        return -1;
    }
    char scotty_frame_fn[36+1]; // +1 for NUL
    for (unsigned i = 0; i < 4; i++)
    {
        sprintf(scotty_frame_fn, "the_mall_src/scotty_front-%d.pattern", i);
        ppu_load_pattern(scotty_pattern, scotty_frame_fn, 2, 2);
        // Load each scotty frame in 2x2 chunks starting at (0, 1) and going right (2,1), ..., (6,1)
        ppu_write_pattern(scotty_pattern, 2, 2, ppu_pattern_addr(2*i, 1));
    }
    free(scotty_pattern);

    return 0;
}

void scroll_world(int input, unsigned *scroll_x, unsigned *scroll_y)
{
    if (CON_IS_PRESSED(input, CON_BUT_DOWN))
    {   
        *scroll_y = (*scroll_y == 511) ? 0 : *scroll_y + 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_UP))
    {
        *scroll_y = (*scroll_y == 0) ? 511 : *scroll_y - 1;
    }

    if (CON_IS_PRESSED(input, CON_BUT_RIGHT))
    {
        *scroll_x = (*scroll_x == 511) ? 0 : *scroll_x + 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_LEFT))
    {
        *scroll_x = (*scroll_x == 0) ? 511 : *scroll_x - 1;
    }
}

void scroll_sprite(int input, unsigned *scroll_x, unsigned *scroll_y)
{
    if (CON_IS_PRESSED(input, CON_BUT_DOWN))
    {   
        *scroll_y = (*scroll_y == 239) ? 0 : *scroll_y + 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_UP))
    {
        *scroll_y = (*scroll_y == 0) ? 239 : *scroll_y - 1;
    }

    if (CON_IS_PRESSED(input, CON_BUT_RIGHT))
    {
        *scroll_x = (*scroll_x == 319) ? 0 : *scroll_x + 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_LEFT))
    {
        *scroll_x = (*scroll_x == 0) ? 319 : *scroll_x - 1;
    }
}

int main(void)
{
    ppu_enable();

    if (load_world() == -1) return -1;

    // Enable all tile layers
    ppu_set_layer_enable(LAYER_BG | LAYER_FG | LAYER_SPR);

    // Create sprite for game character
    unsigned scotty_frame = 0;
    sprite_t scotty_sprite;
    scotty_sprite.pattern_addr = ppu_pattern_addr(scotty_frame*2, 1);
    scotty_sprite.palette_id = 1;
    scotty_sprite.mirror = MIRROR_NONE;
    scotty_sprite.prio = PRIO_IN_FRONT;
    scotty_sprite.x = 0;
    scotty_sprite.y = 0;
    scotty_sprite.height = 2;
    scotty_sprite.width = 2;

    // Game loop locked to 60Hz
    unsigned exit_button_pressed = 0;
    int input;
    unsigned scroll_x = 0;
    unsigned scroll_y = 0;
    unsigned scotty_scroll_x = 0;
    unsigned scotty_scroll_y = 0;
    unsigned anim_update = ANIM_DELAY;
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
        if (CON_IS_PRESSED(input, CON_BUT_A))
        {
            // only move tile background if A button is also pressed
            scroll_world(input, &scroll_x, &scroll_y);
        }
        else
        {
            // otherwise, move scotty
            scroll_sprite(input, &scotty_scroll_x, &scotty_scroll_y);
        }
        while (ppu_set_scroll(LAYER_BG, scroll_x, scroll_y) != 0);


        if (CON_IS_PRESSED(input, CON_BUT_B))
        {
            // Animate scotty if b button is being pressed
            if (anim_update == 0)
            {
                scotty_frame = (scotty_frame == 3) ? 0 : scotty_frame + 1;
                anim_update = ANIM_DELAY;
            }
            else
            {
                anim_update--;
            }
        }

        // update sprites
        scotty_sprite.pattern_addr = ppu_pattern_addr(scotty_frame*2, 1);
        scotty_sprite.x = scotty_scroll_x;
        scotty_sprite.y = scotty_scroll_y;
        while (ppu_write_sprites(&scotty_sprite, 1, 0) != 0);
        
        // ==================

        // render changes
        while(ppu_update() != 0);
    }

    // cleanup and exit
    ppu_disable();
    return 0;
}