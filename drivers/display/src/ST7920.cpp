#include "ST7920.h"
#include "fonts/small4x7_font.h"
#include "fonts/small5x7_font.h"
#include <vector>
/*
#include "mbed_trace.h"
#define TRACE_GROUP "ST7920"*/

#define fontbyte(x) _cfont.font[x]

ST7920::ST7920(PinName mosi, PinName sclk, PinName cs, PinName rs):
    _spi(mosi, NC, sclk), _cs(cs), _rs(rs)
{}

void ST7920::init()
{
    scrWd = SCR_WD / 8;
    scrHt = SCR_HT;
    _isNumberFun = &isNumber;
    _cr = 0;
    _cfont.font = nullptr;
    _dualChar = 0;
    END_COMMAND
    _spi.format(SPI_BITS, SPI_MODE);
    _spi.frequency(SPI_SPEED);

    _write_command(LCD_BASIC);
    _write_command(LCD_BASIC);
    _write_command(LCD_CLS);
    ThisThread::sleep_for(2ms);
    _write_command(LCD_ADDRINC);
    _write_command(LCD_DISPLAYON);
    setGfxMode(true);

    this->cls();
    this->setFont(Small5x7PL);
}

void ST7920::_write_command(byte b)
{
    BEGIN_COMMAND
    _spi.write(0xF8);
    _spi.write(b & 0xF0);
    _spi.write(b << 4);
    END_COMMAND
}

void ST7920::_write_data(byte b)
{
    BEGIN_COMMAND
    _spi.write(0xFA);
    _spi.write(b & 0xF0);
    _spi.write(b << 4);
    END_COMMAND
}

// y =  0..63  -> buffer #0
// y = 64..127 -> buffer #1
void ST7920::gotoXY(byte x, byte y)
{
    if (y >= 32 && y < 64) {
        y -= 32;
        x += 8;
    } else if (y >= 64 && y < 64 + 32) {
        y -= 32;
        x += 0;
    } else if (y >= 64 + 32 && y < 64 + 64) {
        y -= 64;
        x += 8;
    }
    _write_command(LCD_ADDR | y); // 6-bit (0..63)
    _write_command(LCD_ADDR | x); // 4-bit (0..15)
}

// true - graphics mode
// false - text mode
void ST7920::setGfxMode(bool mode)
{
    if (mode) {
        _write_command(LCD_EXTEND);
        _write_command(LCD_GFXMODE);
    } else {
        _write_command(LCD_EXTEND);
        _write_command(LCD_TXTMODE);
    }
}

void ST7920::sleep(bool mode)
{
    //_write_command(mode?LCD_STANDBY:LCD_EXTEND);
    if (mode) {
        _write_command(LCD_STANDBY);
    } else {
        _write_command(LCD_BASIC);
        _write_command(LCD_EXTEND);
        _write_command(LCD_GFXMODE);
    }
}

void ST7920::switchBuf(byte b)
{
    _write_command(LCD_SCROLL);
    _write_command(LCD_SCROLLADDR + 32 * b);
}

void ST7920::display(int buf)
{
    byte i, j, b;
    for (j = 0; j < 64 / 2; j++) {
        gotoXY(0, j + buf * 64);
        BEGIN_COMMAND
        _spi.write(0xFA); // data
        for (i = 0; i < 16; i++) {  // 16 bytes from line #0+
            b = scr[i + j * 16];
            _spi.write(b & 0xF0);
            _spi.write(b << 4);
        }
        for (i = 0; i < 16; i++) {  // 16 bytes from line #32+
            b = scr[i + (j + 32) * 16];
            _spi.write(b & 0xF0);
            _spi.write(b << 4);
        }
        END_COMMAND
    }
}

void ST7920::copy(uint8_t x16, uint8_t y, uint8_t w16, uint8_t h, uint8_t buf)
{
    byte i, j, b;
    for (j = 0; j < h; j++) {
        gotoXY(x16, y + j + buf * 64);
        BEGIN_COMMAND
        _spi.write(0xFA); // data
        for (i = 0; i < w16; i++) {
            b = scr[(i + j * w16) * 2 + 0];
            _spi.write(b & 0xF0);
            _spi.write(b << 4);
            b = scr[(i + j * w16) * 2 + 1];
            _spi.write(b & 0xF0);
            _spi.write(b << 4);
        }
        END_COMMAND
    }
}

