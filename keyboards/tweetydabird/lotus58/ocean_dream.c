/*
 * Copyright 2021 Tyler Thrailkill (@snowe/@snowe2010) <tyler.b.thrailkill@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ocean_dream.h"

// Calculated Parameters
#define TWINKLE_PROBABILITY_MODULATOR 100 / TWINKLE_PROBABILITY                            // CALCULATED: Don't Touch
#define TOTAL_STARS STARS_PER_LINE *NUMBER_OF_STAR_LINES                                   // CALCULATED: Don't Touch
#define OCEAN_ANIMATION_MODULATOR NUMBER_OF_FRAMES / OCEAN_ANIMATION_SPEED                 // CALCULATED: Don't Touch
#define SHOOTING_STAR_ANIMATION_MODULATOR NUMBER_OF_FRAMES / SHOOTING_STAR_ANIMATION_SPEED // CALCULATED: Don't Touch
#define STAR_ANIMATION_MODULATOR NUMBER_OF_FRAMES / STAR_ANIMATION_SPEED                   // CALCULATED: Don't Touch

uint8_t    animation_counter       = 0; // global animation counter.
bool       is_calm                 = false;
uint32_t   starry_night_anim_timer = 0;
uint32_t   starry_night_anim_sleep = 0;
static int current_wpm             = 0;

static uint8_t simple_rand(void) {
    static uint16_t random = 1;
    random *= UINT16_C(36563);
    return (uint8_t)(random >> 8);
}

static uint8_t increment_counter(uint8_t counter, uint8_t max) {
    counter++;
    if (counter >= max) {
        return 0;
    } else {
        return counter;
    }
}

#ifdef ENABLE_WAVE
static uint8_t decrement_counter(uint8_t counter, uint8_t max) {
    counter--;
    if (counter < 0 || counter > max) {
        return max;
    } else {
        return counter;
    }
}
#endif

#ifdef ENABLE_MOON // region
#    ifndef STATIC_MOON
uint8_t  moon_animation_frame   = 0; // keeps track of current moon frame
uint16_t moon_animation_counter = 0; // counts how many frames to wait before animating moon to next frame
#    endif

#    ifdef STATIC_MOON
static const char PROGMEM moon[6] = {
    0x18, 0x7E, 0xFF, 0xC3, 0x81, 0x81,
};
#    endif

#    ifndef STATIC_MOON
static const char PROGMEM moon_animation[14][8] = {
    // clang-format off
    { 0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, },
    { 0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x42, 0x00, },
    { 0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xC3, 0x00, 0x00, },
    { 0x3C, 0x7E, 0xFF, 0xFF, 0xC3, 0x81, 0x00, 0x00, },
    { 0x3C, 0x7E, 0xFF, 0xC3, 0x81, 0x00, 0x00, 0x00, },
    { 0x3C, 0x7E, 0xC3, 0x81, 0x81, 0x00, 0x00, 0x00, },
    { 0x3C, 0x42, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x42, 0x3C, },
    { 0x00, 0x00, 0x00, 0x81, 0x81, 0xC3, 0x7E, 0x3C, },
    { 0x00, 0x00, 0x00, 0x81, 0xC3, 0xFF, 0x7E, 0x3C, },
    { 0x00, 0x00, 0x81, 0xC3, 0xFF, 0xFF, 0x7E, 0x3C, },
    { 0x00, 0x00, 0xC3, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, },
    { 0x00, 0x42, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, },
    // clang-format on
};
#    endif

// we need to draw the moon every frame since snow can overlap with it
static void draw_moon(void) {
    moon_animation_counter = increment_counter(moon_animation_counter, ANIMATE_MOON_EVERY_N_FRAMES);
    if (moon_animation_counter == 0) {
        moon_animation_frame = increment_counter(moon_animation_frame, 14);
    }
    oled_set_cursor(MOON_COLUMN, MOON_LINE);
    oled_write_raw_P(moon_animation[moon_animation_frame], 8);
}
#endif // endregion

#ifdef ENABLE_WAVE // region
uint8_t starry_night_wave_frame_width_counter = 31;
uint8_t rough_waves_frame_counter             = 0;

// clang-format off
static const char PROGMEM ocean_top[8][32] = {
    // still ocean
    {
        0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    },
    // small ripples
    {
        0x20, 0x60, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x20, 0x60, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x20, 0x60, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x20, 0x60, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    },
    // level 2 ripples
    {
        0x20, 0x60, 0x40, 0x40, 0x20, 0x60, 0x40, 0x40,
        0x20, 0x60, 0x40, 0x40, 0x20, 0x60, 0x40, 0x40,
        0x20, 0x60, 0x40, 0x40, 0x20, 0x60, 0x40, 0x40,
        0x20, 0x60, 0x40, 0x40, 0x20, 0x60, 0x40, 0x40,
    },
    // level 3 waves
    {
        0x40, 0x20, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40,
        0x40, 0x20, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40,
        0x40, 0x20, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40,
        0x40, 0x20, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40,
    },
    {
        0x40, 0x40, 0x20, 0x10, 0x28, 0x50, 0x40, 0x40,
        0x40, 0x40, 0x20, 0x10, 0x28, 0x50, 0x40, 0x40,
        0x40, 0x40, 0x20, 0x10, 0x28, 0x50, 0x40, 0x40,
        0x40, 0x40, 0x20, 0x10, 0x28, 0x50, 0x40, 0x40,
    },
    {
        0x40, 0x40, 0x40, 0x20, 0x10, 0x30, 0x70, 0x60,
        0x40, 0x40, 0x40, 0x20, 0x10, 0x30, 0x70, 0x60,
        0x40, 0x40, 0x40, 0x20, 0x10, 0x30, 0x70, 0x60,
        0x40, 0x40, 0x40, 0x20, 0x10, 0x30, 0x70, 0x60,
    },
};
static const char PROGMEM ocean_bottom[8][32] = {
    // still ocean
    {
        0x00, 0x40, 0x40, 0x41, 0x01, 0x01, 0x01, 0x21,
        0x20, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x44,
        0x44, 0x40, 0x40, 0x00, 0x00, 0x08, 0x08, 0x00,
        0x01, 0x01, 0x01, 0x00, 0x40, 0x40, 0x00, 0x00,
    },
    // small ripples
    {
        0x00, 0x00, 0x40, 0x40, 0x01, 0x01, 0x01, 0x20,
        0x20, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
        0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
        0x00, 0x01, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00,
    },
    // level 2 ripples
    {
        0x00, 0x00, 0x40, 0x40, 0x01, 0x01, 0x01, 0x20,
        0x20, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
        0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
        0x00, 0x01, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00,
    },
    // level 3 waves
    {
        0x00, 0x40, 0x40, 0x42, 0x42, 0x03, 0x11, 0x11,
        0x20, 0x20, 0x00, 0x00, 0x08, 0x0C, 0x0C, 0x04,
        0x05, 0x41, 0x41, 0x21, 0x20, 0x00, 0x00, 0x08,
        0x0A, 0x0A, 0x0B, 0x41, 0x41, 0x41, 0x41, 0x00,
    },
    {
        0x10, 0x10, 0x00, 0x80, 0x84, 0xC4, 0x02, 0x06,
        0x84, 0x44, 0xC0, 0x80, 0x80, 0x20, 0x20, 0x10,
        0x08, 0x12, 0x91, 0x81, 0x42, 0x40, 0x00, 0x00,
        0x10, 0x12, 0x22, 0x22, 0x24, 0x04, 0x84, 0x80,
    },
    {
        0x08, 0x80, 0x80, 0x82, 0x82, 0x03, 0x21, 0x21,
        0x10, 0x10, 0x00, 0x00, 0x04, 0x04, 0x0C, 0x08,
        0x09, 0x41, 0x42, 0x22, 0x20, 0x00, 0x00, 0x08,
        0x0A, 0x0A, 0x0B, 0x41, 0x43, 0x42, 0x42, 0x00,
    },
};
// clang-format on

void draw_ocean(uint8_t frame, uint16_t offset, uint8_t byte_index) {
    oled_write_raw_byte(pgm_read_byte(ocean_top[frame] + byte_index), offset);
    oled_write_raw_byte(pgm_read_byte(ocean_bottom[frame] + byte_index), offset + WIDTH);
}

static void animate_waves(void) {
    starry_night_wave_frame_width_counter = decrement_counter(starry_night_wave_frame_width_counter, WIDTH - 1); // only 3 frames for last wave type
    rough_waves_frame_counter             = increment_counter(rough_waves_frame_counter, 3);                     // only 3 frames for last wave type

    for (int i = 0; i < WIDTH; ++i) {
        uint16_t offset     = OCEAN_LINE * WIDTH + i;
        uint8_t  byte_index = starry_night_wave_frame_width_counter + i;
        if (byte_index >= WIDTH) {
            byte_index = byte_index - WIDTH;
        }
        if (is_calm || current_wpm <= WAVE_CALM) {
            draw_ocean(0, offset, byte_index);
        } else if (current_wpm <= WAVE_HEAVY_STORM) {
            draw_ocean(1, offset, byte_index);
        } else if (current_wpm <= WAVE_HURRICANE) {
            draw_ocean(2, offset, byte_index);
        } else {
            draw_ocean(3 + rough_waves_frame_counter, offset, byte_index);
        }
    }
}
#endif // endregion

#ifdef ENABLE_ISLAND // region
uint8_t island_frame_1 = 0;

// clang-format off
// only use 46 bytes (first 18 are blank, so we don't write them, makes it smaller and we can see the shooting stars properly!)

// To save space and allow the shooting stars to be seen, only draw the tree on every frame.
// Tree is only 14bytes wide so we save 108 bytes on just the first row. Second row, the
// first 18 bytes is always the same piece of land, so only store that once, which saves 90 bytes
static const char PROGMEM islandRightTop[6][14] = {
{128,200,236,254,255,254,236,200,128,  0,  0,  0,  0,  0,  },
{0,128,192,192,232,252,252,254,234,200,128,  0,  0,  0,  },
{0,  0, 64, 64,112,224,236,248,252,254,122, 96,192,  0,  },
{0,  0,  0,  0, 48, 96,224,232,248,252,252,222,150,  0,  },
{0,  0,  0,  0,  0,112,200,224,252,240,252,120, 60, 20,  },
{0,  0,  0,  0,  0,  0,128,240,200,240,252,180,212,212,  },
};
static const char PROGMEM islandRightBottom[6][14] = {
{65, 65, 97, 53, 63, 63, 33, 97, 65, 65, 64, 64, 64,128,  },
{64, 64, 96, 48, 52, 63, 63, 97, 65, 67, 66, 64, 64,128,  },
{64, 64, 96, 48, 58, 62, 63, 99, 65, 67, 67, 68, 64,128,  },
{64, 64, 96, 48, 56, 58, 62,127, 71, 65, 67, 70, 64,128,  },
{64, 64, 96, 56, 60, 62, 47,103, 65, 65, 67, 67, 66,128,  },
{64, 64, 96, 48, 56, 62, 46,103, 71, 65, 67, 66, 70,132,  },
};


static const char PROGMEM islandLeft[18] = {
    0x80, 0x40, 0x40, 0x40, 0x40, 0x60,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x60, 0x40, 0x40,
};
// clang-format on

void draw_island_parts(uint8_t frame) {
    oled_set_cursor(ISLAND_COLUMN + 3, ISLAND_LINE);
    oled_write_raw_P(islandRightTop[frame], 14);
    oled_set_cursor(ISLAND_COLUMN + 0, ISLAND_LINE + 1);
    oled_write_raw_P(islandLeft, 18);
    oled_set_cursor(ISLAND_COLUMN + 3, ISLAND_LINE + 1);
    oled_write_raw_P(islandRightBottom[frame], 14);
}

static void animate_island(void) {
    if (animation_counter == 0) {
        island_frame_1 = increment_counter(island_frame_1, 2);
    }

    if (is_calm || current_wpm < ISLAND_CALM) {
        draw_island_parts(0);
    } else if (current_wpm >= ISLAND_CALM && current_wpm < ISLAND_HEAVY_STORM) {
        draw_island_parts(island_frame_1 + 1);
    } else if (current_wpm >= ISLAND_HEAVY_STORM && current_wpm < ISLAND_HURRICANE) {
        draw_island_parts(island_frame_1 + 2);
    } else {
        draw_island_parts(island_frame_1 + 4);
    }
}
#endif // endregion

#ifdef ENABLE_STARS      // region
bool snow_setup = false; // only setup stars once, then we just twinkle them
struct Coordinate {
    int  x;
    int  y;
    bool exists;
};

struct Coordinate stars[TOTAL_STARS]; // tracks all stars/coordinates

/**
 * Setup all the initial stars on the screen
 * This function divides the screen into regions based on STARS_PER_LINE and NUMBER_OF_STAR_LINES
 * where each line is made up of 8x8 pixel groups, that are populated by a single star.
 *
 * Not sure how this function will work with larger or smaller screens.
 * It should be fine, as long as the screen width is a multiple of 8
 */
