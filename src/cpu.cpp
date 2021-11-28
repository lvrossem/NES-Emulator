#include "cpu.h"

CPU::CPU() {
    initialize();
}

CPU::~CPU() {
    delete cpu_memory;
}

void CPU::initialize() {
    cpu_memory = new CPUMemory();

    PC = 0;
    SP = cpu_memory->get_top_of_stack();
    A = 0;
    X = 0;
    Y = 0;
    P = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        cpu_memory->write_byte(i, 0);
    }
}

void CPU::write_data_to_memory(char* data, uint16_t start, uint16_t size) {
    assert(MEMORY_SIZE - size >= start);

    for (int i = 0; i < size; i++) {
        cpu_memory->write_byte(start + i, data[i]);
    }
}

void CPU::set_status_bit(StatusBit bit, bool flag) {
    P = flag ? P | (0b00000001 << bit) : P & ~(0b00000001 << bit);
}

bool CPU::get_status_bit(StatusBit bit) {
    return (P >> bit) & 0b00000001;
} 

bool CPU::get_bit_by_index(uint8_t arg, uint8_t i) {
    return (arg & (0b00000001 << i)) >> i;
}

uint16_t CPU::merge_uint8_t(uint8_t upper, uint8_t lower) {
    return ((uint16_t) upper << 8) | lower;
}

uint8_t CPU::next_prg_byte() {
    return cpu_memory->read_byte(PC++);
}

void CPU::push(uint8_t byte) {
    cpu_memory->write_byte(SP, byte);
    SP--;
}

uint8_t CPU::pop() {
    SP++;
    return cpu_memory->read_byte(SP);
}

void CPU::interrupt(InterruptType type) {
    if (get_status_bit(InterruptDisable) && type == IRQ) {
        return;
    }

    uint8_t lower_PC = static_cast<uint8_t>(PC & 0x00FF);
    uint8_t upper_PC = static_cast<uint8_t>((PC & 0xFF00) >> 8);

    push(lower_PC);
    push(upper_PC);
    push(P);

    set_status_bit(InterruptDisable, true);

    switch (type) {
        case IRQ: {
            // Maskable interrupt
            PC = merge_uint8_t(cpu_memory->read_byte(0xFFFF), cpu_memory->read_byte(0xFFFE));

            break;
        }

        case NMI: {
            // Non-maskable interrupt
            PC = merge_uint8_t(cpu_memory->read_byte(0xFFFB), cpu_memory->read_byte(0xFFFA));

            break;
        }

        case RES: {
            // Reset
            PC = merge_uint8_t(cpu_memory->read_byte(0xFFFD), cpu_memory->read_byte(0xFFFC));

            break;
        }
        
        default: {
            std::cout << "Invalid interrupt type" << std::endl;

            break;
        }
    }
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
        execute_2B(opcode);

    } else if (group_3A.count(static_cast<Instruction>(opcode & 0xF7)) == 1) {
        // Instruction is of type 3A
        execute_3A(opcode);

    } else if (group_3B.count(static_cast<Instruction>(opcode & 0xDF)) == 1) {
        // Instruction is of type 3B
        execute_3B(opcode);

    } else {
        // Instruction is of type 4
        execute_4(opcode);
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
            arg = cpu_memory->read_byte(next_prg_byte());
    
            break;
        }

        case 0b011: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            uint16_t total_arg = merge_uint8_t(upper_arg, lower_arg);

            arg = cpu_memory->read_byte(total_arg);
            
            break;
        }
        
        case 0b101: {
            // Zero page X
            arg = cpu_memory->read_byte(next_prg_byte() + X);
            
            break;
        }

        case 0b111: {
            // Absolute X 
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            uint16_t total_arg = merge_uint8_t(upper_arg, lower_arg);

            arg = cpu_memory->read_byte(total_arg + X);
            
            break;
        }

        case 0b110: {
            // Absolute Y
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            uint16_t total_arg = merge_uint8_t(upper_arg, lower_arg);

            arg = cpu_memory->read_byte(total_arg + Y);
            break;
        }

        case 0b000: {
            // Indirect X
            arg = cpu_memory->read_byte(next_prg_byte() + X);
    
            break;
        }

        case 0b100: {
            // Indirect Y (DIFFERENT FROM Indirect X!)
            arg = cpu_memory->read_byte(cpu_memory->read_byte(next_prg_byte()) + Y);
            
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

            address = merge_uint8_t(upper_arg, lower_arg);

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
                address = merge_uint8_t(upper_arg, lower_arg) + X;
            } else {
                address = merge_uint8_t(upper_arg, lower_arg) + Y;
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

            address = merge_uint8_t(upper_arg, lower_arg);

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

            address = merge_uint8_t(upper_arg, lower_arg) + X;

            break;
        }

        default: {
            std::cout << "Unknown 2A opcode" << std::endl;
        }
    }

    handle_registers_2A(instruction, address);
}

