#ifndef CPU_H
#define CPU_H

#define MEMORY_SIZE 0x10000 // 64 KiB
#define LOWER_PRG_ROM_START 0x8000 // Start of lower PRG-ROM bank
#define UPPER_PRG_ROM_START 0xC000 // Start of upper PRG-ROM bank
#define IO_REGISTERS_START 0x2000 // Address of the first IO-register

#include <stdint.h>
#include <assert.h>
#include <set>
#include <iostream>

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
    uint8_t cpu_memory[MEMORY_SIZE]; // The available memory

    uint16_t PC; // The program counter
    uint16_t SP; // The stack pointer
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

public:
    CPU();

    void set_status_bit(StatusBit bit, bool flag); // Sets a statusbit with the value of flag

    void initialize(); // Set all registers and entire memory to 0
    void write_to_memory(char* data, uint16_t start, uint16_t size); // Write array of bytes to memory

    void execute(); // Determine type of instruction and execute said instruction
    void execute_1A(uint8_t opcode); // Execute instruction of type 1A

    void handle_registers_1A(Instruction instruction, uint8_t arg); // Manipulate registers affected by 1A-instruction

    uint8_t next_prg_byte(); // Read the next byte from the program code
};

#endif