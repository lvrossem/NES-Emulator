#ifndef PPU_H
#define PPU_H

#define VRAM_SIZE 0x4000 // 16 KiB
#define SPR_RAM_SIZE 0x100 // 256 bytes

#include <stdint.h>

#include "cpu.h"

// Picture processing unit
class PPU {
private:
    // Only the lowest 16 KiB is really used, but we have 48 KiB more for mirrors
    uint8_t ppu_memory[VRAM_SIZE * 4];

    // Memory to store sprite attributes
    uint8_t spr_ram[SPR_RAM_SIZE];

    CPU* cpu;

public:
    PPU(CPU* cpu_ptr);
};

#endif