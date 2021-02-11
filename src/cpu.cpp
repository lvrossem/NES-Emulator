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

bool CPU::get_bit_by_index(uint8_t arg, uint8_t i) {
    return (arg & (0x01 << (7 - i))) >> i;
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
        execute_1B(opcode);

    } else if (group_2A.count(static_cast<Instruction>(opcode & 0xE7)) == 1) {
        // Instruction is of type 2A
        execute_2A(opcode);

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

void CPU::execute_1B(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode & 0xE3);

    // Operand needed to execute; determined based on addressing method
    uint8_t arg = 0;
    uint16_t address = 0;
    bool acc_only = false;
    bool immediate = false;

    switch ((opcode & 0x1C) >> 2) {
        case 0b000: {
            // Immediate
            arg = next_prg_byte();
            immediate = true;

            break;
        }

        case 0b001: {
            // Zero page
            address = next_prg_byte();

            break;
        }

        case 0b010: {
            // Accumulator
            acc_only = true;

            break;
        }

        case 0b011: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            address = ((uint16_t) upper_arg << 8) | lower_arg;

            break;
        }

        case 0b101: {
            // Zero page X (Y in LDX)
            if (instruction != LDX) {
                address = next_prg_byte() + X;
            } else {
                address = next_prg_byte() + Y;
            }
            
            break;
        }

        case 0b111: {
            // Absolute X (Y in LDX)
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            if (instruction != LDX) {
                address = (((uint16_t) upper_arg << 8) | lower_arg) + X;
            } else {
                address = (((uint16_t) upper_arg << 8) | lower_arg) + Y;
            }

            break;
        }

        default: {
            std::cout << "Unknown 1B opcode" << std::endl;
        }
    }

    handle_registers_1B(instruction, arg, address, acc_only, immediate);
}

void CPU::execute_2A(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode & 0xE7);

    // Operand needed to execute; determined based on addressing method
    uint16_t address = 0;
    bool acc_only = false;
    bool immediate = false;

    switch ((opcode & 0x18) >> 3) {
        case 0b00: {
            // Zero page
            address = next_prg_byte();

            break;
        }

        case 0b01: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            address = ((uint16_t) upper_arg << 8) | lower_arg;

            break;
        }

        case 0b10: {
            // Zero page X (Y in STX)
            uint8_t next = next_prg_byte();
            address = instruction == STX ? next + Y : next + X;

            break;
        }

        case 0b11: {
            // Absolute X
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            address = (((uint16_t) upper_arg << 8) | lower_arg) + X;

            break;
        }

        default: {
            std::cout << "Unknown 2A opcode" << std::endl;
        }
    }

    handle_registers_2A(instruction, address);
}

void CPU::handle_registers_1A(Instruction instruction, uint8_t arg) {
    switch (instruction) {
        case ADC: {
            // ADC instruction
            bool carry = get_status_bit(Carry);

            // Set overflow if both operands are negative or positive
            set_status_bit(Overflow, (get_bit_by_index(arg + carry, 0) == get_bit_by_index(A, 0)));
            set_status_bit(Carry, 0xFF - A < arg + carry);

            A += arg + carry;

            set_status_bit(Negative, get_bit_by_index(A, 0) == 1);
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
            set_status_bit(Negative, get_bit_by_index(A - arg, 0) == 1);

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

            // SBC n is identical to ADC (n EOR 0xFF)
            arg ^= 0xFF;
            uint8_t carry = get_status_bit(Carry);

            set_status_bit(Overflow, ((arg + carry) & 0x80) == (A & 0x80));
            set_status_bit(Carry, 0xFF - A < arg + carry);

            A += arg + carry;

            set_status_bit(Negative, ((A & 0x80) >> 7) == 1);
            set_status_bit(Zero, A == 0);
            
            break;
        }

        case STA: {
            // STA instruction
            cpu_memory[arg] = A;

            break;
        }

        default: {
            std::cout << "Unknown 1A instruction" << std::endl;
        }
    }
}

