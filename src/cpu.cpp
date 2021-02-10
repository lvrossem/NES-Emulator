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

void CPU::flip_carry() {
    P ^= 0b10000000;
}

void CPU::flip_zero() {
    P ^= 0b01000000;
}

void CPU::flip_interrupt_disable() {
    P ^= 0b00100000;
} 

void CPU::flip_break() {
    P ^= 0b00001000;
} 

void CPU::flip_overflow() {
    P ^= 0b00000010;
}

void CPU::flip_negative(){
    P ^= 0b00000001;
}

uint8_t CPU::next_prg_byte() {
    return cpu_memory[PC++];
}