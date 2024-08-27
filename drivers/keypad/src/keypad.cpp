#include "keypad.h"

Keypad::Keypad(PinName k1, PinName k2, PinName k3, PinName k4, PinName k5, PinName k6, PinName k7, PinName k8)
    : _key1(k1), _key2(k2), _key3(k3), _key4(k4), _key5(k5), _key6(k6), _key7(k7), _key8(k8) {
    _key1.mode(PullUp);
    _key2.mode(PullUp);
    _key3.mode(PullUp);
    _key4.mode(PullUp);
    _key5.mode(PullUp);
    _key6.mode(PullUp);
    _key7.mode(PullUp);
    _key8.mode(PullUp);
}

char Keypad::getKey() {
    if (!_key1) return '1';
    if (!_key2) return '2';
    if (!_key3) return '3';
    if (!_key4) return '4';
    if (!_key5) return '5';
    if (!_key6) return '6';
    if (!_key7) return '7';
    if (!_key8) return '8';
    return '\0';
}
