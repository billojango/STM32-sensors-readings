#include "HX711.h"

HX711::HX711(PinName dout, PinName pd_sck, int gain) :
    _pd_sck(pd_sck), _dout(dout), _gain(gain), _offset(0), _scale(1.0f)
{
    _pd_sck = 0;
    set_gain(gain);
    printf("[HX711] HX711 initialized\r\n");
}

bool HX711::is_ready() {
    bool ready = _dout == 0;
    printf("[HX711] HX711 is %s\r\n", ready ? "ready" : "not ready");
    return ready;
}

void HX711::wait_ready(chrono::milliseconds delay_ms) {
    printf("[HX711] Waiting for HX711 to be ready\r\n");
    while (!is_ready()) {
        ThisThread::sleep_for(delay_ms);
    }
    printf("[HX711] HX711 is now ready\r\n");
}

bool HX711::wait_ready_retry(int retries, chrono::milliseconds delay_ms) {
    printf("[HX711] Retrying wait for HX711 to be ready\r\n");
    int count = 0;
    while (count < retries) {
        if (is_ready()) {
            printf("[HX711] HX711 is ready after retry\r\n");
            return true;
        }
        ThisThread::sleep_for(delay_ms);
        count++;
    }
    printf("[HX711] HX711 not ready after retries\r\n");
    return false;
}

bool HX711::wait_ready_timeout(chrono::milliseconds timeout, chrono::milliseconds delay_ms) {
    printf("[HX711] Waiting for HX711 with timeout\r\n");
    Timer timer;
    timer.start();
    while (timer.elapsed_time() < timeout) {
        if (is_ready()) {
            printf("[HX711] HX711 ready within timeout\r\n");
            return true;
        }
        ThisThread::sleep_for(delay_ms);
    }
    printf("[HX711] HX711 not ready within timeout\r\n");
    return false;
}

void HX711::set_gain(int gain) {
    switch (gain) {
        case 128:
            _gain = 1;
            printf("[HX711] Gain set to 128\r\n");
            break;
        case 64:
            _gain = 3;
            printf("[HX711] Gain set to 64\r\n");
            break;
        case 32:
            _gain = 2;
            printf("[HX711] Gain set to 32\r\n");
            break;
        default:
            _gain = 1;
            printf("[HX711] Invalid gain, defaulting to 128\r\n");
    }

    read();
}

int32_t HX711::read() {
    // Wait for the chip to become ready.
    wait_ready();

    int32_t value = 0;
    uint8_t data[3] = { 0 };

    // Pulse the clock pin 24 times to read the data.
    for (int i = 0; i < 24; i++) {
        _pd_sck = 1;
        data[2] |= _dout.read() << (7 - (i % 8));
        data[1] |= _dout.read() << (7 - ((i + 8) % 8));
        data[0] |= _dout.read() << (7 - ((i + 16) % 8));
        _pd_sck = 0;
    }

    // Set the gain for the next reading.
    for (int i = 0; i < _gain; i++) {
        _pd_sck = 1;
        _pd_sck = 0;
    }

    // Combine the three bytes into a 24-bit signed integer.
    value = ((data[2] & 0x80) ? 0xFF000000 : 0) | (data[2] << 16) | (data[1] << 8) | data[0];

    return value;
}

int32_t HX711::read_average(int times) {
    int32_t sum = 0;
    for (int i = 0; i < times; i++) {
        sum += read();
    }
    return sum / times;
}

double HX711::get_value(int times) {
    return read_average(times) - _offset;
}

float HX711::get_units(int times) {
    return get_value(times) / _scale;
}

void HX711::tare(int times) {
    int32_t sum = read_average(times);
    set_offset(sum);
}

void HX711::set_scale(float scale) {
    _scale = scale;
}

float HX711::get_scale() {
    return _scale;
}

void HX711::set_offset(int32_t offset) {
    _offset = offset;
}

int32_t HX711::get_offset() {
    return _offset;
}

void HX711::power_down() {
    printf("[HX711] Powering down\r\n");
    _pd_sck = 0;
    _pd_sck = 1;
}

void HX711::power_up() {
    printf("[HX711] Powering up\r\n");
    _pd_sck = 0;
}
