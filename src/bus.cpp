#include "bus.h"

Bus::Bus() {
    reset();
}

Bus::~Bus() {}

void Bus::reset() {
    memset(cpu_memory, 0, CPU_MEMORY_SIZE);
    memset(ppu_memory, 0, PPU_MEMORY_SIZE * 4);
    memset(spr_ram, 0, SPR_RAM_SIZE);

    controllers[0] = nullptr;
    controllers[1] = nullptr;
}

void Bus::attach_cartridge(Cartridge* cartridge_ptr) {
    cartridge = cartridge_ptr;
}

uint8_t Bus::read_from_cpu(uint16_t address) {
    switch (address) {
        case 0x4016: {
            return controllers[0]->read();
        }

        case 0x4017: {
            return controllers[1]->read();
        }

        default: {
            return cpu_memory[address];
        }
    }
}

void Bus::write_to_cpu(uint16_t address, uint8_t value) {
    cpu_memory[address] = value;
}

uint8_t Bus::read_from_ppu(uint16_t address) {
    return ppu_memory[address];
}

void Bus::write_to_ppu(uint16_t address, uint8_t value) {
    ppu_memory[address] = value;
}

void Bus::attach_controller(Controller* controller) {
    if (controllers[0]) {
        controllers[1] = controller;
    } else {
        controllers[0] = controller;
    }
}

void Bus::change_button(uint8_t button_index, bool pressed) {
    // Use controller 0 as default, support for more may come later
    controllers[0]->set_button(button_index, pressed);
}

/*
void Bus::initialize_controllers(int amount) {
    for (int i = 0; i < amount; i++) {
        controllers[i] = new Controller();
    }
}
*/

