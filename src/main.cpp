#include "mbed.h"
#include "simplify_deg.h"

#define CAM_QTY 4

// UART通信定義 (TX, RX)
RawSerial main_mcu(PC_10, PC_11);
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
uint8_t ball_dir[CAM_QTY];
uint8_t ball_dis[CAM_QTY];
uint8_t y_goal_dir[CAM_QTY];
uint8_t y_goal_dis[CAM_QTY];
uint8_t y_goal_size[CAM_QTY];
uint8_t b_goal_dir[CAM_QTY];
uint8_t b_goal_dis[CAM_QTY];
uint8_t b_goal_size[CAM_QTY];

int16_t rslt_ball_dir;
uint8_t rslt_ball_dis;

int main() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      main_mcu.baud(57600);
      // main_mcu.attach(main_mcu_rx, Serial::RxIrq);
      cam_1.baud(115200);
      cam_1.attach(cam_1_rx, Serial::RxIrq);
      cam_2.baud(115200);
      cam_2.attach(cam_2_rx, Serial::RxIrq);
      cam_3.baud(115200);
      cam_3.attach(cam_3_rx, Serial::RxIrq);
      cam_4.baud(115200);
      cam_4.attach(cam_4_rx, Serial::RxIrq);

      for (int i = 0; i < 5; i++) {
            led_1 = 1;
            wait_us(50000);
            led_2 = 1;
            wait_us(50000);
            led_3 = 1;
            wait_us(50000);
            led_4 = 1;
            wait_us(50000);
            led_1 = 0;
            led_2 = 0;
            led_3 = 0;
            led_4 = 0;
            wait_us(50000);
      }

      while (1) {
            uint8_t max_dis = 0;
            uint8_t max_dis_num = 0;
            for (int i = 0; i < CAM_QTY; i++) {
                  if (ball_dis[i] > max_dis) {
                        max_dis = ball_dis[i];
                        max_dis_num = i;
                  }
            }
            rslt_ball_dis = max_dis;

            if (max_dis_num == 0) {
                  rslt_ball_dir = ball_dir[0] - 45;
            } else if (max_dis_num == 1) {
                  rslt_ball_dir = ball_dir[1] + 45;
            } else if (max_dis_num == 2) {
                  rslt_ball_dir = ball_dir[2] + 135;
            } else if (max_dis_num == 3) {
                  rslt_ball_dir = ball_dir[3] + 225;
            }
            rslt_ball_dir = SimplifyDeg(rslt_ball_dir);

            uint8_t send_byte_num = 5;
            uint8_t send_byte[send_byte_num];
            send_byte[0] = 0xFF;
            send_byte[1] = rslt_ball_dir > 0 ? rslt_ball_dir : 0;
            send_byte[2] = rslt_ball_dir < 0 ? rslt_ball_dir * -1 : 0;
            send_byte[3] = rslt_ball_dis;
            send_byte[4] = 0xAA;
            for (uint8_t i = 0; i < send_byte_num; i++) {
                  main_mcu.putc(send_byte[i]);
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
            ball_dir[0] = recv_byte[1];
            ball_dis[0] = recv_byte[2];
            y_goal_dir[0] = recv_byte[3];
            y_goal_dis[0] = recv_byte[4];
            y_goal_size[0] = recv_byte[5];
            b_goal_dir[0] = recv_byte[6];
            b_goal_dis[0] = recv_byte[7];
            b_goal_size[0] = recv_byte[8];
      }

      while (cam_1.readable() == 1) {   // 受信データがなくなるまで読み続ける・受信バッファを空にする
            cam_1.getc();   // データは格納されない
      }
}

void cam_2_rx() {
      const uint8_t recv_byte_num = 10;
      uint8_t recv_byte[recv_byte_num];

      for (int i = 0; i < recv_byte_num; i++) {
            recv_byte[i] = cam_2.getc();   // 一旦すべてのデータを格納する
      }

      if (recv_byte[0] == 0xFF && recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
            ball_dir[1] = recv_byte[1];
            ball_dis[1] = recv_byte[2];
            y_goal_dir[1] = recv_byte[3];
            y_goal_dis[1] = recv_byte[4];
            y_goal_size[1] = recv_byte[5];
            b_goal_dir[1] = recv_byte[6];
            b_goal_dis[1] = recv_byte[7];
            b_goal_size[1] = recv_byte[8];
      }

      while (cam_2.readable() == 1) {   // 受信データがなくなるまで読み続ける・受信バッファを空にする
            cam_2.getc();   // データは格納されない
      }
}

void cam_3_rx() {
      const uint8_t recv_byte_num = 10;
      uint8_t recv_byte[recv_byte_num];

      for (int i = 0; i < recv_byte_num; i++) {
            recv_byte[i] = cam_3.getc();   // 一旦すべてのデータを格納する
      }

      if (recv_byte[0] == 0xFF && recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
            ball_dir[2] = recv_byte[1];
            ball_dis[2] = recv_byte[2];
            y_goal_dir[2] = recv_byte[3];
            y_goal_dis[2] = recv_byte[4];
            y_goal_size[2] = recv_byte[5];
            b_goal_dir[2] = recv_byte[6];
            b_goal_dis[2] = recv_byte[7];
            b_goal_size[2] = recv_byte[8];
      }

      while (cam_3.readable() == 1) {   // 受信データがなくなるまで読み続ける・受信バッファを空にする
            cam_3.getc();   // データは格納されない
      }
}

void cam_4_rx() {
      const uint8_t recv_byte_num = 10;
      uint8_t recv_byte[recv_byte_num];

      for (int i = 0; i < recv_byte_num; i++) {
            recv_byte[i] = cam_4.getc();   // 一旦すべてのデータを格納する
      }

      if (recv_byte[0] == 0xFF && recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
            ball_dir[3] = recv_byte[1];
            ball_dis[3] = recv_byte[2];
            y_goal_dir[3] = recv_byte[3];
            y_goal_dis[3] = recv_byte[4];
            y_goal_size[3] = recv_byte[5];
            b_goal_dir[3] = recv_byte[6];
            b_goal_dis[3] = recv_byte[7];
            b_goal_size[3] = recv_byte[8];
      }

      while (cam_4.readable() == 1) {   // 受信データがなくなるまで読み続ける・受信バッファを空にする
            cam_4.getc();   // データは格納されない
      }
}