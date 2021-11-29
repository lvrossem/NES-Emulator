#include "ppu.h"

PPU::PPU(CPU* cpu_ptr) {
    cpu = cpu_ptr;
}

void PPU::draw() {
    uint32_t display[FRAME_HEIGHT * 8][FRAME_WIDTH * 8];
    for (int y = 0; y < FRAME_HEIGHT; y++) {
        for (int x = 0; x < FRAME_WIDTH; x++) {
            int name_table_index = y * FRAME_WIDTH + x;
            int pattern_table_index = ppu_memory[NAME_TABLE_BOTTOM + name_table_index];

            uint8_t lower_eight[8];
            uint8_t upper_eight[8];
            uint8_t result[8];

            std::memcpy(lower_eight, &ppu_memory[PATTERN_TABLE_BOTTOM + pattern_table_index*16], 8);
            std::memcpy(upper_eight, &ppu_memory[PATTERN_TABLE_BOTTOM + pattern_table_index*16 + 8], 8);

            for (int i = 0; i < 8; i++) {
                result[i] = lower_eight[i] | upper_eight[i];
            }

            for (int i = 0; i < 8; i++) {
                display[y*8 + i][x*8] = result[i];
            }

            
            // Add color palette shenanagans later
        }
    }
}