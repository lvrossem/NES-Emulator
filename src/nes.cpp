#include "nes.h"

NES::NES() {
    std::cout << "INIT..." << std::endl;
    bus = new Bus();

    controller = new Controller();
    bus->attach_controller(controller);
}

NES::~NES() {
    delete bus;
    delete controller;
    delete cartridge;
}

void NES::parse_header(std::ifstream& input) {
    cartridge = new Cartridge(input);
}

bool NES::load_rom(const char* rom_path) {
    std::ifstream input(rom_path, std::ios::binary);
    parse_header(input);

    if (cartridge->has_trainer()) {
        // Skip next 512 bytes if trainer is present
        input.ignore(512);
    }

    // Array to store all PRG-ROM banks
    uint8_t **prg_rom_banks = new uint8_t*[cartridge->get_nr_prg_rom_banks()];

    // Storage for CHR-ROM banks, could be 0 so we check before initializing
    char **chr_rom_banks;

    if (cartridge->get_nr_chr_rom_banks() > 0) {
        chr_rom_banks = new char*[cartridge->get_nr_chr_rom_banks()];
    }

    // Read all PRG-ROM banks
    for (int i = 0; i < cartridge->get_nr_prg_rom_banks(); i++) {
        prg_rom_banks[i] = (uint8_t*) new char[0x4000];
        input.read((char*) prg_rom_banks[i], 0x4000);
    }

    // Read all CHR-ROM banks
    for (int i = 0; i < cartridge->get_nr_chr_rom_banks(); i++) {
        chr_rom_banks[i] = new char[0x2000];
        input.read(chr_rom_banks[i], 0x2000);
    }

    if (cartridge->get_nr_prg_rom_banks() == 1) {
        // With only 1 PRG-ROM bank, write to both banks in memory
        bus->write_array_to_memory((uint8_t*) prg_rom_banks[0], LOWER_PRG_ROM_START, 0x4000);
        bus->write_array_to_memory((uint8_t*) prg_rom_banks[0], UPPER_PRG_ROM_START, 0x4000);
    } else if (cartridge->get_nr_prg_rom_banks() == 2) {
        // With 2 PRG-ROM banks, write one to each bank in memory
        bus->write_array_to_memory((uint8_t*) prg_rom_banks[0], LOWER_PRG_ROM_START, 0x4000);
        bus->write_array_to_memory((uint8_t*) prg_rom_banks[1], UPPER_PRG_ROM_START, 0x4000);
    }

    if (cartridge->get_mapper_number() == 2) {
        // UNROM Switch is used; load first and last PRG-ROM bank into memory
        uint8_t last_bank_index = cartridge->get_nr_prg_rom_banks() - 1;
        bus->write_array_to_memory((uint8_t*) prg_rom_banks[0], LOWER_PRG_ROM_START, 0x4000);
        bus->write_array_to_memory((uint8_t*) prg_rom_banks[last_bank_index], UPPER_PRG_ROM_START, 0x4000);
    }
    
    std::cout << cartridge->is_valid_header() << ", " << (int) cartridge->get_nr_prg_rom_banks() << std::endl;
    bus->attach_cartridge(cartridge);

    return true;
}

void NES::execute_next_instruction() {
    bus->execute_next_instruction();
}

void NES::change_button(uint8_t button_index, bool pressed) {
    std::cout << "HERE1" << std::endl;
    bus->change_button(button_index, pressed);
}