void CPU::handle_registers_1B(Instruction instruction, uint8_t arg, uint16_t address, bool acc_only, bool immediate) {
    switch (instruction) {
        case ASL: {
            // ASL instruction
            if (acc_only) {
                set_status_bit(Carry, get_bit_by_index(A, 0) == 1);
                A <<= 1;
                set_status_bit(Zero, A == 0);
                set_status_bit(Negative, get_bit_by_index(A, 0) == 1);
            } else {
                set_status_bit(Carry, get_bit_by_index(cpu_memory[address], 0) == 1);
                cpu_memory[address] <<= 1;
                set_status_bit(Zero, cpu_memory[address] == 0);
                set_status_bit(Negative, get_bit_by_index(cpu_memory[address], 0) == 1);
            }

            break;
        }

        case LDX: {
            // LDX instruction
            X = immediate ? arg : cpu_memory[address];
            set_status_bit(Zero, X == 0);
            set_status_bit(Negative, get_bit_by_index(X, 0) == 1);

            break;
        }

        case LDY: {
            // LDY instruction
            Y = immediate ? arg : cpu_memory[address];
            set_status_bit(Zero, Y == 0);
            set_status_bit(Negative, get_bit_by_index(Y, 0) == 1);

            break;
        }

        case LSR: {
            // LSR instruction
            set_status_bit(Negative, false);

            if (acc_only) {
                set_status_bit(Carry, get_bit_by_index(A, 7));
                A >>= 1;
                set_status_bit(Zero, A == 0);
            } else {
                set_status_bit(Carry, get_bit_by_index(cpu_memory[address], 7));
                cpu_memory[address] >>= 1;
                set_status_bit(Zero, cpu_memory[address] == 0);
            }

            break;
        }

        case ROR: {
            // ROR instruction
            if (acc_only) {
                uint8_t new_carry = get_bit_by_index(A, 7);
                A >>= 1;
                A += get_status_bit(Carry) * 0x80;

                set_status_bit(Carry, new_carry);
                set_status_bit(Zero, A == 0);
                set_status_bit(Negative, get_bit_by_index(A, 0) == 1);
            } else {
                uint8_t new_carry = get_bit_by_index(cpu_memory[address], 7);
                cpu_memory[address] >>= 1;
                cpu_memory[address] += get_status_bit(Carry) * 0x80;

                set_status_bit(Carry, new_carry);
                set_status_bit(Zero, cpu_memory[address] == 0);
                set_status_bit(Negative, get_bit_by_index(cpu_memory[address], 0) == 1);
            }   
            
            break;
        }

        case ROL: {
            // ROL instruction
            if (acc_only) {
                uint8_t new_carry = get_bit_by_index(A, 0);
                A <<= 1;
                A += get_status_bit(Carry);

                set_status_bit(Carry, new_carry);
                set_status_bit(Zero, A == 0);
                set_status_bit(Negative, get_bit_by_index(A, 0) == 1);
            } else {
                uint8_t new_carry = get_bit_by_index(cpu_memory[address], 0);
                cpu_memory[address] <<= 1;
                cpu_memory[address] += get_status_bit(Carry);

                set_status_bit(Carry, new_carry);
                set_status_bit(Zero, cpu_memory[address] == 0);
                set_status_bit(Negative, get_bit_by_index(cpu_memory[address], 0) == 1);
            }

            break;
        }

        default: {
            std::cout << "Unknown 1B instruction" << std::endl;
        }
    }
}

void CPU::handle_registers_2A(Instruction instruction, uint16_t address) {
    switch (instruction) {
        case DEC: {
            // DEC instruction
            cpu_memory[address] -= 1;
            set_status_bit(Zero, cpu_memory[address] == 0);
            set_status_bit(Negative, get_bit_by_index(cpu_memory[address], 0) == 1);

            break;
        }

        case INC: {
            // INC instruction
            cpu_memory[address] += 1;
            set_status_bit(Zero, cpu_memory[address] == 0);
            set_status_bit(Negative, get_bit_by_index(cpu_memory[address], 0) == 1);

            break;
        }

        case STX: {
            // STX instruction
            cpu_memory[address] = X;

            break;
        }

        case STY: {
            // STY instruction
            cpu_memory[address] = Y;

            break;
        }

        default: {
            std::cout << "Unknown 2A instruction" << std::endl;
        }
    }
}