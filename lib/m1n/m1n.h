#ifndef MBED_M1N_H
#define MBED_M1N_H

#include "mbed.h"
#include "simplify_deg.h"
class M1n {
     public:
      M1n(PinName tx_, PinName rx_);

      uint8_t ball_dir;
      uint8_t ball_dis;
      uint8_t goal_dir;
      uint8_t goal_size;
      uint8_t bool_data;
      uint8_t enemy_dir;
      uint8_t court_dis;
      bool is_goal_front;
      bool is_goal_yellow;

     private:
      UnbufferedSerial serial;
      void Receive();
};

#endif