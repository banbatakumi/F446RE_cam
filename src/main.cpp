#include "mbed.h"

// UART通信定義 (TX, RX)
Serial main_mcu(PB_6, PB_7);
Serial cam_1(PA_2, PA_3);
Serial cam_2(PC_12, PD_2);
Serial cam_3(PA_9, PA_10);
Serial cam_4(PC_6, PC_7);

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
      main_mcu.baud(9600);
      //main_mcu.attach(main_mcu_rx, Serial::RxIrq);
      cam_1.baud(38400);
      cam_1.attach(cam_1_rx, Serial::RxIrq);
      /*
      cam_2.baud(9600);
      cam_2.attach(cam_2_rx, Serial::RxIrq);
      cam_3.baud(9600);
      cam_3.attach(cam_3_rx, Serial::RxIrq);
      cam_4.baud(9600);
      cam_4.attach(cam_4_rx, Serial::RxIrq);*/

      while (1) {
            main_mcu.putc('H');
            main_mcu.putc(front_ball_x);
            main_mcu.putc(front_ball_y);
            main_mcu.putc(front_y_goal_x);
            main_mcu.putc(front_y_goal_y);
            main_mcu.putc(front_y_goal_size);
            main_mcu.putc(front_b_goal_x);
            main_mcu.putc(front_b_goal_y);
            main_mcu.putc(front_b_goal_size);

            if (front_b_goal_x > 100) {
                  led_1 = 1;
            } else {
                  led_1 = 0;
            }
      }
}

void cam_1_rx() {
      if(cam_1.getc() == 'H'){
            front_ball_x = cam_1.getc();
            front_ball_y = cam_1.getc();
            front_y_goal_x = cam_1.getc();
            front_y_goal_y = cam_1.getc();
            front_y_goal_size = cam_1.getc();
            front_b_goal_x = cam_1.getc();
            front_b_goal_y = cam_1.getc();
            front_b_goal_size = cam_1.getc();
      }
}/*

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