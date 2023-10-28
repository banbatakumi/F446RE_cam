#ifndef MBED_M1N_H
#define MBED_M1N_H

#include "mbed.h"
#include "simplify_deg.h"
class M1n {
     public:
      M1n(PinName tx_, PinName rx_);

      int16_t GetBallDir();
      uint8_t GetBallDis();
      int16_t GetYellowGoalDir();
      uint8_t GetYellowGoalSize();
      int16_t GetBlueGoalDir();
      uint8_t GetBlueGoalSize();

     private:
      RawSerial serial;
      void Receive();

      int16_t ball_dir;
      uint8_t ball_dis;
      int16_t yellow_goal_dir;
      uint8_t yellow_goal_size;
      int16_t blue_goal_dir;
      uint8_t blue_goal_size;
};

#endif