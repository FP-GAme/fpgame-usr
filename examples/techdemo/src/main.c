/* Tests the FP-GAme User Library by letting the user scroll around in "The Mall" at a pixelized 
 *   CMU. Play as CMU mascot Scotty in the most revolutionary game of 2021.
 * Author: Joseph Yankel
 */

#include <fp-game/apu.h>
#include <fp-game/ppu.h>
#include <fp-game/con.h>

#include <stdlib.h>
#include <stdio.h>

// scotty's animation delay (slightly slower than 8 fps)
#define SCOTTY_ANIM_DELAY 7

// world tiles animation delay (around 2 fps)
#define WORLD_ANIM_DELAY 30

// Palette IDs
#define THE_MALL_PALETTE_ID 0
#define SCOTTY_PALLETE_ID 0

// Sprite IDs
#define SCOTTY_SPRITE_ID 0

// Max scroll values for tile layer
#define MAX_TILE_SCROLL_X (512 - 320)
#define MAX_TILE_SCROLL_Y (512 - 240)
#define MAX_SCOTTY_X (320 - 16)
#define MAX_SCOTTY_Y (240 - 16)
#define SCOTTY_CENTER_X ((320 - 16)>>1)
#define SCOTTY_CENTER_Y ((240 - 16)>>1)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef enum { SCOTTY_FRONT=0, SCOTTY_BACK=1, SCOTTY_RSIDE=2, SCOTTY_LSIDE=3 } scotty_state_e;

static unsigned bark_btn_pressed = 0;
static unsigned start_new_bark = 0;

// the filenames for all mall patterns in order of their position in Pattern RAM
const char *the_mall_pattern_fns[] = {
    "assets/1-grass-0.pattern",
    "assets/2-grass_angled-0.pattern",
    "assets/3-grass_horizontal-0.pattern",
    "assets/4-grass_corner-0.pattern",
    "assets/5-grass_vertical-0.pattern",
    "assets/6-gravel.pattern",
    "assets/7-pavement.pattern",
    "assets/8-pink_bush_base-0.pattern",
    "assets/9-pink_bush_branch-0.pattern"
};

// the filenames for all dynamic patterns
const char *the_mall_anim_pattern_fns[] = {
    "assets/1-grass-0.pattern",
    "assets/1-grass-1.pattern",
    "assets/2-grass_angled-0.pattern",
    "assets/2-grass_angled-1.pattern",
    "assets/3-grass_horizontal-0.pattern",
    "assets/3-grass_horizontal-1.pattern",
    "assets/4-grass_corner-0.pattern",
    "assets/4-grass_corner-1.pattern",
    "assets/5-grass_vertical-0.pattern",
    "assets/5-grass_vertical-1.pattern",
    "assets/8-pink_bush_base-0.pattern",
    "assets/8-pink_bush_base-1.pattern",
    "assets/9-pink_bush_branch-0.pattern",
    "assets/9-pink_bush_branch-1.pattern"
};

// the filenames for all scotty sprite frames in order of their position in Pattern RAM
const char *scotty_pattern_fns[] = {
    "assets/scotty_front-0.pattern",
    "assets/scotty_front-1.pattern",
    "assets/scotty_front-2.pattern",
    "assets/scotty_front-3.pattern",
    "assets/scotty_back-0.pattern",
    "assets/scotty_back-1.pattern",
    "assets/scotty_back-2.pattern",
    "assets/scotty_back-3.pattern",
    "assets/scotty_side-0.pattern",
    "assets/scotty_side-1.pattern",
    "assets/scotty_side-2.pattern",
    "assets/scotty_side-3.pattern"
};

