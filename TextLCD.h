
#ifndef MBED_TEXT_LCD_H
#define MBED_TEXT_LCD_H

#include "mbed.h"



class TextLCD {

public:

    /** LCD panel format */
    enum class LCDType {
        LCD16x2,  /**< 16x2 LCD panel (default) */
        LCD20x2,  /**< 20x2 LCD panel */
        LCD20x4   /**< 20x4 LCD panel */
    };

    /* LCD Bus control */
    enum class LCDBus {
        PinBus,  /*<  Regular mbed pins */
        I2CBus,  /*<  I2C PCF8574 Portexpander */
    };

    /** Create a TextLCD Pin Bus interface
    *
    * @param rs    Instruction/data control line
    * @param e     Enable line (clock)
    * @param d4-d7 Data lines for using as a 4-bit interface
    * @param type  Sets the panel size/addressing mode (default = LCD20x4)
    */
    TextLCD(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7,
            LCDType type = LCDType::LCD20x4);


    /** Create a TextLCD Pin Bus interface
     * @param sda i2c data line
     * @param scl i2c clock line
     * @param i2c_addr  i2c addr, use 8 bit addressing
     * @param type  Sets the panel size/addressing mode (default = LCD20x4)
    */
    TextLCD(PinName sda, PinName scl, uint8_t i2c_addr, LCDType type = LCDType::LCD20x4);

    ~TextLCD();

    /** Write a character to the LCD */
    int putc(int c);

    /** write a byte array  to LCD*/
    int printf(const char* format, ...);

    /** fast print for i2c */
    int fast_i2c_print(const char* data);

    /** Locate to a screen column and row */
    void locate(int column, int row);

    /** Clear the screen and locate to 0,0 */
    void cls();

    /** get no of rows for LCD type */
    int rows() const;

    /** get no of columns for lcd type */
    int columns() const;

protected:

    /*  Init the LCD Controller(s)
    *  Clear display
    */
    void _init();

    int address(int column, int row);

    void _write_byte(int value);

    void _write_command(int command);

    /** Write a byte using the 4-bit interface */
    void _write_data(int data);

    /** Set E pin */
    void _set_enable(bool value);

    /** Set RS pin */
    void _set_rs(bool value);

    /** Place the 4bit data on the databus */
    void _set_Data(int value);

    /** writes i2c byte to the bus */
    int _i2c_write();

    //data bus
    DigitalOut _rs, _e;
    BusOut _d;

    //i2c bus
    I2C* _i2c = nullptr;
    uint8_t _slaveAddress;
    char _pins = 0;

    LCDType _type;
    int _column;
    int _row;
    LCDBus _busType;

    //out buffer
    char* _w_buffer;

    // instruction buffer for fprintf
    char* _ins_buffer;

};

#endif //MBED_TEXT_LCD_H