static void spawn_snow(void) {
    // For every line, split the line into STARS_PER_LINE, find a random point in that region, and turn the pixel on
    // 36% probability it will not be added
    // (said another way, 80% chance it will start out lit in the x direction, then 80% chance it will start out lit in the y direction = 64% probability it will start out lit at all)
    for (int line = 0; line < NUMBER_OF_STAR_LINES; ++line) {
        for (int column_group = 0; column_group < STARS_PER_LINE; ++column_group) {
            uint8_t rand_column = simple_rand() % 10;
            uint8_t rand_row    = simple_rand() % 10;
            if (rand_column < 8 && rand_row < 8) {
                int column_adder = column_group * 8;
                int line_adder   = line * 8;
                int x            = rand_column + column_adder;
                int y            = rand_row + line_adder;
                oled_write_pixel(x, y, true);
                stars[column_group + (line * STARS_PER_LINE)].x      = x;
                stars[column_group + (line * STARS_PER_LINE)].y      = y;
                stars[column_group + (line * STARS_PER_LINE)].exists = true;
            } else {
                stars[column_group + (line * STARS_PER_LINE)].exists = false;
            }
        }
    }
    snow_setup = true;
}

static void move_snow(void) {
    for (int i = 0; i < TOTAL_STARS; i++) {
        struct Coordinate *star = &stars[i];
        if (!star->exists) {
            continue;
        }

        oled_write_pixel(star->x, star->y, false);

        uint8_t x_delta, y_delta;
        if (is_calm || current_wpm <= WAVE_CALM) {
            x_delta = 0;
            y_delta = simple_rand() % 2;
        } else if (current_wpm <= WAVE_HEAVY_STORM) {
            x_delta = simple_rand() % 2;
            y_delta = 1;
        } else if (current_wpm <= WAVE_HURRICANE) {
            x_delta = 1 + simple_rand() % 2;
            y_delta = 1 + simple_rand() % 2;
        } else {
            x_delta = 1 + simple_rand() % 5;
            y_delta = 1 + simple_rand() % 2;
        }
        star->x += x_delta;
        star->y += y_delta;

        // its slightly less than the full width to prevent snow clipping into
        // the ground
        if (star->y >= OLED_DISPLAY_WIDTH - 20) {
            star->y = 0;
        }
        if (star->x >= OLED_DISPLAY_HEIGHT) {
            star->x = 0;
        }
        oled_write_pixel(star->x, star->y, true);
    }
}