void ST7920::cls()
{
    //for(int i=0;i<scrWd*scrHt;i++) scr[i]=0;
    //memset(scr,0,scrWd*scrHt);
    memset(scr, 0, SCR_WD * SCR_HT / 8);
}

void ST7920::drawPixel(uint8_t x, uint8_t y, uint8_t col)
{
    if (x >= SCR_WD || y >= SCR_HT) {
        return;
    }
    switch (col) {
        case 1:
            scr[y * scrWd + x / 8] |= (0x80 >> (x & 7));
            break;
        case 0:
            scr[y * scrWd + x / 8] &= ~(0x80 >> (x & 7));
            break;
        case 2:
            scr[y * scrWd + x / 8] ^= (0x80 >> (x & 7));
            break;
    }
}

void ST7920::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t col)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        drawPixel(x0, y0, col);
        if (x0 == x1 && y0 == y1) {
            return;
        }
        int err2 = err + err;
        if (err2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (err2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void ST7920::drawLineH(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
    if (x1 > x0) for (uint8_t x = x0; x <= x1; x++) {
            drawPixel(x, y, col);
        } else for (uint8_t x = x1; x <= x0; x++) {
            drawPixel(x, y, col);
        }
}

void ST7920::drawLineV(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
    if (y1 > y0) for (uint8_t y = y0; y <= y1; y++) {
            drawPixel(x, y, col);
        } else for (uint8_t y = y1; y <= y0; y++) {
            drawPixel(x, y, col);
        }
}

// about 4x faster than regular drawLineV
void ST7920::drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
    uint8_t mask = 0x80 >> (x & 7);
    if (y1 < y0) {
        mask = y0;
        y0 = y1;
        y1 = mask; // swap
    }
    mask = 0x80 >> (x & 7);
    switch (col) {
        case 1:
            for (int y = y0; y <= y1; y++) {
                scr[y * scrWd + x / 8] |= mask;
            }
            break;
        case 0:
            for (int y = y0; y <= y1; y++) {
                scr[y * scrWd + x / 8] &= ~mask;
            }
            break;
        case 2:
            for (int y = y0; y <= y1; y++) {
                scr[y * scrWd + x / 8] ^= mask;
            }
            break;
    }
}

// limited to pattern #8
void ST7920::drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
    uint8_t mask = 0x80 >> (x & 7);
    if (y1 < y0) {
        mask = y0;
        y0 = y1;
        y1 = mask; // swap
    }
    if (((x & 1) == 1 && (y0 & 1) == 0) || ((x & 1) == 0 && (y0 & 1) == 1)) {
        y0++;
    }
    mask = 0x80 >> (x & 7);
    switch (col) {
        case 1:
            for (int y = y0; y <= y1; y += 2) {
                scr[y * scrWd + x / 8] |= mask;
            }
            break;
        case 0:
            for (int y = y0; y <= y1; y += 2) {
                scr[y * scrWd + x / 8] &= ~mask;
            }
            break;
        case 2:
            for (int y = y0; y <= y1; y += 2) {
                scr[y * scrWd + x / 8] ^= mask;
            }
            break;
    }
}

