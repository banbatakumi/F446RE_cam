#include "mbed.h"

// UART通信定義 (TX, RX)
RawSerial main_mcu(PB_6, PB_7);
RawSerial cam_1(PA_2, PA_3);
RawSerial cam_2(PC_12, PD_2);
RawSerial cam_3(PA_9, PA_10);
RawSerial cam_4(PC_6, PC_7);

// 関数定義
void main_mcu_rx();
void cam_1_rx();
void cam_2_rx();
void cam_3_rx();
void cam_4_rx();

// ピン定義
DigitalOut led_1(PB_10);
DigitalOut led_2(PB_2);
DigitalOut led_3(PB_1);
DigitalOut led_4(PB_0);

// グローバル変数定義
uint8_t front_ball_x = 0;
uint8_t front_ball_y = 0;
uint8_t front_y_goal_x = 0;
uint8_t front_y_goal_y = 0;
uint8_t front_y_goal_size = 0;
uint8_t front_b_goal_x = 0;
uint8_t front_b_goal_y = 0;
uint8_t front_b_goal_size = 0;

int main() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      main_mcu.baud(57600);
      // main_mcu.attach(main_mcu_rx, Serial::RxIrq);
      cam_1.baud(115200);
      cam_1.attach(cam_1_rx, Serial::RxIrq);
      /*
      cam_2.baud(9600);
      cam_2.attach(cam_2_rx, Serial::RxIrq);
      cam_3.baud(9600);
      cam_3.attach(cam_3_rx, Serial::RxIrq);
      cam_4.baud(9600);
      cam_4.attach(cam_4_rx, Serial::RxIrq);*/

      while (1) {
            main_mcu.putc(0xFF);
            main_mcu.putc(front_ball_x);
            main_mcu.putc(front_ball_y);
            main_mcu.putc(front_y_goal_x);
            main_mcu.putc(front_y_goal_y);
            main_mcu.putc(front_y_goal_size);
            main_mcu.putc(front_b_goal_x);
            main_mcu.putc(front_b_goal_y);
            main_mcu.putc(front_b_goal_size);
            main_mcu.putc(0xAA);

            if (front_ball_x > 100) {
                  led_1 = 1;
            } else {
                  led_1 = 0;
            }
      }
}

void cam_1_rx() {
      const uint8_t recv_byte_num = 10;
      uint8_t recv_byte[recv_byte_num];

      for (int i = 0; i < recv_byte_num; i++) {
            recv_byte[i] = cam_1.getc();   // 一旦すべてのデータを格納する
      }
      if (recv_byte[0] == 0xFF && recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
            front_ball_x = recv_byte[1];
            front_ball_y = recv_byte[2];
            front_y_goal_x = recv_byte[3];
            front_y_goal_y = recv_byte[4];
            front_y_goal_size = recv_byte[5];
            front_b_goal_x = recv_byte[6];
            front_b_goal_y = recv_byte[7];
            front_b_goal_size = recv_byte[8];
      }

      while (cam_1.readable() == 1) {   // 受信データがなくなるまで読み続ける・受信バッファを空にする
            cam_1.getc();   // データは格納されない
      }
} /*

 void cam_2_rx() {
       led_2 = 1;
 }

 void cam_3_rx() {
       led_3 = 1;
       if (cam_3.getc() == 'H') {
             back_ball_x = cam_3.getc();
             back_ball_y = cam_3.getc();
       }
 }

 void cam_4_rx() {
       led_4 = 1;
 }
 */