#ifndef _WII_CONTROLLER_H_
#define _WII_CONTROLLER_H_

class WiiController {
  public:
    WiiController(void);
    ~WiiController(void);
    void printButtonStates(void);
    boolean buttons[16];


  //private:
};

#endif