// load and write static tiles, static palettes, and initial patterns for both sprite and world.
int load_the_mall()
{
    // === load background tilemap ===
    tile_t *the_mall_tiles;
    if ((the_mall_tiles = malloc(64 * 64 * sizeof(tile_t))) == NULL)
    {
        printf("Malloc Tiles Failed!\n");
        return -1;
    }
    ppu_load_tilemap(the_mall_tiles, 64*64, "assets/the_mall.tilemap");
    ppu_write_vram(the_mall_tiles, 64*64*sizeof(tile_t), 0);
    free(the_mall_tiles); // unload tiles to save resources

    // === load the foreground pink bush ===
    tile_t pink_bush_base_tile = ppu_make_tile(
        ppu_pattern_addr(8,0),
        THE_MALL_PALETTE_ID,
        MIRROR_NONE
    );
    tile_t pink_bush_branch_tile = ppu_make_tile(
        ppu_pattern_addr(9,0),
        THE_MALL_PALETTE_ID,
        MIRROR_NONE
    );
    // the pink bush contains 4 rows of 5 branch tiles each, one of which will be overwritten with
    //   the base tile
    for (unsigned i = 0; i < 4; i++)
    {
        ppu_write_tiles_horizontal(&pink_bush_branch_tile, 1, LAYER_FG, 48, 26+i, 5);
    }
    ppu_write_tiles_horizontal(&pink_bush_base_tile, 1, LAYER_FG, 50, 29, 1);

    // === load palettes ===
    palette_t *palettes;
    if ((palettes = malloc(2*sizeof(palette_t))) == NULL)
    {
        printf("Malloc Palettes Failed!\n");
        return -1;
    }
    ppu_load_palette(&palettes[0], "assets/the_mall.palette");
    ppu_load_palette(&palettes[1], "assets/scotty.palette");
    ppu_write_palette(&palettes[0], LAYER_BG, THE_MALL_PALETTE_ID);
    ppu_write_palette(&palettes[0], LAYER_FG, THE_MALL_PALETTE_ID);
    ppu_write_palette(&palettes[1], LAYER_SPR, SCOTTY_PALLETE_ID);
    free(palettes); // unload palette to save resources (minimal)

    // === load static world patterns ===
    pattern_t *the_mall_pattern;
    // First, load all world tiles in one 9x1 chunk located at (1,0) (first tile is transparent)
    if ((the_mall_pattern = malloc(1*1*sizeof(pattern_t))) == NULL)
    {
        printf("Malloc Patterns Failed!\n");
        return -1;
    }
    for (unsigned i = 0; i < 9; i++)
    {
        printf("Loading pattern: %s\n", the_mall_pattern_fns[i]);
        ppu_load_pattern(the_mall_pattern, the_mall_pattern_fns[i], 1, 1);
        ppu_write_pattern(the_mall_pattern, 1, 1, ppu_pattern_addr(i+1,0));
    }
    free(the_mall_pattern);

    // === load scotty ===
    // load scotty frames (4 front, 4 back, 4 side) sequentially starting at (0,1).
    pattern_t *scotty_pattern;
    if ((scotty_pattern = malloc(2*2*sizeof(pattern_t))) == NULL)
    {
        printf("Malloc Cloud Pattern Failed!\n");
        return -1;
    }
    for (unsigned i = 0; i < 12; i++)
    {
        printf("Loading pattern: %s\n", scotty_pattern_fns[i]);
        ppu_load_pattern(scotty_pattern, scotty_pattern_fns[i], 2, 2);
        ppu_write_pattern(scotty_pattern, 2, 2, ppu_pattern_addr(2*i, 1));
    }
    free(scotty_pattern);

    return 0;
}

// update scrolling values for world and scotty based on input
void update_scrolling(int input, unsigned *world_scroll_x, unsigned *world_scroll_y,
                      unsigned *scotty_x, unsigned *scotty_y)
{
    // Keep increasing/decreasing world_scroll in a direction until it hits its max/min value, then
    //   move scotty

    // TODO: I actually want to keep scotty centered...

    // If scotty not centered on an axis, move scotty first.
    // else, move the world with scotty fixed at the center.

    if (CON_IS_PRESSED(input, CON_BUT_DOWN))
    {
        if (*world_scroll_y < MAX_TILE_SCROLL_Y && *scotty_y == SCOTTY_CENTER_Y)
        {
            *world_scroll_y += 1;
        }
        else
        {
            *scotty_y = (*scotty_y < MAX_SCOTTY_Y) ? *scotty_y + 1 : *scotty_y;
        }
    }
    else if (CON_IS_PRESSED(input, CON_BUT_UP))
    {
        if (*world_scroll_y > 0 && *scotty_y == SCOTTY_CENTER_Y)
        {
            *world_scroll_y -= 1;
        }
        else
        {
            *scotty_y = (*scotty_y > 0) ? *scotty_y - 1 : *scotty_y;
        }
    }

    if (CON_IS_PRESSED(input, CON_BUT_RIGHT))
    {
        if (*world_scroll_x < MAX_TILE_SCROLL_X && *scotty_x == SCOTTY_CENTER_X)
        {
            *world_scroll_x += 1;
        }
        else
        {
            *scotty_x = (*scotty_x < MAX_SCOTTY_X) ? *scotty_x + 1 : *scotty_x;
        }
    }
    else if (CON_IS_PRESSED(input, CON_BUT_LEFT))
    {
        if (*world_scroll_x > 0 && *scotty_x == SCOTTY_CENTER_X)
        {
            *world_scroll_x -= 1;
        }
        else
        {
            *scotty_x = (*scotty_x > 0) ? *scotty_x - 1 : *scotty_x;
        }
    }
}

