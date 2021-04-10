#include <M5Stack.h>

#ifndef SCROLLTEXTWINDOW_H
#define SCROLLTEXTWINDOW_H

#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240

class ScrollTextWindow {
private:
    uint16_t m_textWidth;
    uint16_t m_textHeight;
    uint16_t m_topFixedHeight;
    uint16_t m_bottomFixedHeight;
    uint16_t m_scrollableHeight;
    uint16_t m_scrollLimit;
    uint16_t m_backgroundColor;
    uint16_t m_yScrollPos;
    uint16_t m_yPos;
    uint16_t m_xPos;
    bool m_bScroll;

    void setupScrollArea(uint16_t tfa, uint16_t bfa);
    void scrollAddress(uint16_t vsp);
    
public:
    ScrollTextWindow(uint16_t topFixedHeight, uint16_t bottomFixedHeight, uint16_t bgColor = TFT_BLACK, uint8_t textWidth = 6, uint8_t textHeight = 8);
    void cls();
    void print(char c);
    void print(const char *str);
    void print(const String &str);
    void print(int num);
};

#endif //SCROLLTEXTWINDOW_H
