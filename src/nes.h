#ifndef NES_H
#define NES_H

#include <stdint.h>
#include <string.h>
#include <fstream>
#include <iostream>

#include "bus.h"
#include "cpu.h"
#include "cartridge.h"

class NES {
private:
    Bus* bus;
    Controller* controller;
    Cartridge* cartridge;
public:
    NES();
    ~NES();

    bool load_rom(const char* rom_path); // Loads the ROM into memory
    void parse_header(std::ifstream& input); // Parse the iNES-header
    void execute_next_instruction();
    void change_button(uint8_t button_index, bool pressed);
};

#endif