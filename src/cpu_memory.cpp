#include "cpu_memory.h"

CPUMemory::CPUMemory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

uint8_t& CPUMemory::operator[](uint16_t index) {
    return memory[index];
}

void CPUMemory::write_byte(uint16_t address, uint8_t byte) {
    assert(address < MEMORY_SIZE);

    memory[address] = byte;
}

uint8_t CPUMemory::read_byte(uint16_t address) {
    return memory[address];
}