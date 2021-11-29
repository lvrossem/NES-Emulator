#include "nes.h"

NES::NES() {
    std::cout << "INIT..." << std::endl;
    cpu = new CPU();
    ppu = new PPU();
    bus = new Bus();

    cpu->set_bus(bus);
    ppu->set_bus(bus);

    controller = new Controller();

    bus->attach_controller(controller);
}

NES::~NES() {
    delete cpu;
    delete ppu;
    delete bus;
    delete controller;
}

iNES_header NES::parse_header(std::ifstream& input) {
    iNES_header result;
    result.is_valid_header = false;

    if (!input) {
        std::cout << "Failed to open ROM" << std::endl;
        return result;
    }

    // Read the 16-byte header into the buffer
    char header[16];
    input.read(header, 16);

    // Check if first 4 bytes are correct
    if (strncmp(header, "NES", 3) != 0 || header[3] != 0x1A) {
        std::cout << "Invalid file header" << std::endl;
        return result;
    }

    result.nr_prg_rom_banks = header[4];
    result.nr_chr_rom_banks = header[5];

    uint8_t control_byte_1 = header[6];
    uint8_t control_byte_2 = header[7];

    result.mirror_type = (control_byte_1 & 0x80) >> 7;

    result.has_battery_backed_ram = (control_byte_1 & 0x40) >> 6;

    result.has_trainer = (control_byte_1 & 0x20) >> 5;

    result.four_screen_mirroring = (control_byte_1 & 0x10) >> 4;

    if (control_byte_2 & 0xF0 == 0) {
        std::cout << "Invalid file header" << std::endl;
        return result;
    }

    // Compose the mapper number from the lowest 4 bits of each control byte
    result.mapper_number = ((control_byte_2 & 0x0F) << 4) | (control_byte_1 & 0x0F);

    result.nr_ram_banks = header[8];

    // If the header says 0, it means 1
    if (result.nr_ram_banks == 0) {
        result.nr_ram_banks = 1;
    }

    // Bytes 9 to 15 should be 0
    for (int i = 9; i < 16; i++) {
        if (header[i] != 0) {
            std::cout << "Invalid file header" << std::endl;
            return result;
        }
    }

    result.is_valid_header = true;
    return result;
}

bool NES::load_rom(const char* rom_path) {
    std::ifstream input(rom_path, std::ios::binary);
    iNES_header rom_header = parse_header(input);

    if (!rom_header.is_valid_header) {
        // Don't continue if header is invalid
        return false;
    }

    if (rom_header.has_trainer) {
        // Skip next 512 bytes if trainer is present
        input.ignore(512);
    }

    // Array to store all PRG-ROM banks
    char **prg_rom_banks = new char*[rom_header.nr_prg_rom_banks];

    // Storage for CHR-ROM banks, could be 0 so we check before initializing
    char **chr_rom_banks;

    if (rom_header.nr_chr_rom_banks > 0) {
        chr_rom_banks = new char*[rom_header.nr_chr_rom_banks];
    }

    // Read all PRG-ROM banks
    for (int i = 0; i < rom_header.nr_prg_rom_banks; i++) {
        prg_rom_banks[i] = new char[0x4000];
        input.read(prg_rom_banks[i], 0x4000);
    }

    // Read all CHR-ROM banks
    for (int i = 0; i < rom_header.nr_chr_rom_banks; i++) {
        chr_rom_banks[i] = new char[0x2000];
        input.read(chr_rom_banks[i], 0x2000);
    }

    if (rom_header.nr_prg_rom_banks == 1) {
        // With only 1 PRG-ROM bank, write to both banks in memory
        cpu->write_data_to_memory(prg_rom_banks[0], LOWER_PRG_ROM_START, 0x4000);
        cpu->write_data_to_memory(prg_rom_banks[0], UPPER_PRG_ROM_START, 0x4000);
    } else if (rom_header.nr_prg_rom_banks == 2) {
        // With 2 PRG-ROM banks, write one to each bank in memory
        cpu->write_data_to_memory(prg_rom_banks[0], LOWER_PRG_ROM_START, 0x4000);
        cpu->write_data_to_memory(prg_rom_banks[1], UPPER_PRG_ROM_START, 0x4000);
    }

    if (rom_header.mapper_number == 2) {
        // UNROM Switch is used; load first and last PRG-ROM bank into memory
        uint8_t last_bank_index = rom_header.nr_prg_rom_banks - 1;
        cpu->write_data_to_memory(prg_rom_banks[0], LOWER_PRG_ROM_START, 0x4000);
        cpu->write_data_to_memory(prg_rom_banks[last_bank_index], UPPER_PRG_ROM_START, 0x4000);
    }
    
    return true;
}

void NES::execute_next_instruction() {
    cpu->execute_next_instruction();
}

void NES::change_button(uint8_t button_index, bool pressed) {
    std::cout << "HERE1" << std::endl;
    bus->change_button(button_index, pressed);
}