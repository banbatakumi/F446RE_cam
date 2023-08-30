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

int main() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      main_mcu.baud(38400);
      main_mcu.attach(main_mcu_rx, Serial::RxIrq);
      cam_1.baud(38400);
      cam_1.attach(cam_1_rx, Serial::RxIrq);
      cam_2.baud(38400);
      cam_2.attach(cam_2_rx, Serial::RxIrq);
      cam_3.baud(38400);
      cam_3.attach(cam_3_rx, Serial::RxIrq);
      cam_4.baud(38400);
      cam_4.attach(cam_4_rx, Serial::RxIrq);
      for (int i = 0; i < 10; i++) {
            led_1 = 1;
            wait_us(100000);
            led_1 = 0;
            wait_us(100000);
      }
      led_2 = 1;
      wait_us(1000000);
      led_2 = 0;
      wait_us(1000000);
      while (1) {
      }
}

void main_mcu_rx() {
}

void cam_1_rx() {
}

void cam_2_rx() {
}

void cam_3_rx() {
}

void cam_4_rx() {
}
