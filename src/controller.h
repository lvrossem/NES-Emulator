#ifndef CONTROLLER_H
#define CONTROLLER_H

#define NR_OF_BUTTONS 8

#include <cstdint>
#include <cstring>
#include <iostream>

class Controller {
private:
    bool buttons[8];
    uint8_t strobe;
    uint8_t index;
public:
    Controller();
    ~Controller();
    
    void check_strobe();
    void write(uint8_t value);
    void set_button(uint8_t button_index, bool pressed);

    uint8_t read();
};

#endif