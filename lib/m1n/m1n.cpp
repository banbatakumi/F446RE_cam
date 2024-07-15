#include "m1n.h"

#include "mbed.h"

M1n::M1n(PinName tx_, PinName rx_, int16_t* own_dir_) : serial(tx_, rx_) {
      serial.baud(230400);
      serial.attach(callback(this, &M1n::Receive), SerialBase::RxIrq);  // シリアル割り込みの設定
      this->own_dir = own_dir_;
}

void M1n::Receive() {
      static uint8_t data_length;
      const uint8_t recv_data_num = 6;  // 受信データ数
      static uint8_t recv_data[recv_data_num];
      uint8_t read_byte;
      uint8_t send_byte;
      serial.read(&read_byte, 1);

      if (data_length == 0) {
            if (read_byte == 0xFF) {  // ヘッダーの受信
                  data_length++;
            } else {
                  data_length = 0;
            }
      } else if (data_length == recv_data_num + 1) {
            if (read_byte == 0xAA) {  // フッターの受信
                  ball_dir = recv_data[0];
                  ball_dis = recv_data[1];
                  goal_dir = recv_data[2];
                  goal_size = recv_data[3];
                  bool_data = recv_data[4];
                  proximity = recv_data[5];
                  is_goal_yellow = bool_data & 1;
                  is_goal_front = (bool_data >> 1) & 1;
                  court_dis = (bool_data >> 2) & 0b00111111;

                  send_byte = *own_dir * 0.5 + 90;
                  serial.write(&send_byte, 1);
            }
            data_length = 0;
      } else {
            recv_data[data_length - 1] = read_byte;
            data_length++;
      }
}