void CPU::execute_2B(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode & 0xF3);

    uint8_t arg;
    switch ((opcode & 0x0C) >> 2) {
        case 0b00: {
            // Immediate
            arg = next_prg_byte();

            break;
        }

        case 0b01: {
            // Zero page
            arg = cpu_memory->read_byte(next_prg_byte());
    
            break;
        }

        case 0b11: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            arg = cpu_memory->read_byte(merge_uint8_t(upper_arg, lower_arg));

            break;
        }

        default: {
            std::cout << "Unknown 2B opcode" << std::endl;
        }
    }

    handle_registers_2B(instruction, arg);
}

void CPU::execute_3A(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode & 0xF7);

    uint8_t arg;
    switch ((opcode & 0x08) >> 3) {
        case 0b0: {
            // Zero page
            arg = cpu_memory->read_byte(next_prg_byte());

            break;
        }

        case 0b1: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();
            
            arg = cpu_memory->read_byte(merge_uint8_t(upper_arg, lower_arg));

            break;
        }

        default: {
            std::cout << "Unknown 3A opcode" << std::endl;
        }
    }

    handle_registers_3A(instruction, arg);
}

void CPU::execute_3B(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode & 0xDF);

    uint8_t arg;
    switch ((opcode & 0x20) >> 5) {
        case 0b0: {
            // Absolute
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            arg = merge_uint8_t(upper_arg, lower_arg);

            break;
        }

        case 0b1: {
            // Indirect
            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            arg = cpu_memory->read_byte(merge_uint8_t(upper_arg, lower_arg));

            break;
        }

        default: {
            std::cout << "Unknown 3B opcode" << std::endl;
        }
    }

    handle_registers_3B(instruction, arg);
}

