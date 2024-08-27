//
// Created by wells on 9/22/23.
//

#ifndef TEST_ST7920_TEXTLCDINTERFACE_H
#define TEST_ST7920_TEXTLCDINTERFACE_H


#include <cstdint>
#include "mbed.h"

typedef unsigned char byte;

/**
 * Interface class TextLCDInterface
 */
class TextLCDInterface: public NonCopyable<TextLCDInterface> {

public:

    /*Note:
     * Also implement static get_default_interface() for each class that inherits from this
     * */
    /**
     * Init controller
     */
    virtual void init() = 0;

    /**
     * Write a character to the LCD
     * @param c
     * */
    virtual int putc(byte c) = 0;

    /**
     * Write a byte array  to LCD
     * @param format
     * @param ...
     * @return
     */
    virtual int printf(const char* format, ...) = 0;

    /**
     * Set cursor to position on screen
     * @param column
     * @param row
     */
    virtual void locate(int column, int row) = 0;

    /**
     * Clear the screen and locate to 0,0
     */
    virtual void cls() = 0;

    /**
     * Get no of rows for LCD type
     * @return
     */
    virtual int rows() const = 0;

    /**
     *  Get no of columns for lcd type
     * @return
     */
    virtual int columns() const = 0;


    virtual ~TextLCDInterface() = default;
protected:
    /**
     * Get current address
     * @param column
     * @param row
     * @return
     */
    virtual int address(int column, int row) = 0;

    /**
     * Write bytes to controller
     * @param value
     */
    virtual void _write_byte(byte value) = 0;

    /**
     * Write command to controller (usually rs=0 then write_byte(cmd))
     * @param command
     */
    virtual void _write_command(byte command) = 0;

    /**
     * write data to controller (usually rs=1 then write_byte(data))
     * @param data
     */
    virtual void _write_data(byte data) = 0;

    /**
     * Set enable pin
     * @param value
     */
    virtual void _set_enable(bool value) = 0;

    /**
     * _set reset pin
     * @param value
     */
    virtual void _set_rs(bool value) = 0;
};

#endif //TEST_ST7920_TEXTLCDINTERFACE_H