static void animate_snow(void) {
    if (!snow_setup) {
        spawn_snow();
    } else {
        move_snow();
    }
}
#endif // endregion

#ifdef ENABLE_SHOOTING_STARS       // region
bool shooting_stars_setup = false; // only setup shooting stars array once with defaults

struct ShootingStar {
    int  x_1;
    int  y_1;
    int  x_2;
    int  y_2;
    bool running;
    int  frame;
    int  delay;
};

struct ShootingStar shooting_stars[MAX_NUMBER_OF_SHOOTING_STARS]; // tracks all the shooting stars

static void setup_shooting_star(struct ShootingStar *shooting_star) {
    int column_to_start = simple_rand() % (WIDTH / 2);
    int row_to_start    = simple_rand() % (HEIGHT - 48); // shooting_stars travel diagonally 1 down, 1 across. So the lowest a shooting_star can start and not 'hit' the ocean is 32 above the ocean.

    shooting_star->x_1     = column_to_start;
    shooting_star->y_1     = row_to_start;
    shooting_star->x_2     = column_to_start + 1;
    shooting_star->y_2     = row_to_start + 1;
    shooting_star->running = true;
    shooting_star->frame++;
    shooting_star->delay = simple_rand() % SHOOTING_STAR_DELAY;
}

static void move_shooting_star(struct ShootingStar *shooting_star) {
    oled_write_pixel(shooting_star->x_1, shooting_star->y_1, false);
    oled_write_pixel(shooting_star->x_2, shooting_star->y_2, false);

    shooting_star->x_1++;
    shooting_star->y_1++;
    shooting_star->x_2++;
    shooting_star->y_2++;
    shooting_star->frame++;

    oled_write_pixel(shooting_star->x_1, shooting_star->y_1, true);
    oled_write_pixel(shooting_star->x_2, shooting_star->y_2, true);
}