void CPU::execute_4(uint8_t opcode) {
    Instruction instruction = static_cast<Instruction>(opcode);

    switch (instruction) {
        case BCC: {
            // BCC instruction
            if (!get_status_bit(Carry)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BCS: {
            // BCS instruction
            if (get_status_bit(Carry)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BEQ: {
            // BEQ instruction
            if (get_status_bit(Zero)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BMI: {
            // BMI instruction
            if (get_status_bit(Negative)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BNE: {
            // BNE instruction
            if (!get_status_bit(Zero)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BPL: {
            // BPL instruction
            if (!get_status_bit(Negative)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BRK: {
            // BRK instruction
            set_status_bit(Break, true);
            interrupt(IRQ);
            break;
        }

        case BVC: {
            // BVC instruction
            if (!get_status_bit(Overflow)) {
                PC += next_prg_byte();
            }

            break;
        }

        case BVS: {
            // BVS instruction
            if (get_status_bit(Overflow)) {
                PC += next_prg_byte();
            }

            break;
        }

        case CLC: {
            // CLC instruction
            set_status_bit(Carry, false);

            break;
        }

        case CLD: {
            // CLD instruction
            set_status_bit(DecimalMode, false);

            break;
        }

        case CLI: {
            // CLI instruction
            set_status_bit(InterruptDisable, false);

            break;
        }

        case CLV: {
            // CLV instruction
            set_status_bit(Overflow, false);

            break;
        }

        case DEX: {
            // DEX instruction
            X -= 1;
            set_status_bit(Negative, get_bit_by_index(X, 0) == 1);
            set_status_bit(Zero, X == 0);

            break;
        }

        case DEY: {
            // DEY instruction
            Y -= 1;
            set_status_bit(Negative, get_bit_by_index(Y, 0) == 1);
            set_status_bit(Zero, Y == 0);

            break;
        }

        case INX: {
            // INX instruction
            X = (X + 1) % 256;
            set_status_bit(Negative, get_bit_by_index(X, 0) == 1);
            set_status_bit(Zero, X == 0);

            break;
        }

        case INY: {
            // INY instruction
            Y = (Y + 1) % 256;
            set_status_bit(Negative, get_bit_by_index(Y, 0) == 1);
            set_status_bit(Zero, Y == 0);

            break;
        }

        case NOP: {
            // NOP instruction
            // Do nothing
            break;
        }

        case PHA: {
            // PHA instruction
            push(A);

            break;
        }

        case PHP: {
            // PHP instruction
            push(P);

            break;
        }

        case PLA: {
            // PLA instruction
            A = pop();

            break;
        }

        case PLP: {
            // PLP instruction
            P = pop();

            break;
        }

        case RTI: {
            // RTI instruction
            // TODO Fix issue with PC being 2 bytes long
            P = pop();
            PC = merge_uint8_t(pop(), pop());

            break;
        }

        case RTS: {
            // RTS instruction
            // TODO Fix issue with PC being 2 bytes long
            PC = merge_uint8_t(pop(), pop());
            PC += 1;

            break;
        }

        case SEC: {
            // SEC instruction
            set_status_bit(Carry, true);
        
            break;
        }

        case SED: {
            // SED instruction
            set_status_bit(DecimalMode, true);

            break;
        }

        case SEI: {
            // SEI instruction
            set_status_bit(InterruptDisable, true);

            break;
        }

        case TAX: {
            // TAX instruction
            X = A;
            set_status_bit(Carry, X == 0);
            set_status_bit(Negative, get_bit_by_index(X, 0));

            break;
        }

        case TAY: {
            // TAY instruction
            Y = A;
            set_status_bit(Carry, Y == 0);
            set_status_bit(Negative, get_bit_by_index(Y, 0));

            break;
        }

        case TSX: {
            // TSX instruction
            X = SP;
            set_status_bit(Carry, X == 0);
            set_status_bit(Negative, get_bit_by_index(X, 0));

            break;
        }

        case TXA: {
            // TXA instruction
            A = X;
            set_status_bit(Carry, A == 0);
            set_status_bit(Negative, get_bit_by_index(A, 0));

            break;
        }

        case TXS: {
            // TXS instruction; no status bits are changed
            SP = X;

            break;
        }

        case TYA: {
            // TYA instruction
            A = Y;
            set_status_bit(Carry, A == 0);
            set_status_bit(Negative, get_bit_by_index(A, 0));

            break;
        }

        case JSR: {
            // JSR instruction
            // UPPER BYTE ON STACK ABOVE LOWER PART!
            uint8_t lower_PC = static_cast<uint8_t>((PC + 2) & 0x00FF);
            uint8_t upper_PC = static_cast<uint8_t>(((PC + 2) & 0xFF00) >> 8);

            push(lower_PC);
            push(upper_PC);

            uint8_t lower_arg = next_prg_byte();
            uint8_t upper_arg = next_prg_byte();

            PC = merge_uint8_t(upper_arg, lower_arg);

            break;
        }

        default: {
            std::cout << "Unknown 4 opcode" << std::endl;
        }
    }
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
            set_status_bit(Negative, get_bit_by_index(A, 0) == 1);

            break;
        }

        case LDA: {
            // LDA instruction
            A = arg;
            set_status_bit(Zero, A == 0);
            set_status_bit(Negative, get_bit_by_index(A, 0) == 1);

            break;
        }

        case ORA: {
            // ORA instruction
            A |= arg;
            set_status_bit(Zero, A == 0);
            set_status_bit(Negative, get_bit_by_index(A, 0) == 1);

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

            set_status_bit(Negative, get_bit_by_index(A, 0) == 1);
            set_status_bit(Zero, A == 0);
            
            break;
        }

        case STA: {
            // STA instruction
            cpu_memory->write_byte(arg, A);

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
                uint8_t byte = cpu_memory->read_byte(address);
                set_status_bit(Carry, get_bit_by_index(byte, 0) == 1);

                byte <<= 1;
                cpu_memory->write_byte(address, byte);

                set_status_bit(Zero, byte == 0);
                set_status_bit(Negative, get_bit_by_index(byte, 0) == 1);
            }

            break;
        }

        case LDX: {
            // LDX instruction
            X = immediate ? arg : cpu_memory->read_byte(address);
            set_status_bit(Zero, X == 0);
            set_status_bit(Negative, get_bit_by_index(X, 0) == 1);

            break;
        }

        case LDY: {
            // LDY instruction
            Y = immediate ? arg : cpu_memory->read_byte(address);
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
                uint8_t byte = cpu_memory->read_byte(address);
                set_status_bit(Carry, get_bit_by_index(byte, 7));

                byte >>= 1;
                cpu_memory->write_byte(address, byte);

                set_status_bit(Zero, byte == 0);
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
                uint8_t byte = cpu_memory->read_byte(address);
                uint8_t new_carry = get_bit_by_index(byte, 7);

                byte = (byte >> 1) + get_status_bit(Carry) * 0x80;

                cpu_memory->write_byte(address, byte);

                set_status_bit(Carry, new_carry);
                set_status_bit(Zero, byte == 0);
                set_status_bit(Negative, get_bit_by_index(byte, 0) == 1);
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
                uint8_t byte = cpu_memory->read_byte(address);
                uint8_t new_carry = get_bit_by_index(byte, 0);

                byte = (byte << 1) + get_status_bit(Carry);

                cpu_memory->write_byte(address, byte);

                set_status_bit(Carry, new_carry);
                set_status_bit(Zero, byte == 0);
                set_status_bit(Negative, get_bit_by_index(byte, 0) == 1);
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
            uint8_t value = cpu_memory->read_byte(address);
            cpu_memory->write_byte(address, --value);
            
            set_status_bit(Zero, value == 0);
            set_status_bit(Negative, get_bit_by_index(value, 0) == 1);

            break;
        }

        case INC: {
            // INC instruction
            uint8_t value = cpu_memory->read_byte(address);
            cpu_memory->write_byte(address, ++value);
            
            set_status_bit(Zero, value == 0);
            set_status_bit(Negative, get_bit_by_index(value, 0) == 1);

            break;
        }

        case STX: {
            // STX instruction
            cpu_memory->write_byte(address, X);

            break;
        }

        case STY: {
            // STY instruction
            cpu_memory->write_byte(address, Y);

            break;
        }

        default: {
            std::cout << "Unknown 2A instruction" << std::endl;
        }
    }
}

void CPU::handle_registers_2B(Instruction instruction, uint8_t arg) {
    switch (instruction) {
        case CPX: {
            // CPX instruction
            set_status_bit(Negative, get_bit_by_index(X - arg, 7));
            set_status_bit(Zero, X == arg);
            set_status_bit(Carry, X >= arg);

            break;
        }

        case CPY: {
            // CPY instruction
            set_status_bit(Negative, get_bit_by_index(Y - arg, 7));
            set_status_bit(Zero, Y == arg);
            set_status_bit(Carry, Y >= arg);

            break;
        }

        default: {
            std::cout << "Unknown 2B instruction" << std::endl;
        }
    }
}

void CPU::handle_registers_3A(Instruction instruction, uint8_t arg) {
    switch (instruction) {
        case BIT: {
            uint8_t result = arg & A;

            set_status_bit(Zero, result == 0);
            set_status_bit(Overflow, get_bit_by_index(result , 6));
            set_status_bit(Negative, get_bit_by_index(result, 7));

            break;
        }

        default: {
            std::cout << "Unknown 3A instruction" << std::endl;
        }
    }
}

void CPU::handle_registers_3B(Instruction instruction, uint8_t arg) {
    switch (instruction) {
        case JMP: {
            PC = arg;

            break;
        }

        default: {
            std::cout << "Unknown 3B instruction" << std::endl;
        }
    }
}