// calculates the pattern address for scotty's current animation frame
pattern_addr_t calc_scotty_pattern(unsigned scotty_frame, scotty_state_e scotty_state)
{
    // the last two states should display the same pattern (scotty_side), just mirrored
    unsigned scotty_state_offset = (scotty_state == SCOTTY_LSIDE) ? 2 : scotty_state;

    return ppu_pattern_addr(2*4*(scotty_state_offset) + 2*scotty_frame, 1);
}

// updates scotty's sprite data
void scotty_update(sprite_t *scotty_sprite, unsigned scotty_frame, scotty_state_e scotty_state,
                   unsigned x, unsigned y)
{
    scotty_sprite->pattern_addr = calc_scotty_pattern(scotty_frame, scotty_state);
    scotty_sprite->mirror = (scotty_state == SCOTTY_LSIDE) ? MIRROR_X : MIRROR_NONE;
    scotty_sprite->x = x;
    scotty_sprite->y = y;
}

// animate scotty by changing state and frame id based on a timer/delay and input direction
// Does not write or block like animate_world does
void animate_scotty(int input, unsigned *scotty_frame, scotty_state_e *scotty_state)
{
    static unsigned scotty_anim_timer = SCOTTY_ANIM_DELAY;
    unsigned button_pressed = 0;
    
    if (CON_IS_PRESSED(input, CON_BUT_DOWN))
    {
        *scotty_state = SCOTTY_FRONT;
        button_pressed = 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_UP))
    {
        *scotty_state = SCOTTY_BACK;
        button_pressed = 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_RIGHT))
    {
        *scotty_state = SCOTTY_RSIDE;
        button_pressed = 1;
    }
    else if (CON_IS_PRESSED(input, CON_BUT_LEFT))
    {
        *scotty_state = SCOTTY_LSIDE;
        button_pressed = 1;
    }
    // If no direction is pressed, keep last state

    if (scotty_anim_timer == 0 && button_pressed == 1)
    {
        // increment frame with wrap-around
        *scotty_frame = (*scotty_frame == 3) ? 0 : *scotty_frame + 1;
        // reset animation timer
        scotty_anim_timer = SCOTTY_ANIM_DELAY;
    }
    else if (button_pressed == 1)
    {
        scotty_anim_timer--;
    }
    else
    {
        // Revert scotty to the default (standing still) frame when no buttons are pressed
        *scotty_frame = 0;
    }
}

// animate world by changing the patterns of specific world tiles (BLOCKS UNTIL WRITE)
void animate_world(pattern_t *anim_patterns)
{
    static unsigned world_anim_timer = WORLD_ANIM_DELAY;
    static unsigned world_anim_frame = 0;

    // play world animation
    if (world_anim_timer == 0)
    {
        world_anim_frame = !world_anim_frame; // toggle which animation frame we are using
        world_anim_timer = WORLD_ANIM_DELAY;  // reset timer

        // write all animated tiles:
        // start by writing dynamic patterns 1-5
        for (unsigned i = 0; i < 5; i++)
        {
            while (ppu_write_pattern(&anim_patterns[2*i + world_anim_frame], 1, 1, ppu_pattern_addr(i+1,0)) != 0);
        }
        // end with writing dynamic patterns 8 and 9
        while (ppu_write_pattern(&anim_patterns[10+world_anim_frame], 1, 1, ppu_pattern_addr(8,0)) != 0);
        while (ppu_write_pattern(&anim_patterns[12+world_anim_frame], 1, 1, ppu_pattern_addr(9,0)) != 0);
    }
    else
    {
        world_anim_timer--;
    }
}

