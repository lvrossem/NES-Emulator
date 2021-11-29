#include "controller.h"

Controller::Controller() {
    memset(buttons, 0, 8 * sizeof(bool));
    strobe = 0;
    index = 0;
}

Controller::~Controller() {}

void Controller::check_strobe() {
    if (strobe & 1) {
        index = 0;
    }
}

void Controller::write(uint8_t value) {
    strobe = value;
    check_strobe();
}

void Controller::set_button(uint8_t button_index, bool pressed) {
    if (button_index < 8) {
        buttons[button_index] = pressed;
    } else {
        std::cout << "Invalid button was selected" << std::endl;
    }
}

uint8_t Controller::read() {
    uint8_t result;

    if (index < 8) {
        result = buttons[index];
    }

    index++;
    check_strobe();

    return result;
}