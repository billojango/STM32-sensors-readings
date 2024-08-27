#include "mbed.h"
#include "ST7920.h"
#include "HX711.h"

// Define SPI LCD pins
#define LCD_MOSI PB_5
#define LCD_MISO PB_14
#define LCD_SCK  PB_3
#define LCD_CS   PC_8

// Create SPI and LCD instances
ST7920 lcd(LCD_MOSI, LCD_SCK, LCD_CS ); // MOSI, MISO, SCK

int main() {
    // Define pin names based on your setup
    lcd.init();
    lcd.cls();

    LoadCells loadcell(PD_6, PE_2);  // DATA, SCK
    printf("HX711 Load Cell Reader\n");
    printf("----------------------\n");

    // Initialize LoadCells
    printf("Initializing LoadCells...\n");
    loadcell.init();
    printf("LoadCells initialized.\n");

    // Set scale and tare
    float scale_factor = 420.0983;  // Example scale factor, adjust for your calibration
    printf("Setting scale factor...\n");
    loadcell.setCalibrationFactor(scale_factor);
    printf("Scale factor set to: %.4f\n", scale_factor);

    printf("Beginning weight measurements...\n");
    printf("--------------------------------\n");

    while (true) {
        // Get the latest value and print the raw reading
        float grams = loadcell.getValue();
        lcd.cls();
        lcd.printf("Raw reading: %.4f g\n", grams);  // Print raw reading rounded to 1 decimal place
        printf("Raw reading: %.4f g\n", grams);  // Print raw reading rounded to 1 decimal place
        ThisThread::sleep_for(1000ms);  // Sleep for 1 second
    }
}
