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

bool CPU::get_status_bit(StatusBit bit) {
    return (P >> bit) & 0b00000001;
} 

uint8_t CPU::next_prg_byte() {
    return cpu_memory[PC++];
}

void CPU::execute() {
    uint8_t opcode = next_prg_byte();

    if (group_1A.count(static_cast<Instruction>(opcode & 0xE3)) == 1) {
        // Instruction is of type 1A
        execute_1A(opcode);
    } else if (group_1B.count(static_cast<Instruction>(opcode & 0xE3)) == 1) {
        // Instruction is of type 1B
    } else if (group_2A.count(static_cast<Instruction>(opcode & 0xE7)) == 1) {
        // Instruction is of type 2A
    } else if (group_2B.count(static_cast<Instruction>(opcode & 0xF3)) == 1) {
        // Instruction is of type 2B
    } else if (group_3A.count(static_cast<Instruction>(opcode & 0xF7)) == 1) {
        // Instruction is of type 3A
    } else if (group_3B.count(static_cast<Instruction>(opcode & 0xDF)) == 1) {
        // Instruction is of type 3B
    } else {
        // Instruction is of type 4
    }
}

void CPU::execute_1A(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode & 0xE3);

    // Operand needed to execute; determined based on addressing method
    uint8_t arg;
    switch ((opcode & 0x1C) >> 2) {
        case 0b010: {
            // Immediate
            arg = next_prg_byte();
            break;
        }

        case 0b001: {
            // Zero page
            arg = cpu_memory[next_prg_byte()];
            break;
        }

        case 0b011: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            uint16_t total_arg = ((uint16_t) upper_arg << 8) | lower_arg;

            arg = cpu_memory[total_arg];
            break;
        }
        
        case 0b101: {
            // Zero page X
            arg = cpu_memory[next_prg_byte() + X];
            break;
        }

        case 0b111: {
            // Absolute X 
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            uint16_t total_arg = ((uint16_t) upper_arg << 8) | lower_arg;

            arg = cpu_memory[total_arg + X];
            break;
        }

        case 0b110: {
            // Absolute Y
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            uint16_t total_arg = ((uint16_t) upper_arg << 8) | lower_arg;

            arg = cpu_memory[total_arg + Y];
            break;
        }

        case 0b000: {
            // Indirect X
            arg = cpu_memory[next_prg_byte() + X];
            break;
        }

        case 0b100: {
            // Indirect Y (DIFFERENT FROM Indirect X!)
            arg = cpu_memory[cpu_memory[next_prg_byte()] + Y];
            break;
        }

        default: {
            std::cout << "Unknown 1A opcode" << std::endl;
        }
    }

    handle_registers_1A(instruction, arg);
}

void CPU::handle_registers_1A(Instruction instruction, uint8_t arg) {
    switch (instruction) {
        case ADC: {
            // ADC instruction
            // TODO: add carry bit
            set_status_bit(Carry, 0xFF - arg < A);
            A += arg;
            set_status_bit(Negative, ((A & 0x80) >> 7) == 1);
            set_status_bit(Zero, A == 0);

            break;
        }

        case AND: {
            // AND instruction
            A &= arg;
            set_status_bit(Zero, A == 0);
            
            break;
        }

        case CMP: {
            // CMP instruction
            set_status_bit(Zero, A == arg);
            set_status_bit(Carry, arg <= A);
            set_status_bit(Negative, (((A - arg) & 0x80) >> 7) == 1);

            break;
        }

        case EOR: {
            // EOR instruction
            A ^= arg;
            set_status_bit(Zero, A == 0);
            set_status_bit(Negative, ((A & 0x80) >> 7) == 1);

            break;
        }

        case LDA: {
            // LDA instruction
            A = arg;
            set_status_bit(Zero, A == 0);
            set_status_bit(Negative, ((A & 0x80) >> 7) == 1);

            break;
        }

        case ORA: {
            // ORA instruction
            A |= arg;
            set_status_bit(Zero, A == 0);
            set_status_bit(Negative, ((A & 0x80) >> 7) == 1);

            break;
        }

        case SBC: {
            // SBC instruction
            
            
            break;
        }

        case STA: {
            // STA instruction
            cpu_memory[arg] = A;

            break;
        }
    }
}