#include "ppu.h"

PPU::PPU() {
    display = new uint32_t*[FRAME_HEIGHT * 8];
    for (int i = 0; i < FRAME_HEIGHT * 8; i++) {
        display[i] = new uint32_t[FRAME_WIDTH * 8];
    }
}

PPU::~PPU() {
    for (int i = 0; i < FRAME_HEIGHT * 8; i++) {
        delete[] display[i];
    }

    delete[] display;
}

void PPU::fill_tile(uint8_t tile_x, uint8_t tile_y, uint8_t palette_index) {
    int name_table_index = tile_y * FRAME_WIDTH + tile_x;

    int pattern_table_index = bus->read_from_ppu(NAME_TABLE_BOTTOM + name_table_index);

    // Fill all 8x8 pixels of the tile
    for (int y = 0; y < 8; y++) {

        uint16_t lower = bus->read_from_ppu(PATTERN_TABLE_BOTTOM + pattern_table_index*16 + y);
        uint16_t upper = bus->read_from_ppu(PATTERN_TABLE_BOTTOM + pattern_table_index*16 + y + 8);

        for (int x = 0; x < 8; x++) {
            uint8_t shift = 7 - x;
            // Combine 1 bit from upper with 1 from lower to get te index to use in the palette
            uint8_t frame_palette_index = ((upper & (0b1 << shift)) >> (shift - 1)) | ((lower & (0b1 << shift)) >> shift);

            // Assign the correct color to the current pixel
            display[tile_y * 8 + y][tile_x * 8 + x] = colour_palette[bus->read_from_ppu(SPRITE_PALETTE_BOTTOM + palette_index * 4 + frame_palette_index)];
        }
    }
}

void PPU::fill_quadrant(uint8_t upper_left_tile_x, uint8_t upper_left_tile_y, uint8_t palette_index) {
    // Upper left tile
    fill_tile(upper_left_tile_x, upper_left_tile_y, palette_index);

    // Upper right tile
    fill_tile(upper_left_tile_x + 1, upper_left_tile_y, palette_index);

    // Lower left tile
    fill_tile(upper_left_tile_x, upper_left_tile_y + 1, palette_index);

    // Lower right tile
    fill_tile(upper_left_tile_x + 1, upper_left_tile_y + 1, palette_index);
}

void PPU::fill_block(uint8_t block_nr) {
    uint8_t attribute_byte = bus->read_from_ppu(ATTRIBUTE_TABLE_BOTTOM + block_nr);

    uint8_t upper_left_tile_x = (block_nr * 4) % 32;
    uint8_t upper_left_tile_y = (block_nr / 8) * 4;

    // Fill quadrant 0
    fill_quadrant(upper_left_tile_x, upper_left_tile_y, attribute_byte & 0b11);

    // Fill quadrant 1
    fill_quadrant(upper_left_tile_x + 2, upper_left_tile_y, attribute_byte & 0b1100);

    // Fill quadrant 2
    fill_quadrant(upper_left_tile_x, upper_left_tile_y + 2, attribute_byte & 0b110000);

    // Fill quadrant 3
    fill_quadrant(upper_left_tile_x + 2, upper_left_tile_y + 2, attribute_byte & 0b11000000);
}

void PPU::draw() {
    // Iterate over the 64 blocks
    for (uint8_t block_nr = 0; block_nr < 64; block_nr++) {

        for (uint8_t quadrant_nr = 0; quadrant_nr < 4; quadrant_nr++) {
            fill_block(block_nr);
        }
    }
}