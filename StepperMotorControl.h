#ifndef STEPPER_MOTOR_CONTROL_H
#define STEPPER_MOTOR_CONTROL_H

#include "mbed.h"

class StepperMotorControl {
public:
    StepperMotorControl(DigitalOut& enablePin, DigitalOut& directionPin, DigitalOut& pulsePin, DigitalOut& buzzerPin);
    
    void moveSteps(int steps);
    void playMelody();

private:
    DigitalOut& enablePin;
    DigitalOut& directionPin;
    DigitalOut& pulsePin;
    DigitalOut& buzzerPin;

    void step();
    void playTone(float frequency, int duration_ms);
};

#endif // STEPPER_MOTOR_CONTROL_H