static void finish_shooting_star(struct ShootingStar *shooting_star) {
    oled_write_pixel(shooting_star->x_1, shooting_star->y_1, false);
    oled_write_pixel(shooting_star->x_2, shooting_star->y_2, false);
    shooting_star->running = false;
    shooting_star->frame   = 0;
}

static void animate_shooting_star(struct ShootingStar *shooting_star) {
    if (shooting_star->frame > SHOOTING_STAR_FRAMES) {
        finish_shooting_star(shooting_star);
        return;
    } else if (!shooting_star->running) {
        setup_shooting_star(shooting_star);
    } else {
        if (shooting_star->delay == 0) {
            move_shooting_star(shooting_star);
        } else {
            shooting_star->delay--;
        }
    }
}

static void animate_shooting_stars(void) {
    if (is_calm) {
        return;
    }
    if (!shooting_stars_setup) {
        for (int i = 0; i < MAX_NUMBER_OF_SHOOTING_STARS; ++i) {
            shooting_stars[i].running = false;
        }
        shooting_stars_setup = true;
    }
    /**
     * Fixes issue with stars that were falling _while_ the
     * wpm dropped below the condition for them to keep falling
     */
    void end_extra_stars(uint8_t starting_index) {
        for (int shooting_star_index = starting_index; shooting_star_index < MAX_NUMBER_OF_SHOOTING_STARS; ++shooting_star_index) {
            struct ShootingStar shooting_star = shooting_stars[shooting_star_index];
            if (shooting_star.running) {
                finish_shooting_star(&shooting_star);
                shooting_stars[shooting_star_index] = shooting_star;
            }
        }
    }

    int number_of_shooting_stars = current_wpm / SHOOTING_STAR_WPM_INCREMENT;
    number_of_shooting_stars     = (number_of_shooting_stars > MAX_NUMBER_OF_SHOOTING_STARS) ? MAX_NUMBER_OF_SHOOTING_STARS : number_of_shooting_stars;

    if (number_of_shooting_stars == 0) {
        // make sure all shooting_stars are ended
        end_extra_stars(0);
    } else {
        for (int shooting_star_index = 0; shooting_star_index < number_of_shooting_stars; ++shooting_star_index) {
            struct ShootingStar shooting_star = shooting_stars[shooting_star_index];
            animate_shooting_star(&shooting_star);
            shooting_stars[shooting_star_index] = shooting_star;
        }
        end_extra_stars(number_of_shooting_stars);
    }
}
#endif // endregion

