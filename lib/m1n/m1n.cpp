#include "m1n.h"

#include "mbed.h"

M1n::M1n(PinName tx_, PinName rx_) : serial(tx_, rx_) {
      serial.baud(115200);
      serial.attach(callback(this, &M1n::Receive), Serial::RxIrq);
}

void M1n::Receive() {
      static uint8_t data_length;   // データの長さ

      if (data_length == 0) {   // ヘッダの受信
            uint8_t head = serial.getc();
            if (head == 0xFF) {
                  data_length += 1;
            } else {
                  data_length = 0;
            }
      } else if (data_length == 1) {
            ball_dir = serial.getc();
            data_length += 1;
      } else if (data_length == 2) {
            ball_dis = serial.getc();
            data_length += 1;
      } else if (data_length == 3) {
            yellow_goal_dir = serial.getc();
            data_length += 1;
      } else if (data_length == 4) {
            yellow_goal_size = serial.getc();
            data_length += 1;
      } else if (data_length == 5) {
            blue_goal_dir = serial.getc();
            data_length += 1;
      } else if (data_length == 6) {
            blue_goal_size = serial.getc();
            data_length = 0;
      }
}

int16_t M1n::GetBallDir() {
      return ball_dir;
}

uint8_t M1n::GetBallDis() {
      return ball_dis;
}

int16_t M1n::GetYellowGoalDir() {
      return yellow_goal_dir;
}

uint8_t M1n::GetYellowGoalSize() {
      return yellow_goal_size;
}

int16_t M1n::GetBlueGoalDir() {
      return blue_goal_dir;
}

uint8_t M1n::GetBlueGoalSize() {
      return blue_goal_size;
}