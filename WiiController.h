#ifndef _WII_CONTROLLER_H_
#define _WII_CONTROLLER_H_

#define WII_BUTTON_A			0
#define WII_BUTTON_B			1
#define WII_BUTTON_Z			2
#define WII_BUTTON_START	3
#define WII_BUTTON_UP			4
#define WII_BUTTON_DOWN		5
#define WII_BUTTON_LEFT		6
#define WII_BUTTON_RIGHT	7

class WiiController {
  public:
    WiiController(void);
    ~WiiController(void);
    void printButtonStates(void);
    boolean buttons[16];


  //private:
};

//WiiController wiiController;

#endif
