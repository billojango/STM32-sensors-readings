#pragma once
#include "mbed.h"

#ifndef LOADCELLS_H
#define LOADCELLS_H

// increase the sample count in moving average
#if defined(SAMPLES)
  #undef SAMPLES
  #define SAMPLES 128
#endif

class LoadCells {
public:
    LoadCells(PinName dout, PinName sck);
    void init();
    void tare(uint8_t times = 10);
    void setCalibrationFactor(float scale);
    float getValue();

private:
    float read();

    DigitalIn _data;
    DigitalOut _sck;
    long _offset = 0;
    float _scale = 1.f;
};

#endif // LOADCELLS_H
