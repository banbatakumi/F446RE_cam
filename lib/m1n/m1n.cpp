#include "m1n.h"

#include "mbed.h"

M1n::M1n(PinName tx_, PinName rx_) : serial(tx_, rx_) {
      serial.baud(115200);
      serial.attach(callback(this, &M1n::Receive), SerialBase::RxIrq);
}

void M1n::Receive() {
      static uint8_t data_length;   // データの長さ
      const uint8_t recv_data_num = 5;
      static uint8_t recv_data[recv_data_num];
      uint8_t read_byte;
      serial.read(&read_byte, 1);

      if (data_length == 0) {   // ヘッダの受信
            if (read_byte == 0xFF) {
                  data_length++;
            } else {
                  data_length = 0;
            }
      } else if (data_length == recv_data_num + 1) {
            if (read_byte == 0xAA) {
                  ball_dir = recv_data[0];
                  ball_dis = recv_data[1];
                  goal_dir = recv_data[2];
                  goal_size = recv_data[3];
                  bool_data = recv_data[4];
                  is_goal_yellow = bool_data & 1;
                  is_goal_front = (bool_data >> 1) & 1;
            }
            data_length = 0;
      } else {
            recv_data[data_length - 1] = read_byte;
            data_length++;
      }
}