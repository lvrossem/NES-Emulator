#include <iostream>
#include <stdint.h>
#include <string.h>

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
    std::cout << "Hello NES" << std::endl << (0x80 * true) << std::endl;
}