byte ST7920::xstab[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
byte ST7920::xetab[8] = {0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
byte ST7920::pattern[4] = {0xaa, 0x55, 0xaa, 0x55};

void ST7920::drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
    int yadd = y * scrWd;
    int x8s, x8e;
    if (x1 >= x0) {
        x8s = x0 / 8;
        x8e = x1 / 8;
    } else {
        x8s = x1;
        x1 = x0;
        x0 = x8s; // swap
        x8s = x1 / 8;
        x8e = x0 / 8;
    }
    switch (col) {
        case 1:
            if (x8s == x8e) {
                scr[yadd + x8s] |= (xstab[x0 & 7] & xetab[x1 & 7]);
            } else {
                scr[yadd + x8s] |= xstab[x0 & 7];
                scr[yadd + x8e] |= xetab[x1 & 7];
            }
            for (int x = x8s + 1; x < x8e; x++) {
                scr[yadd + x] = 0xff;
            }
            break;
        case 0:
            if (x8s == x8e) {
                scr[yadd + x8s] &= ~(xstab[x0 & 7] & xetab[x1 & 7]);
            } else {
                scr[yadd + x8s] &= ~xstab[x0 & 7];
                scr[yadd + x8e] &= ~xetab[x1 & 7];
            }
            for (int x = x8s + 1; x < x8e; x++) {
                scr[yadd + x] = 0x00;
            }
            break;
        case 2:
            if (x8s == x8e) {
                scr[yadd + x8s] ^= (xstab[x0 & 7] & xetab[x1 & 7]);
            } else {
                scr[yadd + x8s] ^= xstab[x0 & 7];
                scr[yadd + x8e] ^= xetab[x1 & 7];
            }
            for (int x = x8s + 1; x < x8e; x++) {
                scr[yadd + x] ^= 0xff;
            }
            break;
    }
}

// dithered version
void ST7920::drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
    int yadd = y * scrWd;
    int x8s, x8e;
    if (x1 >= x0) {
        x8s = x0 / 8;
        x8e = x1 / 8;
    } else {
        x8s = x1;
        x1 = x0;
        x0 = x8s; // swap
        x8s = x1 / 8;
        x8e = x0 / 8;
    }
    switch (col) {
        case 1:
            if (x8s == x8e) {
                scr[yadd + x8s] |= (xstab[x0 & 7] & xetab[x1 & 7] & pattern[y & 3]);
            } else {
                scr[yadd + x8s] |= (xstab[x0 & 7] & pattern[y & 3]);
                scr[yadd + x8e] |= (xetab[x1 & 7] & pattern[y & 3]);
            }
            for (int x = x8s + 1; x < x8e; x++) {
                scr[yadd + x] = pattern[y & 3];
            }
            break;
        case 0:
            if (x8s == x8e) {
                scr[yadd + x8s] &= ~(xstab[x0 & 7] & xetab[x1 & 7] & pattern[y & 3]);
            } else {
                scr[yadd + x8s] &= ~(xstab[x0 & 7] & pattern[y & 3]);
                scr[yadd + x8e] &= ~(xetab[x1 & 7] & pattern[y & 3]);
            }
            for (int x = x8s + 1; x < x8e; x++) {
                scr[yadd + x] = ~pattern[y & 3];
            }
            break;
        case 2:
            if (x8s == x8e) {
                scr[yadd + x8s] ^= (xstab[x0 & 7] & xetab[x1 & 7] & pattern[y & 3]);
            } else {
                scr[yadd + x8s] ^= (xstab[x0 & 7] & pattern[y & 3]);
                scr[yadd + x8e] ^= (xetab[x1 & 7] & pattern[y & 3]);
            }
            for (int x = x8s + 1; x < x8e; x++) {
                scr[yadd + x] ^= pattern[y & 3];
            }
            break;
    }
}

void ST7920::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
    if (x >= SCR_WD || y >= SCR_HT) {
        return;
    }
    byte drawVright = 1;
    if (x + w > SCR_WD) {
        w = SCR_WD - x;
        drawVright = 0;
    }
    if (y + h > SCR_HT) {
        h = SCR_HT - y;
    } else {
        drawLineHfast(x, x + w - 1, y + h - 1, col);
    }
    drawLineHfast(x, x + w - 1, y, col);
    drawLineVfast(x, y + 1, y + h - 2, col);
    if (drawVright) {
        drawLineVfast(x + w - 1, y + 1, y + h - 2, col);
    }
}

// dithered version (50% of brightness)
void ST7920::drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
    if (x >= SCR_WD || y >= SCR_HT) {
        return;
    }
    byte drawVright = 1;
    if (x + w > SCR_WD) {
        w = SCR_WD - x;
        drawVright = 0;
    }
    if (y + h > SCR_HT) {
        h = SCR_HT - y;
    } else {
        drawLineHfastD(x, x + w - 1, y + h - 1, col);
    }
    drawLineHfastD(x, x + w - 1, y, col);
    drawLineVfastD(x, y + 1, y + h - 2, col);
    if (drawVright) {
        drawLineVfastD(x + w - 1, y + 1, y + h - 2, col);
    }
}

