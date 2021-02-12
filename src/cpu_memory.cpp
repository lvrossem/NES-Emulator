#include "cpu_memory.h"

CPUMemory::CPUMemory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

uint8_t& CPUMemory::operator[](uint16_t index) {
    return memory[index];
}