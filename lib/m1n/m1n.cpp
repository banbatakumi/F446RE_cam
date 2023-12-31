#include "m1n.h"

#include "mbed.h"

M1n::M1n(PinName tx_, PinName rx_) : serial(tx_, rx_) {
      serial.baud(230400);
      serial.attach(callback(this, &M1n::Receive), Serial::RxIrq);
}

void M1n::Receive() {
      static uint8_t data_length;   // データの長さ
      const uint8_t recv_data_num = 7;
      static uint8_t recv_data[recv_data_num];

      if (data_length == 0) {   // ヘッダの受信
            if (serial.getc() == 0xFF) {
                  data_length++;
            } else {
                  data_length = 0;
            }
      } else if (data_length == recv_data_num + 1) {
            if (serial.getc() == 0xAA) {
                  ball_dir = recv_data[0];
                  ball_dis = recv_data[1];
                  yellow_goal_dir = recv_data[2];
                  yellow_goal_size = recv_data[3];
                  blue_goal_dir = recv_data[4];
                  blue_goal_size = recv_data[5];
                  is_goal_front = recv_data[6];
            }
            data_length = 0;
      } else {
            recv_data[data_length - 1] = serial.getc();
            data_length++;
      }
}