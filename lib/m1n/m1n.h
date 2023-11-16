#ifndef MBED_M1N_H
#define MBED_M1N_H

#include "mbed.h"
#include "simplify_deg.h"
class M1n {
     public:
      M1n(PinName tx_, PinName rx_);

      uint8_t ball_dir;
      uint8_t ball_dis;
      uint8_t yellow_goal_dir;
      uint8_t yellow_goal_size;
      uint8_t blue_goal_dir;
      uint8_t blue_goal_size;

     private:
      RawSerial serial;
      void Receive();
};

#endif