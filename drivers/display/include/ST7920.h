#ifndef _ST7920_H
#define _ST7920_H

#include "mbed.h"
#include  "TextLCDInterface.h"

#define LCD_CLS         0x01
#define LCD_HOME        0x02
#define LCD_ADDRINC     0x06
#define LCD_DISPLAYON   0x0C
#define LCD_DISPLAYOFF  0x08
#define LCD_CURSORON    0x0E
#define LCD_CURSORBLINK 0x0F
#define LCD_BASIC       0x30
#define LCD_EXTEND      0x34
#define LCD_GFXMODE     0x36
#define LCD_TXTMODE     0x34
#define LCD_STANDBY     0x01
#define LCD_SCROLL      0x03
#define LCD_SCROLLADDR  0x40
#define LCD_ADDR        0x80
#define LCD_LINE0       0x80
#define LCD_LINE1       0x90
#define LCD_LINE2       0x88
#define LCD_LINE3       0x98

#define SPI_SPEED 1000000
#define SPI_BITS  8
#define SPI_MODE 3
#ifndef SCR_WD
#define SCR_WD  128
#define SCR_HT  64
#endif

#define ALIGN_LEFT    0
#define ALIGN_RIGHT  (-1)
#define ALIGN_CENTER (-2)

/**
 * Device font structure
 */
typedef struct {
    const uint8_t* font;  // font array
    int8_t xSize;//  font size in x
    uint8_t ySize;// font size in y
    uint8_t firstCh;
    uint8_t lastCh;
    uint8_t minCharWd; // minimum character width
    uint8_t minDigitWd;// minimum digit width
} device_font_t;

typedef uint8_t byte;

#define BEGIN_COMMAND _cs.write(1);
//#define BEGIN_COMMAND _spi.select();
#define END_COMMAND _cs.write(0);
//#define END_COMMAND _spi.deselect();

/**
 * ST7920 Driver class for operating ST7920 128x94 LCD
 */
class ST7920 : public NonCopyable<ST7920>, public TextLCDInterface {
public:
    /**
    * ST7920 Constructor
    * @param mosi  - MOSI pin for SPI connection
    * @param sclk - Serial Clock pin
    * @param cs - Chip Select pin
    * @param rs - Reset line
    */
    ST7920(PinName mosi, PinName sclk, PinName cs, PinName rs = NC);
    ~ST7920() override = default;
    int putc(byte c) override
    {
        (void)c;
        return -1;
    }
    int printf(const char* format, ...) override;
    void locate(int column, int row) override
    {
        _x = column;
        _y = row;
        gotoXY(_x, _y);
    }
    int rows() const override
    {
        return SCR_WD;
    }
    int columns() const override
    {
        return SCR_HT;
    }
    /**
     * Initialize screen members
     */
    void init() override;

    /**
     * Select Buffer
     * @param b - The index of the buffer to write to screen
     */
    void switchBuf(byte b);

    /**
     * Select write from buffer to screen
     * 33-34ms -> 29fps
     * optimized for full 128x64 frame
     * 0 - buffer #0
     * 1 - buffer #1
     * @param buf - Index of buffer to write to screen to screen
     */
    void display(int buf = 0);

    /**
     * Copy part of Frame buffer
     * @param x16 - starting index in horizontal direction
     * @param y  - starting index in vertical direction
     * @param w16 - width to copy
     * @param h - height to copy
     * @param buf - index of buffer to copy
     */
    void copy(uint8_t x16, uint8_t y, uint8_t w16, uint8_t h, uint8_t buf = 0);

    /**
     * Set cursor to position on screen
     * @param x  - position in horizontal direction
     * @param y - position in vertical direction
     */
    void gotoXY(byte x, byte y);

    /**
     *  Put the screen in standby mode
     * @param mode  - true - for sleep mode
     *              - false - for GFX mode
     */
    void sleep(bool mode = true);

    /**
     * Turn on Graphics mode for the screen
     * @param mode - true - on
     *             - false - off
     */
    void setGfxMode(bool mode = true);

    /**
     * Write basic characters to the screen
     * @param pos - starting row
     * @param str - char* buffer
     */
    void printTxt(uint8_t pos, char* str);

    /**
     * Write signs and uint8_t* buffers to the screen
     * @param pos - starting row
     * @param signs  - uint8_t* buffer
     */
    void printTxt(uint8_t pos, uint16_t* signs);

    /**
     * Clear screen buffer
     */
    void cls() override;

    /**
     * Write pixels to screen buffer
     * @param x - horizontal position on the screen
     * @param y - vertical position on the screen
     * @param col - The col of the screen to write to.
     */
    void drawPixel(uint8_t x, uint8_t y, uint8_t col = 0);

