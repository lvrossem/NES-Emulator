#include <iostream>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv) {
    char nes[4] = "NES";
    char bits[3];
    bits[0] = 0x4E;
    bits[1] = 0x46;
    bits[2] = 0x53;
    std::cout << "Hello NES" << std::endl << (0x4E == 'A') << std::endl;
}