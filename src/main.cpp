#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>


#include "nes.h"

int main(int argc, char **argv) {
    char nes[4] = "NES";
    char bits[4];
    bits[0] = 0x4E;
    bits[1] = 0x45;
    bits[2] = 0x53;
    bits[3] = 0x1A;
    bool test = 1;
    bool test2 = true;
    uint8_t z = 0xFA;
    std::cout << "Hello NES" << std::endl << ("a" > "A") << std::endl;

    std::string line;
    std::ifstream infile("/home/lvrossem/Documents/emulators/nes-emulator/src/nes.cpp");

    for (int i = 0; i < 10; i++) {
        infile >> line;
        std::cout << line.length() << std::endl;
        std::cout << "First: " << line[0] << std::endl;
        std::cout << line << std::endl;
    }

    infile.close();
}