    /**
     * Draw line on screen
     * @param x0 - Starting position horizontally relative to top left corner of the screen
     * @param y0 - Starting position vertically relative to top left corner of the screen
     * @param x1 - Ending position
     * @param y1 - Ending position
     * @param col
     */
    void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t col = 0);
    /**
     * Draw horizontal line
     * @param x0
     * @param x1
     * @param y
     * @param col
     */
    void drawLineH(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col = 0);
    /**
     * Draw vertical line
     * @param x
     * @param y0
     * @param y1
     * @param col
     */
    void drawLineV(uint8_t x,  uint8_t y0, uint8_t y1, uint8_t col = 0);
    /**
     * Draw vertical line fast
     * @param x
     * @param y0
     * @param y1
     * @param col
     */
    void drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col = 0);
    /**
     * Draw vertical line dithered version
     * @param x
     * @param y0
     * @param y1
     * @param col
     */
    void drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col  = 0);
    /**
     * Draw horizontal lines about 40x faster than regular drawLineH
     * @param x0
     * @param x1
     * @param y
     * @param col
     */
    void drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col = 0);

    /**
     * Draw horizontal lines dithered version
     * @param x0
     * @param x1
     * @param y
     * @param col
     */
    void drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col = 0);

    /**
     * Draw rectangles
     * @param x0 - Top left corner x
     * @param y0 - Top left corner y
     * @param w - width
     * @param h - height
     * @param col  - buffer index
     */
    void drawRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col = 0);

    /**
     * Draw rectangles dithered version
     * @param x - Top left corner x
     * @param y - Top left corner y
     * @param w - width
     * @param h - height
     * @param col - buffer index
     */
    void drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col = 0);

    /**
     * Draw and fill rectangles
     * @param x0 - Top left corner x
     * @param y0 - Top left corner y
     * @param w - Width
     * @param h - Height
     * @param col - Buffer index
     */
    void fillRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col = 0);

    /**
     * Draw and fill rectangle dithered version
     * @param x0 - Top left corner x
     * @param y0 - Top left corner y
     * @param w - width
     * @param h - height
     * @param col - Buffer index
     */
    void fillRectD(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col = 0);

    /**
     * Draw Circle given radius
     * @param x0 - Center x
     * @param y0 - Center y
     * @param radius - Radius
     * @param col - buffer index
     */
    void drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col = 0);

    /**
     * Draw filled circle given radius
     * @param x0 - Center in x
     * @param y0 - Center in y
     * @param r - Radius
     * @param col - buffer index
     */
    void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col  = 0);

    /**
     * Draw filled circle dithered version
     * @param x0 - Center in x
     * @param y0 - Center in y
     * @param r - Radius
     * @param col  - buffer index
     */
    void fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col = 0);

    /**
     * Enable dithering
     * @param s
     */
    void setDither(uint8_t s);

    /**
     * Write portion of bitmap array (w x h ) to screen buffer
     * @param bmp - Bitmap array
     * @param x - starting position x
     * @param y - starting position y
     * @param w - width of bitmap
     * @param h - height pf bitmap
     * @return - Bottom right corner (x+w)
     */
    int drawBitmap(const uint8_t* bmp, int x, uint8_t y, uint8_t w, uint8_t h);

    /**
     * Write full bitmap array to screen buffer
     * @param bmp - Bitmap array
     * @param x - Starting position in x
     * @param y - Starting position in y
     * @return Bottom left corner  (x+w)
     */
    int drawBitmap(const uint8_t* bmp, int x, uint8_t y);

    /**
     * Select font
     * @param f - font array
     */
    void setFont(const uint8_t* f);

    /**
     * Set carriage return mode for printStr
     * @param _cr = 0
     */
    void setCR(uint8_t _cr)
    {
        _cr = _cr;
    }

    /**
     * Font inversion
     * @param _inv - inversion mode
     */
    void setInvert(uint8_t _inv)
    {
        _invertCh = _inv;
    }

    /**
     * Set font minimum width
     * @param wd
     */
    void setFontMinWd(uint8_t wd)
    {
        _cfont.minCharWd = wd;
    }

    /**
     * Set char mininum width field of the font structure
     * @param wd
     */
    void setCharMinWd(uint8_t wd)
    {
        _cfont.minCharWd = wd;
    }

    /**
     * Set digit minimum width  in the font structure
     * @param wd
     */
    void setDigitMinWd(uint8_t wd)
    {
        _cfont.minDigitWd = wd;
    }

    /**
     * Print character to screen buffer
     * @param xpos
     * @param ypos
     * @param c
     * @return
     */
    int printChar(int xpos, int ypos, unsigned char c);

    /**
     * Print string to screen buffer
     * @param xpos
     * @param ypos
     * @param str
     * @return
     */
    int printStr(int xpos, int ypos, const char* str);


    int charWidth(uint8_t _ch, bool last = true);

    int strWidth(const char* txt);

    unsigned char convertPolish(unsigned char _c);

    static bool isNumber(uint8_t ch);

    static bool isNumberExt(uint8_t ch);

    void setIsNumberFun(bool (*fun)(uint8_t))
    {
        _isNumberFun = fun;
    }
private:
    byte scr[SCR_WD * SCR_HT / 8] {};
    byte scrWd = 128 / 8;
    byte scrHt = 64 / 8;

    static byte xstab[8];
    static byte xetab[8];
    static byte pattern[4];
    static const byte ditherTab[4 * 17];
protected:
    /**
      * Write commands to screen registers
      * @param b  - Byte of command to write to screen
      */
    void _write_command(byte b) override;

    /**
     * Function to write data to screen registers
     * @param b - Byte of data to write to screen
     */
    void _write_data(byte b) override;
    void _write_byte(byte value) override
    {
        (void)value;
    }
    int address(int column, int row) override
    {
        (void)column;
        (void)row;
        return -1;
    }
    void _set_enable(bool value) override
    {
        _cs.write(value);
    }
    void _set_rs(bool value) override
    {
        _rs.write(value);
    }
private:
    bool (*_isNumberFun)(uint8_t ch) {};

    int _x{}, _y{};
    device_font_t _cfont{};
    uint8_t _cr{};
    uint8_t _dualChar{};
    uint8_t _invertCh{};
    uint8_t _spacing = 1;

    SPI _spi;
    DigitalOut _cs, _rs;

    constexpr byte get_incr_line(int y);
};

#endif