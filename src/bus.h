#ifndef BUS_H
#define BUS_H

#define CPU_MEMORY_SIZE 0x10000 // 64 KiB
#define PPU_MEMORY_SIZE 0x4000 // 16 KiB
#define SPR_RAM_SIZE 0x100 // 256 bytes
#define CPU_STACK_BOTTOM 0x0100
#define CPU_STACK_SIZE 0xFF

#include <cstring>

#include "controller.h"
#include "cpu.h"
#include "cartridge.h"
#include "ppu.h"

class CPU;
class PPU;
class Bus {
private:
    uint8_t cpu_memory[CPU_MEMORY_SIZE];
    uint8_t ppu_memory[PPU_MEMORY_SIZE * 4];
    uint8_t spr_ram[SPR_RAM_SIZE];

    Controller* controllers[2];
    Cartridge* cartridge;
    CPU* cpu;
    PPU* ppu;
public:
    Bus();
    ~Bus();

    void reset();

    void attach_cartridge(Cartridge* cartridge_ptr);

    uint8_t read_from_cpu(uint16_t address);
    void write_to_memory(uint16_t address, uint8_t value);
    void write_array_to_memory(uint8_t* data, uint16_t start, uint16_t size);
    void execute_next_instruction();

    uint8_t read_from_ppu(uint16_t address);
    void write_to_ppu(uint16_t address, uint8_t value);
    
    //void initialize_controllers(int amount);
    void attach_controller(Controller* controller);
    void change_button(uint8_t button_index, bool pressed);
};

#endif