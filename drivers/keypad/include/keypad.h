#ifndef KEYPAD_H
#define KEYPAD_H

#pragma once
#include "mbed.h"

class Keypad {
public:
    Keypad(PinName k1, PinName k2, PinName k3, PinName k4, PinName k5, PinName k6, PinName k7, PinName k8);
    char getKey();

private:
    DigitalIn _key1;
    DigitalIn _key2;
    DigitalIn _key3;
    DigitalIn _key4;
    DigitalIn _key5;
    DigitalIn _key6;
    DigitalIn _key7;
    DigitalIn _key8;
};

#endif // KEYPAD_H
