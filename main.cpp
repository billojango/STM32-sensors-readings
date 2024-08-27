#include "mbed.h"
#include "ST7920.h"
#include "HX711.h"

// Define SPI LCD pins
#define LCD_MOSI PB_5
#define LCD_SCK  PB_3
#define LCD_CS   PC_8

// Create SPI and LCD instances
ST7920 lcd(LCD_MOSI, LCD_SCK, LCD_CS); // MOSI, SCK, CS

// Define pins for HX711
#define DOUT_PIN PD_6
#define SCK_PIN  PE_2

// Create HX711 object
HX711 scale(DOUT_PIN, SCK_PIN);

// Define pins for LED indicators
DigitalOut redLed(PG_1);
DigitalOut greenLed(PG_6);
DigitalOut blueLed(PG_7);

int main() {
    lcd.init();
    lcd.cls();
    lcd.printf("HX711 Scale Test\r\n");
    printf("HX711 Scale Test\r\n");

    // Initialize the scale
    scale.set_scale(2280.f);    // This value is obtained by calibrating the scale with known weights
    scale.tare();               // Reset the scale to 0

    while (1) {
        float weight = scale.get_units(100);  // Get the average of 10 readings
        lcd.cls();
        lcd.printf("Weight: %.2f kg\r\n", weight);
        printf("Weight: %.2f kg\r\n", weight);
        
        // Update LED indicators based on weight
        if (weight < 10.0) {
            redLed = 1;
            greenLed = 0;
            blueLed = 0;
        } else if (weight >= 10.0 && weight <= 20.0) {
            redLed = 0;
            greenLed = 1;
            blueLed = 0;
        } else {
            redLed = 0;
            greenLed = 0;
            blueLed = 1;
        }
        
        ThisThread::sleep_for(1s);
    }
}
