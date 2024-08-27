#ifndef HX711_H
#define HX711_H

#include "mbed.h"

class HX711 {
private:
    DigitalOut _pd_sck;
    DigitalIn _dout;
    int _gain;
    long _offset;
    float _scale;

public:
    HX711(PinName dout, PinName pd_sck, int gain = 128);
    
    bool is_ready();
    void wait_ready(chrono::milliseconds delay_ms = 0ms);
    bool wait_ready_retry(int retries = 3, chrono::milliseconds delay_ms = 0ms);
    bool wait_ready_timeout(chrono::milliseconds timeout = 1000ms, chrono::milliseconds delay_ms = 0ms);
    
    void set_gain(int gain = 128);
    int32_t read();
    int32_t read_average(int times = 10);
    double get_value(int times = 1);
    float get_units(int times = 1);
    
    void tare(int times = 10);
    void set_scale(float scale = 1.0f);
    float get_scale();
    void set_offset(int32_t offset = 0);
    int32_t get_offset();
    
    void power_down();
    void power_up();

private:
    void log(const char* message);
};

#endif // HX711_H
