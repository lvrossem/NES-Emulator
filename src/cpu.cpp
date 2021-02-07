#include "cpu.h"

CPU::CPU() {
    initialize();
}

void CPU::initialize() {
    PC = 0;
    SP = 0;
    A = 0;
    X = 0;
    Y = 0;
    P = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        cpu_memory[i] = 0;
    }
}

void CPU::write_to_memory(char* data, uint16_t start, uint16_t size) {
    assert(MEMORY_SIZE - size >= start);

    for (int i = 0; i < size; i++) {
        cpu_memory[start + i] = data[i];
    }
}