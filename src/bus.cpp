#include "bus.h"

Bus::Bus() {
    reset();
}

void Bus::reset() {
    memset(cpu_memory, 0, CPU_MEMORY_SIZE);
    memset(ppu_memory, 0, PPU_MEMORY_SIZE * 4);
    memset(spr_ram, 0, SPR_RAM_SIZE);
}

uint8_t Bus::read_from_cpu(uint16_t address) {
    return cpu_memory[address];
}

void Bus::write_to_cpu(uint16_t address, uint8_t value) {
    cpu_memory[address] = value;
}

uint8_t Bus::read_from_ppu(uint16_t address) {
    return ppu_memory[address];
}

void Bus::write_to_ppu(uint16_t address, uint8_t value) {
    ppu_memory[address] = value;
}