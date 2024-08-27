#include "HX711.h"

LoadCells::LoadCells(PinName dout, PinName sck) : _data(dout), _sck(sck) {
    _sck = 0;
    wait_us(100);
}

void LoadCells::init() {
    tare();
}

void LoadCells::tare(uint8_t times) {
    double sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += read();
    }
    _offset = sum / times;
}

float LoadCells::read() {
    long value = 0;
    uint8_t data[3] = { 0 };
    uint8_t filler = 0x00;

    // Wait for the chip to become ready
    while (_data == 1);

    // Clock in the data
    for (int i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            _sck = 1;
            wait_us(1);
            data[2 - i] |= (_data & 0x1) << (7 - j);
            _sck = 0;
            wait_us(1);
        }
    }

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if (data[2] & 0x80) {
        filler = 0xFF;
    } else {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = (static_cast<unsigned long>(filler) << 24
            | static_cast<unsigned long>(data[2]) << 16
            | static_cast<unsigned long>(data[1]) << 8
            | static_cast<unsigned long>(data[0]));

    return static_cast<float>(value) - _offset; // Subtract tare offset
}

void LoadCells::setCalibrationFactor(float scale) {
    _scale = scale;
}

float LoadCells::getValue() {
    return read() / _scale; // Return weight in grams
}
