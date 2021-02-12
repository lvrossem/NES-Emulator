#ifndef CPU_H
#define CPU_H

#define MEMORY_SIZE 0x10000 // 64 KiB
#define LOWER_PRG_ROM_START 0x8000 // Start of lower PRG-ROM bank
#define UPPER_PRG_ROM_START 0xC000 // Start of upper PRG-ROM bank
#define IO_REGISTERS_START 0x2000 // Address of the first IO-register
#define STACK_SIZE 0x00FF; // Size of the stack

#include <stdint.h>
#include <assert.h>
#include <set>
#include <iostream>

#include "cpu_memory.h"

enum StatusBit { Negative = 0, Overflow, NotUsed, Break, DecimalMode, InterruptDisable, Zero, Carry };
enum Instruction {
    ADC = 0x61, AND = 0x21, CMP = 0xC1, EOR = 0x41,
    LDA = 0xA1, ORA = 0x01, SBC = 0xE1, STA = 0x81,
    ASL = 0x02, LDX = 0xA2, LDY = 0xA0, LSR = 0x42,
    ROL = 0x22, ROR = 0x62, DEC = 0xC6, INC = 0xE6,
    STX = 0x86, STY = 0x84, CPX = 0xE0, CPY = 0xC0,
    BIT = 0x24, JMP = 0x4C, BCC = 0x90, BCS = 0xB0,
    BEQ = 0xF0, BMI = 0x30, BNE = 0xD0, BPL = 0x10,
    BRK = 0x00, BVC = 0x50, BVS = 0x70, CLC = 0x18,
    CLD = 0xD8, CLI = 0x58, CLV = 0xB8, DEX = 0xCA,
    DEY = 0x88, INX = 0xE8, INY = 0xC8, NOP = 0xEA,
    PHA = 0x48, PHP = 0x08, PLA = 0x68, PLP = 0x28,
    RTI = 0x40, RTS = 0x60, SEC = 0x38, SED = 0xF8,
    SEI = 0x78, TAX = 0xAA, TAY = 0xA8, TSX = 0xBA,
    TXA = 0x8A, TXS = 0x9A, TYA = 0x98, JSR = 0x20
};

class CPU {
private:
    CPUMemory* cpu_memory; // The available memory

    uint16_t PC; // The program counter
    uint8_t SP; // The stack pointer
    uint8_t A;   // The accumulator
    uint8_t X;   // The index register X
    uint8_t Y;   // The index register Y
    /* Processor status; 8 status bits
        Bit 0: Carry
        Bit 1: Zero
        Bit 2: Interrupt disable
        Bit 3: Decimal mode (will most likely be ignored)
        Bit 4: Break
        Bit 5: Not used
        Bit 6: Overflow
        Bit 7: Negative */
    uint8_t P;

    const std::set<Instruction> group_1A {ADC, AND, CMP, EOR, LDA, ORA, SBC, STA};
    const std::set<Instruction> group_1B {ASL, LDX, LDY, LSR, ROL, ROR};
    const std::set<Instruction> group_2A {DEC, INC, STX, STY};
    const std::set<Instruction> group_2B {CPX, CPY};
    const std::set<Instruction> group_3A {BIT};
    const std::set<Instruction> group_3B {JMP};

    void execute_1A(uint8_t opcode); // Execute instruction of type 1A
    void execute_1B(uint8_t opcode); // Execute instruction of type 1B
    void execute_2A(uint8_t opcode); // Execute instruction of type 2A
    void execute_2B(uint8_t opcode); // Execute instruction of type 2B
    void execute_3A(uint8_t opcode); // Execute instruction of type 3A
    void execute_3B(uint8_t opcode); // Execute instruction of type 3B
    void execute_4(uint8_t opcode); // Execute instruction of type 4

    void handle_registers_1A(Instruction instruction, uint8_t arg); // Manipulate registers affected by 1A-instruction
    void handle_registers_1B(Instruction instruction, uint8_t arg, uint16_t address, bool acc_only, bool immediate); // Manipulate registers affected by 1B-instruction
    void handle_registers_2A(Instruction instruction, uint16_t address); // Manipulate registers affected by 2A-instruction
    void handle_registers_2B(Instruction instruction, uint8_t arg); // Manipulate registers affected by 2B-instruction
    void handle_registers_3A(Instruction instruction, uint8_t arg); // Manipulate registers affected by 3A-instruction
    void handle_registers_3B(Instruction instruction, uint8_t arg); // Manipulate registers affected by 3B-instruction

public:
    CPU();

    void set_status_bit(StatusBit bit, bool flag); // Sets a statusbit with the value of flag
    bool get_status_bit(StatusBit bit); // Gets a statusbit from P
    bool get_bit_by_index(uint8_t arg, uint8_t i); // Get the value of bit i in arg

    uint16_t merge_uint8_t(uint8_t upper, uint8_t lower); // Combine upper and lower into one uint16_t

    void initialize(); // Set all registers and entire memory to 0
    void write_to_memory(char* data, uint16_t start, uint16_t size); // Write array of bytes to memory

    void execute(); // Determine type of instruction and execute said instruction

    uint8_t next_prg_byte(); // Read the next byte from the program code
};

#endif