void ST7920::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
    if (x >= SCR_WD || y >= SCR_HT) {
        return;
    }
    if (x + w > SCR_WD) {
        w = SCR_WD - x;
    }
    if (y + h > SCR_HT) {
        h = SCR_HT - y;
    }
    for (int i = y; i < y + h; i++) {
        drawLineHfast(x, x + w - 1, i, col);
    }
}

// dithered version (50% of brightness)
void ST7920::fillRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
    if (x >= SCR_WD || y >= SCR_HT) {
        return;
    }
    if (x + w >= SCR_WD) {
        w = SCR_WD - x;
    }
    if (y + h >= SCR_HT) {
        h = SCR_HT - y;
    }
    for (int i = y; i < y + h; i++) {
        drawLineHfastD(x, x + w - 1, i, col);
    }
}

// circle
void ST7920::drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col)
{
    int f = 1 - (int) radius;
    int ddF_x = 1;
    int ddF_y = -2 * (int) radius;
    int x = 0;
    int y = radius;

    drawPixel(x0, y0 + radius, col);
    drawPixel(x0, y0 - radius, col);
    drawPixel(x0 + radius, y0, col);
    drawPixel(x0 - radius, y0, col);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        drawPixel(x0 + x, y0 + y, col);
        drawPixel(x0 - x, y0 + y, col);
        drawPixel(x0 + x, y0 - y, col);
        drawPixel(x0 - x, y0 - y, col);
        drawPixel(x0 + y, y0 + x, col);
        drawPixel(x0 - y, y0 + x, col);
        drawPixel(x0 + y, y0 - x, col);
        drawPixel(x0 - y, y0 - x, col);
    }
}

void ST7920::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
    drawLineHfast(x0 - r, x0 - r + 2 * r + 1, y0, col);
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        drawLineHfast(x0 - x, x0 - x + 2 * x + 1, y0 + y, col);
        drawLineHfast(x0 - y, x0 - y + 2 * y + 1, y0 + x, col);
        drawLineHfast(x0 - x, x0 - x + 2 * x + 1, y0 - y, col);
        drawLineHfast(x0 - y, x0 - y + 2 * y + 1, y0 - x, col);
    }
}

// dithered version (50% of brightness)
void ST7920::fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
    drawLineHfastD(x0 - r, x0 - r + 2 * r + 1, y0, col);
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        drawLineHfastD(x0 - x, x0 - x + 2 * x + 1, y0 + y, col);
        drawLineHfastD(x0 - y, x0 - y + 2 * y + 1, y0 + x, col);
        drawLineHfastD(x0 - x, x0 - x + 2 * x + 1, y0 - y, col);
        drawLineHfastD(x0 - y, x0 - y + 2 * y + 1, y0 - x, col);
    }
}

// clang-format off
const byte ST7920::ditherTab[4 * 17] = {
    0x00, 0x00, 0x00, 0x00, // 0

    0x88, 0x00, 0x00, 0x00, // 1
    0x88, 0x00, 0x22, 0x00, // 2
    0x88, 0x00, 0xaa, 0x00, // 3
    0xaa, 0x00, 0xaa, 0x00, // 4
    0xaa, 0x44, 0xaa, 0x00, // 5
    0xaa, 0x44, 0xaa, 0x11, // 6
    0xaa, 0x44, 0xaa, 0x55, // 7

    0xaa, 0x55, 0xaa, 0x55, // 8

    0xaa, 0xdd, 0xaa, 0x55, // 9
    0xaa, 0xdd, 0xaa, 0x77, // 10
    0xaa, 0xdd, 0xaa, 0xff, // 11
    0xaa, 0xff, 0xaa, 0xff, // 12
    0xbb, 0xff, 0xaa, 0xff, // 13
    0xbb, 0xff, 0xee, 0xff, // 14
    0xbb, 0xff, 0xff, 0xff, // 15

    0xff, 0xff, 0xff, 0xff  // 16
};
// clang-format on