/**
 * Main rendering function
 *
 * Calls all different animations at different rates
 */
void render_stars(void) {
    current_wpm = get_current_wpm();
    if (current_wpm > 0) {
        oled_on();
        starry_night_anim_sleep = timer_read32();
    } else if (timer_elapsed32(starry_night_anim_sleep) > OLED_TIMEOUT) {
        oled_off();
        return;
    }

    // animation timer
    if (timer_elapsed32(starry_night_anim_timer) > STARRY_NIGHT_ANIM_FRAME_DURATION) {
        starry_night_anim_timer = timer_read32();

#ifdef ENABLE_ISLAND
        animate_island();
#endif

#ifdef ENABLE_SHOOTING_STARS
        if (animation_counter % SHOOTING_STAR_ANIMATION_MODULATOR == 0) {
            animate_shooting_stars();
        }
#endif

#ifdef ENABLE_STARS
        // TODO offsetting the star animation from the wave animation would look better,
        // but if I do that, then the stars appear in the water because
        // the ocean animation has to wait a bunch of frames to overwrite it.
        // Possible solutions:
        // 1. Only draw stars to the top of the island/ocean.
        // 2. Draw ocean every frame, only move ocean on frames matching modulus
        // Problems:
        // 1. What if someone wants to move the island up a bit, or they want to have the stars reflect in the water?
        // 2. More cpu intensive. And I'm already running out of cpu as it is...
        if (animation_counter % STAR_ANIMATION_MODULATOR == 0) {
            animate_snow();
        }
#endif

#ifdef ENABLE_WAVE
        if (animation_counter % OCEAN_ANIMATION_MODULATOR == 0) {
            animate_waves();
        }
#endif

#ifdef ENABLE_MOON
        draw_moon();
#endif

        animation_counter = increment_counter(animation_counter, NUMBER_OF_FRAMES);
    }
}
