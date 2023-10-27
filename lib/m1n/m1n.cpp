#include "m1n.h"

#include "mbed.h"

M1n::M1n(PinName tx_, PinName rx_) : serial(tx_, rx_) {
      serial.baud(115200);
      serial.attach(callback(this, &M1n::Receive), Serial::RxIrq);

      thSendTimer.start();
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

            // 送信
            if (thSendTimer.read() < 5) {
                  uint8_t send_byte[14];
                  send_byte[0] = 0xFF;
                  send_byte[1] = ball_th[0] + 127;
                  send_byte[2] = ball_th[1] + 127;
                  send_byte[3] = ball_th[2] + 127;
                  send_byte[4] = ball_th[3] + 127;
                  send_byte[5] = yellow_goal_th[0] + 127;
                  send_byte[6] = yellow_goal_th[1] + 127;
                  send_byte[7] = yellow_goal_th[2] + 127;
                  send_byte[8] = yellow_goal_th[3] + 127;
                  send_byte[9] = blue_goal_th[0] + 127;
                  send_byte[10] = blue_goal_th[1] + 127;
                  send_byte[11] = blue_goal_th[2] + 127;
                  send_byte[12] = blue_goal_th[3] + 127;
                  send_byte[13] = 0xAA;

                  for (uint8_t i = 0; i < 14; i++) {
                        serial.putc(send_byte[i]);
                  }
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

void M1n::SetBallTh(int8_t* ball_th_) {
      for (uint8_t i = 0; i++; i < 4) {
            this->ball_th[i] = ball_th_[i];
      }
}

void M1n::SetYellowGoalTh(int8_t* yellow_goal_th_) {
      for (uint8_t i = 0; i++; i < 4) {
            this->yellow_goal_th[i] = yellow_goal_th_[i];
      }
}

void M1n::SetBlueGoalTh(int8_t* blue_goal_th_) {
      for (uint8_t i = 0; i++; i < 4) {
            this->blue_goal_th[i] = blue_goal_th_[i];
      }
}