void ST7920::setDither(uint8_t s)
{
    if (s > 16) {
        return;
    }
    pattern[0] = *(ditherTab + s * 4 + 0);
    pattern[1] = *(ditherTab + s * 4 + 1);
    pattern[2] = *(ditherTab + s * 4 + 2);
    pattern[3] = *(ditherTab + s * 4 + 3);
}

int ST7920::drawBitmap(const uint8_t* bmp, int x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t wdb = w;
    // alignment
    if (x == -1) {
        x = SCR_WD - w;
    } else if (x < 0) {
        x = (SCR_WD - w) / 2;
    }
    if (x < 0) {
        x = 0;
    }
    if (x >= SCR_WD || y >= SCR_HT) {
        return 0;
    }
    if (x + w > SCR_WD) {
        w = SCR_WD - x;
    }
    if (y + h > SCR_HT) {
        h = SCR_HT - y;
    }

    byte i, j8, d, b, ht8 = (h + 7) / 8;
    for (j8 = 0; j8 < ht8; j8++) {
        for (i = 0; i < w; i++) {
            byte mask = 0x80 >> ((x + i) & 7);
            d = *(bmp + wdb * j8 + i);
            int lastbit = h - j8 * 8;
            if (lastbit > 8) {
                lastbit = 8;
            }
            for (b = 0; b < lastbit; b++) {
                if (d & 1) {
                    scr[(y + j8 * 8 + b) * scrWd + (x + i) / 8] |= mask;
                }
                d >>= 1;
            }
        }
    }
    return x + w;
}

int ST7920::drawBitmap(const uint8_t* bmp, int x, uint8_t y)
{
    uint8_t w = *(bmp + 0);
    uint8_t h = *(bmp + 1);
    return drawBitmap(bmp + 2, x, y, w, h);
}


void ST7920::setFont(const uint8_t* font)
{
    _cfont.font = font;
    _cfont.xSize = fontbyte(0);
    _cfont.ySize = fontbyte(1);
    _cfont.firstCh = fontbyte(2);
    _cfont.lastCh = fontbyte(3);
    _cfont.minDigitWd = 0;
    _cfont.minCharWd = 0;
    _isNumberFun = &isNumber;
    _spacing = 1;
    _cr = 0;
    _invertCh = 0;
}

int ST7920::charWidth(uint8_t c, bool last)
{
    c = convertPolish(c);
    if (c < _cfont.firstCh || c > _cfont.lastCh) {
        return c == ' ' ? 1 + _cfont.xSize / 2 : 0;
    }
    if (_cfont.xSize > 0) {
        return _cfont.xSize;
    }
    int ys8 = (_cfont.ySize + 7) / 8;
    int idx = 4 + (c - _cfont.firstCh) * (-_cfont.xSize * ys8 + 1);
    int wd = *(_cfont.font + idx);
    int wdL = 0, wdR = _spacing; // default _spacing before and behind char
    if ((*_isNumberFun)(c)) {
        if (_cfont.minDigitWd > wd) {
            wdL = (_cfont.minDigitWd - wd) / 2;
            wdR += (_cfont.minDigitWd - wd - wdL);
        }
    } else if (_cfont.minCharWd > wd) {
        wdL = (_cfont.minCharWd - wd) / 2;
        wdR += (_cfont.minCharWd - wd - wdL);
    }
    return last ? wd + wdL + wdR : wd + wdL + wdR - _spacing;  // last!=0 -> get rid of last empty columns
}

int ST7920::strWidth(const char* str)
{
    int wd = 0;
    while (*str) {
        wd += charWidth(*str++);
    }
    return wd;
}

