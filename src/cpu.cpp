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

void CPU::set_status_bit(StatusBit bit, bool flag) {
    P = flag ? P | (0b00000001 << bit) : P & (0b11111110 << bit);
}

void CPU::handle_status_ADC(uint8_t arg) {
    if (0xFF - arg < A) {
        set_status_bit(Carry, true);
    }

    A += arg;
}

uint8_t CPU::next_prg_byte() {
    return cpu_memory[PC++];
}

void CPU::execute() {
    uint8_t opcode = next_prg_byte();

    // Start with 1A instructions
    switch (opcode & 0xE3) {
        case 0x61: {
            // ADC Instruction
            switch ((opcode & 0x1C) >> 2) {
                case 0b010: {
                    // Immediate
                    uint8_t arg = next_prg_byte();
                    handle_status_ADC(arg);
                    break;
                }

                case 0b001: {
                    // Zero page
                    uint8_t arg = next_prg_byte();
                    handle_status_ADC(cpu_memory[arg]);
                    break;
                }

                case 0b011: {
                    // Absolute
                    uint8_t lower_arg = next_prg_byte();
                    uint8_t upper_arg = next_prg_byte();

                    uint16_t total_arg = ((uint16_t) upper_arg << 8) | lower_arg;
                    handle_status_ADC(cpu_memory[total_arg]);
                    break;
                }
                
                case 0b101: {
                    // Zero page X
                    uint8_t arg = next_prg_byte() + X;
                    handle_status_ADC(cpu_memory[arg]);
                    break;
                }

                case 0b111: {
                    // Absolute X 
                    uint8_t lower_arg = next_prg_byte();
                    uint8_t upper_arg = next_prg_byte();

                    uint16_t total_arg = ((uint16_t) upper_arg << 8) | lower_arg;
                    handle_status_ADC(cpu_memory[total_arg + X]);
                    break;
                }

                case 0b110: {
                    // Absolute Y
                    uint8_t lower_arg = next_prg_byte();
                    uint8_t upper_arg = next_prg_byte();

                    uint16_t total_arg = ((uint16_t) upper_arg << 8) | lower_arg;
                    handle_status_ADC(cpu_memory[total_arg + Y]);
                    break;
                }

                case 0b000: {
                    // Indirect X
                    uint8_t arg = next_prg_byte();
                    handle_status_ADC(cpu_memory[arg + X]);
                    break;
                }

                case 0b100: {
                    // Indirect Y (DIFFERENT FROM Indirect X!)
                    uint8_t arg = next_prg_byte();
                    handle_status_ADC(cpu_memory[cpu_memory[arg] + Y]);
                    break;
                }

            }
            break;
            
        }
    }
}