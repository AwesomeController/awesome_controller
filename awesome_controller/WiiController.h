#ifndef _WII_CONTROLLER_H_
#define _WII_CONTROLLER_H_

#define WII_REMOTE_2      0
#define WII_REMOTE_1      1
#define WII_REMOTE_B      2
#define WII_REMOTE_PLUS   3
#define WII_REMOTE_UP     4
#define WII_REMOTE_DOWN   5
#define WII_REMOTE_LEFT   6
#define WII_REMOTE_RIGHT  7

class WiiController {
  public:
    typedef struct ClassicButtons {
      bool    A;
      bool    B;
      bool    X;
      bool    Y;
      bool    Up;
      bool    Down;
      bool    Left;
      bool    Right;
      bool    Minus;
      bool    Plus;
      bool    Home;
      bool    ZL;
      bool    ZR;
      int16_t L;
      bool    LClick;
      int16_t R;
      bool    RClick;
      int32_t LeftStickX;
      int32_t LeftStickY;
      int16_t RightStickX;
      int16_t RightStickY;
    } ClassicButtons_t;

    WiiController(void);
    ~WiiController(void);
    void printButtonStates(void);
    boolean buttons[16];
    ClassicButtons_t classicButtons;

  //private:
};

//WiiController wiiController;

#endif
