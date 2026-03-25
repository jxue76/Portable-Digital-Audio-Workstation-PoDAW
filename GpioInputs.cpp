#include "GpioInputs.hpp"
#include <gpiod.h>
#include <stdexcept>

GpioInputs::GpioInputs() {
    // Initialize GPIO pins
    try {
        gpiod::chip chip("/dev/gpiochip0");
        pinA = chip.get_line(17); // GPIO17
        pinB = chip.get_line(27); // GPIO27
        pinX = chip.get_line(22); // GPIO22
        pinUp = chip.get_line(9); // GPIO9
        pinDown = chip.get_line(11); // GPIO11
        pinLeft = chip.get_line(8); // GPIO8
        pinRight = chip.get_line(7); // GPIO7
        pinDialClk = chip.get_line(23); // GPIO23
        pinDialDt = chip.get_line(24); // GPIO24

        pinA.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinB.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinX.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinUp.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinDown.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinLeft.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinRight.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinDialClk.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinDialDt.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize GPIO inputs: " + std::string(e.what()));
    }
}
bool GpioInputs::isUpPressed() const {
    return pinUp.get_value() == 0; // Active low
}
bool GpioInputs::isDownPressed() const {
    return pinDown.get_value() == 0; // Active low
}
bool GpioInputs::isLeftPressed() const {
    return pinLeft.get_value() == 0; // Active low
}
bool GpioInputs::isRightPressed() const {
    return pinRight.get_value() == 0; // Active low
}
bool GpioInputs::isAPressed() const {
    return pinA.get_value() == 0; // Active low
}
bool GpioInputs::isBPressed() const {
    return pinB.get_value() == 0; // Active low
}
bool GpioInputs::isXPressed() const {
    return pinX.get_value() == 0; // Active low
}
Dial GpioInputs::getDialPosition() const {
    bool clk = pinDialClk.get_value() == 0; // Active low
    bool dt = pinDialDt.get_value() == 0; // Active low
    if (clk && !dt) {
        return Dial::LEFT;
    } else if (!clk && dt) {
        return Dial::RIGHT;
    } else {
        return Dial::NEUTRAL;
    }
}