int ST7920::printChar(int xpos, int ypos, unsigned char c)
{
    if (xpos >= SCR_WD || ypos >= SCR_HT) {
        return 0;
    }
    int fht8 = (_cfont.ySize + 7) / 8, wd, fwd = _cfont.xSize;
    if (fwd < 0) {
        fwd = -fwd;
    }

    c = convertPolish(c);
    if (c < _cfont.firstCh || c > _cfont.lastCh) {
        return c == ' ' ? 1 + fwd / 2 : 0;
    }

    int x, y8, b, cdata = (c - _cfont.firstCh) * (fwd * fht8 + 1) + 4;
    byte d;
    wd = fontbyte(cdata++);
    int wdL = 0, wdR = _spacing;
    if ((*_isNumberFun)(c)) {
        if (_cfont.minDigitWd > wd) {
            wdL = (_cfont.minDigitWd - wd) / 2;
            wdR += (_cfont.minDigitWd - wd - wdL);
        }
    } else if (_cfont.minCharWd > wd) {
        wdL = (_cfont.minCharWd - wd) / 2;
        wdR += (_cfont.minCharWd - wd - wdL);
    }
    if (xpos + wd + wdL + wdR > SCR_WD) {
        wdR = max(SCR_WD - xpos - wdL - wd, 0);
    }
    if (xpos + wd + wdL + wdR > SCR_WD) {
        wd = max(SCR_WD - xpos - wdL, 0);
    }
    if (xpos + wd + wdL + wdR > SCR_WD) {
        wdL = max(SCR_WD - xpos, 0);
    }

    for (x = 0; x < wd; x++) {
        byte mask = 0x80 >> ((xpos + x + wdL) & 7);
        for (y8 = 0; y8 < fht8; y8++) {
            d = fontbyte(cdata + x * fht8 + y8);
            int lastbit = _cfont.ySize - y8 * 8;
            if (lastbit > 8) {
                lastbit = 8;
            }
            for (b = 0; b < lastbit; b++) {
                if (d & 1)
                    scr[(ypos + y8 * 8 + b) * scrWd +
                                            (xpos + x + wdL) / 8] |= mask;  //drawPixel(xpos+x, ypos+y8*8+b, 1);
                d >>= 1;
            }
        }
    }
    return wd + wdR + wdL;
}

int ST7920::printStr(int xpos, int ypos, const char* str)
{
    unsigned char ch;
    int stl, row;
    int x = xpos;
    int y = ypos;
    int wd = strWidth(str);

    if (x == -1) { // right = -1
        x = SCR_WD - wd;
    } else if (x < 0) { // center = -2
        x = (SCR_WD - wd) / 2;
    }
    if (x < 0) {
        x = 0;    // left
    }

    while (*str) {
        int wd = printChar(x, y, *str++);
        x += wd;
        if (_cr && x >= SCR_WD) {
            x = 0;
            y += _cfont.ySize;
            if (y > SCR_HT) {
                y = 0;
            }
        }
    }
    if (_invertCh) {
        fillRect(xpos, x - 1, y, y + _cfont.ySize + 1, 2);
    }
    return x;
}

void ST7920::printTxt(uint8_t pos, char* str)
{
    _write_command(LCD_BASIC);
    _write_command(pos);
    while (*str) {
        _write_data(*str++);
    }
}

void ST7920::printTxt(uint8_t pos, uint16_t* signs)
{
    _write_command(LCD_BASIC);
    _write_command(pos);
    while (*signs) {
        _write_data(*signs >> 8);
        _write_data(*signs & 0xff);
        signs++;
    }
}

bool ST7920::isNumber(uint8_t ch)
{
    return isdigit(ch) || ch == ' ';
}

bool ST7920::isNumberExt(uint8_t ch)
{
    return isdigit(ch) || ch == '-' || ch == '+' || ch == '.' || ch == ' ';
}

