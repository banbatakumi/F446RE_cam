#include "m1n.h"

#include "mbed.h"

M1n::M1n(PinName tx_, PinName rx_) : serial(tx_, rx_) {
      serial.baud(115200);
      serial.attach(callback(this, &M1n::Receive), Serial::RxIrq);
}

void M1n::Receive() {
      static uint8_t data_length;   // データの長さ
      static uint8_t recv_data[6];

      if (data_length == 0) {   // ヘッダの受信
            if (serial.getc() == 0xFF) {
                  data_length++;
            } else {
                  data_length = 0;
            }
      } else if (data_length == 1) {
            recv_data[0] = serial.getc();
            data_length++;
      } else if (data_length == 2) {
            recv_data[1] = serial.getc();
            data_length++;
      } else if (data_length == 3) {
            recv_data[2] = serial.getc();
            data_length++;
      } else if (data_length == 4) {
            recv_data[3] = serial.getc();
            data_length++;
      } else if (data_length == 5) {
            recv_data[4] = serial.getc();
            data_length++;
      } else if (data_length == 6) {
            recv_data[5] = serial.getc();
            data_length++;
      } else if (data_length == 7) {
            if (serial.getc() == 0xAA) {
                  ball_dir = recv_data[0];
                  ball_dis = recv_data[1];
                  yellow_goal_dir = recv_data[2];
                  yellow_goal_size = recv_data[3];
                  blue_goal_dir = recv_data[4];
                  blue_goal_size = recv_data[5];
            }
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