void apu_callback(const int8_t **buf, int *len)
{
    static size_t bark_loc = 0; // location within the scotty_bark raw audio samples

    extern const int8_t _binary_bins_scottybark_bin_start[];
    extern const int8_t _binary_bins_scottybark_bin_end[];
    size_t bark_bin_size = (size_t) (((uintptr_t) _binary_bins_scottybark_bin_end)
                           - ((uintptr_t) _binary_bins_scottybark_bin_start));

    // Handle any queued bark (doesn't matter if bark_done or not):
    if (start_new_bark)
    {
        start_new_bark = 0;

        // reset bark waveform
        bark_loc = 0;
    }

    if (bark_loc >= bark_bin_size)
    {
        // The waveform is finished playing. This is the done state!

        // This will set the buffer to 0s, causing silence.
        *len = 0;
    }
    else
    {
        *buf = &_binary_bins_scottybark_bin_start[bark_loc];
        *len = MIN(bark_bin_size - bark_loc, APU_BUF_MAX);
        bark_loc += APU_BUF_MAX;
    }
}

int main(void)
{
    ppu_enable();
    apu_enable(apu_callback);

    if (load_the_mall() == -1) return -1;

    // Animated world tiles. These will get written to Pattern RAM to play an animation without
    //   needing to write tons of tiles in Tile RAM.
    pattern_t *anim_patterns;
    if ((anim_patterns = malloc(14 * sizeof(pattern_t))) == NULL)
    {
        printf("Malloc Tiles Failed!\n");
        return -1;
    }
    for (unsigned i = 0; i < 14; i++)
    {
        printf("Loading pattern: %s\n", the_mall_anim_pattern_fns[i]);
        ppu_load_pattern(&anim_patterns[i], the_mall_anim_pattern_fns[i], 1, 1);
    }
    // we will free these tiles after we exit from the game loop

    // Enable all tile layers
    ppu_set_layer_enable(LAYER_BG | LAYER_FG | LAYER_SPR);

    // Create sprite for game character
    unsigned scotty_frame = 0;
    scotty_state_e scotty_state = SCOTTY_FRONT;
    unsigned scotty_x = 0;
    unsigned scotty_y = 0;
    sprite_t scotty_sprite;
    scotty_sprite.palette_id = SCOTTY_PALLETE_ID;
    scotty_sprite.height = 2;
    scotty_sprite.width = 2;
    scotty_sprite.prio = PRIO_IN_MIDDLE;

    // world scroll
    unsigned world_scroll_x = 0;
    unsigned world_scroll_y = 0;

    // Game loop capped at 60Hz
    unsigned exit_button_pressed = 0;
    int input;
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

        // make scotty bark if the B button is pressed
        if (bark_btn_pressed == 0 && CON_IS_PRESSED(input, CON_BUT_B))
        {
            bark_btn_pressed = 1;
            start_new_bark = 1;
        }
        else if (!CON_IS_PRESSED(input, CON_BUT_B))
        {
            bark_btn_pressed = 0;
        }

        // update tile layer scrolls and scotty's position based on input
        update_scrolling(input, &world_scroll_x, &world_scroll_y, &scotty_x, &scotty_y);

        // Calculate scotty animation changes depending on the direction pressed
        animate_scotty(input, &scotty_frame, &scotty_state);

        // perform local sprite updates
        scotty_update(&scotty_sprite, scotty_frame, scotty_state, scotty_x, scotty_y);
        
        // buffer changes to VRAM
        animate_world(anim_patterns); // does not return until VRAM is written
        while (ppu_set_scroll(LAYER_BG, world_scroll_x, world_scroll_y) != 0);
        while (ppu_set_scroll(LAYER_FG, world_scroll_x, world_scroll_y) != 0);
        while (ppu_write_sprites(&scotty_sprite, 1, SCOTTY_SPRITE_ID) != 0);
        // ==================

        // render VRAM changes
        while(ppu_update() != 0);
    }

    // cleanup and exit
    free(anim_patterns);
    ppu_disable();
    apu_disable();
    return 0;
}