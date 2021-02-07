#ifndef PPU_H
#define PPU_H

#define VRAM_SIZE 16384 // 16 KiB

#include <stdint.h>

// Picture processing unit
class PPU {
private:
    // Only the lowest 16 KiB is really used, but we have 48 KiB more for mirrors
    uint8_t ppu_memory[VRAM_SIZE * 4];

};

#endif