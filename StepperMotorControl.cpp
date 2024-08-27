// #include "StepperMotorControl.h"

// StepperMotorControl::StepperMotorControl(DigitalOut& enablePin, DigitalOut& directionPin, DigitalOut& pulsePin, DigitalOut& buzzerPin)
//     : enablePin(enablePin), directionPin(directionPin), pulsePin(pulsePin), buzzerPin(buzzerPin) {
//     // Initialize your pins as needed
//     enablePin = 1; // Disable motor initially
//     directionPin = 0; // Set initial direction
// }

// void StepperMotorControl::moveSteps(int steps) {
//     // Move the stepper motor in one direction for specified steps
//     for (int i = 0; i < steps; i++) {
//         step();
//     }
// }

// void StepperMotorControl::step() {
//     pulsePin = 1;
//     wait_us(500);  // Reduced delay for faster speed
//     pulsePin = 0;
//     wait_us(500);  // Reduced delay for faster speed
// }

// void StepperMotorControl::playTone(float frequency, int duration_ms) {
//     float period = 1.0f / frequency;
//     int cycles = duration_ms * frequency / 1000;
    
//     for (int i = 0; i < cycles; i++) {
//         buzzerPin = 1;
//         wait_us(period * 500000); // Half-period in microseconds
//         buzzerPin = 0;
//         wait_us(period * 500000); // Half-period in microseconds
//     }
// }

// void StepperMotorControl::playMelody() {
//     // Define melody to simulate "dispense completed open the door"
//     float melody[][2] = {
//         {523.25, 200}, // "dis-"
//         {493.88, 200}, // "pense"
//         {392.00, 300}, // "com-"
//         {349.23, 300}, // "plet-"
//         {329.63, 200}, // "-ed"
//         {261.63, 500}, // Pause
//         {392.00, 200}, // "o-"
//         {349.23, 200}, // "pen"
//         {329.63, 200}, // "the"
//         {293.66, 200}, // "door"
//         {261.63, 500}, // Pause
//     };

//     for (int i = 0; i < static_cast<int>(sizeof(melody) / sizeof(melody[0])); i++) {
//         playTone(melody[i][0], melody[i][1]);
//         wait_us(10000); // Short delay between notes
//     }
// }
