#ifndef _N64_SYSTEM_H_
#define _N64_SYSTEM_H_

// Dumb toggle so we can actually fake buttons
// These used to be 0x04 which I think is B00000100, but if something breaks, change these
#define SET_N64_PIN_HIGH DDRD &= ~B00000100
#define SET_N64_PIN_LOW  DDRD |=  B00000100
#define QUERY_N64_PIN    (PIND &  B00000100)

#define WHITE_LED_ON   PORTD |= B00001000
#define WHITE_LED_OFF  PORTD &= B11110111

#define RED_LED_ON     PORTD |= B00010000
#define RED_LED_OFF    PORTD &= B11101111


class N64 {
  public:
    N64();
    ~N64();
    void init();
    int DATA_PIN;
};

#endif
