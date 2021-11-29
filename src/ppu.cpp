#include "ppu.h"

PPU::PPU() {}

void PPU::draw() {
    uint32_t display[FRAME_HEIGHT * 8][FRAME_WIDTH * 8];

    // Iterate over the name table tiles 
    for (int y = 0; y < FRAME_HEIGHT; y++) {
        for (int x = 0; x < FRAME_WIDTH; x++) {
            int name_table_index = y * FRAME_WIDTH + x;
            int pattern_table_index = bus->read_from_ppu(NAME_TABLE_BOTTOM + name_table_index);

            uint8_t lower_eight[8];
            uint8_t upper_eight[8];
            uint8_t result[8];

            for (int i = 0; i < 8; i++) {
                // Split the 16 bytes found by the index into the lower half and upper half
                lower_eight[i] = bus->read_from_ppu(PATTERN_TABLE_BOTTOM + pattern_table_index*16 + i);
                upper_eight[i] = bus->read_from_ppu(PATTERN_TABLE_BOTTOM + pattern_table_index*16 + 8 + i);

                result[i] = lower_eight[i] | upper_eight[i];

                display[y*8 + i][x*8] = result[i];
            }
            
            // Add color palette shenanagans later
        }
    }
}