unsigned char ST7920::convertPolish(unsigned char _c)
{
    unsigned char pl, c = _c;
    if (c == 196 || c == 197 || c == 195) {
        _dualChar = c;
        return 0;
    }
    if (_dualChar) { // UTF8 coding
        switch (_c) {
            case 133:
                pl = 1 + 9;
                break;
            case 135:
                pl = 2 + 9;
                break;
            case 153:
                pl = 3 + 9;
                break;
            case 130:
                pl = 4 + 9;
                break;
            case 132:
                pl = _dualChar == 197 ? 5 + 9 : 1;
                break;
            case 179:
                pl = 6 + 9;
                break;
            case 155:
                pl = 7 + 9;
                break;
            case 186:
                pl = 8 + 9;
                break;
            case 188:
                pl = 9 + 9;
                break;
            //case 132: pl = 1; break; // '�'
            case 134:
                pl = 2;
                break;
            case 152:
                pl = 3;
                break;
            case 129:
                pl = 4;
                break;
            case 131:
                pl = 5;
                break;
            case 147:
                pl = 6;
                break;
            case 154:
                pl = 7;
                break;
            case 185:
                pl = 8;
                break;
            case 187:
                pl = 9;
                break;
            default:
                return c;
                break;
        }
        _dualChar = 0;
    } else
        switch (_c) {  // Windows coding
            case 165:
                pl = 1;
                break;
            case 198:
                pl = 2;
                break;
            case 202:
                pl = 3;
                break;
            case 163:
                pl = 4;
                break;
            case 209:
                pl = 5;
                break;
            case 211:
                pl = 6;
                break;
            case 140:
                pl = 7;
                break;
            case 143:
                pl = 8;
                break;
            case 175:
                pl = 9;
                break;
            case 185:
                pl = 10;
                break;
            case 230:
                pl = 11;
                break;
            case 234:
                pl = 12;
                break;
            case 179:
                pl = 13;
                break;
            case 241:
                pl = 14;
                break;
            case 243:
                pl = 15;
                break;
            case 156:
                pl = 16;
                break;
            case 159:
                pl = 17;
                break;
            case 191:
                pl = 18;
                break;
            default:
                return c;
                break;
        }
    return pl + '~' + 1;
}

constexpr byte ST7920::get_incr_line(int y)
{
    switch (y) {
        case 0:
            return LCD_LINE0;
        case 1:
            return LCD_LINE1;
        case 2:
            return LCD_LINE2;
        case 3:
            return LCD_LINE3;
        default:
            _y = 0;
            return LCD_LINE0;
    }
}

/*int ST7920::printf(const char* format, ...)
{
    std::vector<char> buffer;

    va_list args;
    va_start(args, format);
    int length = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (length > 0) {
        buffer.reserve(static_cast<size_t>(length) + 1); // +1 for the null terminator

        va_start(args, format);
        std::vsnprintf(buffer.data(), buffer.size(), format, args);
        va_end(args);

        int ret = 0;
        _y = 0;

        std::string input(buffer.data(), static_cast<size_t>(length));
        tr_err("ST7920: %s", input.c_str());
        size_t start = 0;
        size_t delimiterPos;
        while ((delimiterPos = input.find('\n', start)) != std::string::npos) {
            std::string token = input.substr(start, delimiterPos - start);
            start = delimiterPos + 1;
            ret = this->printStr(ALIGN_LEFT, _y, token.c_str());
            _y += _cfont.ySize;
        }

        // Get the last token (if any).
        if (start < input.length()) {
            std::string token = input.substr(start);
            ret = this->printStr(ALIGN_LEFT, _y, token.c_str());
        }
        this->display(0);

        return ret;
    }

    return -1; // Error in vsnprintf
}*/

int ST7920::printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int length = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (length >= 0) {
        std::vector<char> buffer(length + 1); // +1 for the null terminator

        va_start(args, format);
        std::vsnprintf(buffer.data(), buffer.size(), format, args);
        va_end(args);

        uint8_t _y = 0;
        int ret  = 0;

        std::string input(buffer.data(), static_cast<size_t>(length));
        // tr_err("ST7920: %s", input.c_str());

        size_t start = 0;
        size_t delimiterPos;
        while ((delimiterPos = input.find('\n', start)) != std::string::npos) {
            std::string token = input.substr(start, delimiterPos - start);
            start = delimiterPos + 1;
            ret = this->printStr(ALIGN_LEFT, _y, token.c_str());
            _y += _cfont.ySize;
        }

        // Get the last token (if any).
        if (start < input.length()) {
            std::string token = input.substr(start);
            ret = this->printStr(ALIGN_LEFT, _y, token.c_str());
        }
        this->display(0);

        return ret;
    }
    return -1;
}
