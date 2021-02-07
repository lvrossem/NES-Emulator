#include "nes.h"

NES::NES() {
    initialize();
}

void NES::initialize() {
    PC = 0;
    SP = 0;
    A = 0;
    X = 0;
    Y = 0;
    P = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

bool NES::load_rom(const char* rom_path) {
    std::ifstream input(rom_path);

    if (!input) {
        std::cout << "Failed to open ROM" << std::endl;
        return false;
    }

    // Read the 16-byte header into the buffer
    char header[16];
    input.read(header, 16);

    // Check if first 4 bytes are correct
    if (strncmp(header, "NES", 3) != 0 || header[3] != 0x1A) {
        std::cout << "Invalid file header" << std::endl;
        return false;
    }

    // The number of 16 KB PRG-ROM banks, where program code is stored
    uint8_t nr_prg_rom_banks = header[4];

    // The number of 8 KB PRG-ROM banks, where graphics information is stored
    uint8_t nr_chr_rom_banks = header[5];

    uint8_t control_byte_1 = header[6];
    uint8_t control_byte_2 = header[7];

    // False: horizontal mirroring; True: vertical mirroring
    bool mirror_type = (control_byte_1 & 0x80) >> 7;

    // Indicates presence of battery-backed RAM
    bool has_battery_backed_ram = (control_byte_1 & 0x40) >> 6;

    // Indicates the presence of a 512-byte trainer after the header
    bool has_trainer = (control_byte_1 & 0x20) >> 5;

    // Indicates that four-screen mirroring should be used
    bool four_screen_mirroring = (control_byte_1 & 0x10) >> 4;

    if (control_byte_2 & 0xF0 == 0) {
        std::cout << "Invalid file header" << std::endl;
        return false;
    }

    // Compose the mapper number from the lowest 4 bits of each control byte
    uint8_t mapper_number = ((control_byte_2 & 0x0F) << 4) | (control_byte_1 & 0x0F);
}