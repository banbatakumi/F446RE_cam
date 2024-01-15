#include "m1n.h"
#include "mbed.h"
#include "simplify_deg.h"

#define CAM_QTY 4

// UART通信定義 (TX, RX)
UnbufferedSerial mainSerial(PC_10, PC_11);
//M1n m1n[CAM_QTY] = {(PA_2, PA_3), (PC_12, PD_2), (PA_9, PA_10), (PC_6, PC_7)};
M1n m1n_1(PA_2, PA_3);
M1n m1n_2(PC_12, PD_2);
M1n m1n_3(PA_9, PA_10);
M1n m1n_4(PC_6, PC_7);

// 関数定義
void MainMcu();

void BallConversion();
void YGoalConversion();
void BGoalConversion();

// ピン定義
DigitalOut led[4] = {PB_10, PB_2, PB_1, PB_0};

// グローバル変数定義
uint8_t ball_dir[CAM_QTY];
uint8_t ball_dis[CAM_QTY];
uint8_t yellow_goal_dir[CAM_QTY];
uint8_t yellow_goal_size[CAM_QTY];
uint8_t blue_goal_dir[CAM_QTY];
uint8_t blue_goal_size[CAM_QTY];

int16_t rslt_ball_dir;
int16_t rslt_ball_dis;
int16_t rslt_yellow_goal_dir;
int16_t rslt_yellow_goal_size;
int16_t rslt_blue_goal_dir;
int16_t rslt_blue_goal_size;

void setup() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      mainSerial.baud(115200);

      __disable_irq();   // 禁止
      for (int i = 0; i < 4; i++) {
            led[i] = 1;
            wait_us(500000);
      }
      for (int i = 0; i < 4; i++) {
            led[i] = 0;
      }
      __enable_irq();   // 許可
}

int main() {
      setup();
      while (1) {
            ball_dir[0] = m1n_1.ball_dir;
            ball_dis[0] = m1n_1.ball_dis;
            if (m1n_1.is_goal_yellow == 1) {
                  yellow_goal_dir[0] = m1n_1.goal_dir;
                  yellow_goal_size[0] = m1n_1.goal_size;
                  blue_goal_dir[0] = 0;
                  blue_goal_size[0] = 0;
            } else {
                  yellow_goal_dir[0] = 0;
                  yellow_goal_size[0] = 0;
                  blue_goal_dir[0] = m1n_1.goal_dir;
                  blue_goal_size[0] = m1n_1.goal_size;
            }
            ball_dir[1] = m1n_2.ball_dir;
            ball_dis[1] = m1n_2.ball_dis;
            if (m1n_2.is_goal_yellow == 1) {
                  yellow_goal_dir[1] = m1n_2.goal_dir;
                  yellow_goal_size[1] = m1n_2.goal_size;
                  blue_goal_dir[1] = 0;
                  blue_goal_size[1] = 0;
            } else {
                  yellow_goal_dir[1] = 0;
                  yellow_goal_size[1] = 0;
                  blue_goal_dir[1] = m1n_2.goal_dir;
                  blue_goal_size[1] = m1n_2.goal_size;
            }
            ball_dir[2] = m1n_3.ball_dir;
            ball_dis[2] = m1n_3.ball_dis;
            if (m1n_3.is_goal_yellow == 1) {
                  yellow_goal_dir[2] = m1n_3.goal_dir;
                  yellow_goal_size[2] = m1n_3.goal_size;
                  blue_goal_dir[2] = 0;
                  blue_goal_size[2] = 0;
            } else {
                  yellow_goal_dir[2] = 0;
                  yellow_goal_size[2] = 0;
                  blue_goal_dir[2] = m1n_3.goal_dir;
                  blue_goal_size[2] = m1n_3.goal_size;
            }
            ball_dir[3] = m1n_4.ball_dir;
            ball_dis[3] = m1n_4.ball_dis;
            if (m1n_4.is_goal_yellow == 1) {
                  yellow_goal_dir[3] = m1n_4.goal_dir;
                  yellow_goal_size[3] = m1n_4.goal_size;
                  blue_goal_dir[3] = 0;
                  blue_goal_size[3] = 0;
            } else {
                  yellow_goal_dir[3] = 0;
                  yellow_goal_size[3] = 0;
                  blue_goal_dir[3] = m1n_4.goal_dir;
                  blue_goal_size[3] = m1n_4.goal_size;
            }

            BallConversion();
            YGoalConversion();
            BGoalConversion();

            MainMcu();
      }
}

void BallConversion() {
      uint8_t max_dis = 0;
      uint8_t max_dis_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {
            if (ball_dis[i] > max_dis) {
                  max_dis = ball_dis[i];
                  max_dis_num = i;
            }
      }
      rslt_ball_dis = max_dis;

      if (rslt_ball_dis == 0) {
            rslt_ball_dir = 0;
      } else if (max_dis_num == 0) {
            rslt_ball_dir = ball_dir[0] - 45;
      } else if (max_dis_num == 1) {
            rslt_ball_dir = ball_dir[1] + 45;
      } else if (max_dis_num == 2) {
            rslt_ball_dir = ball_dir[2] + 135;
      } else if (max_dis_num == 3) {
            rslt_ball_dir = ball_dir[3] + 225;
      }
      rslt_ball_dir = SimplifyDeg(rslt_ball_dir);
}

void YGoalConversion() {
      uint8_t max_size = 0;
      uint8_t max_size_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {
            if (yellow_goal_size[i] > max_size) {
                  max_size = yellow_goal_size[i];
                  max_size_num = i;
            }
      }
      rslt_yellow_goal_size = max_size;

      if (rslt_yellow_goal_size == 0) {
            rslt_yellow_goal_dir = 0;
      } else if (max_size_num == 0) {
            rslt_yellow_goal_dir = yellow_goal_dir[0] - 45;
      } else if (max_size_num == 1) {
            rslt_yellow_goal_dir = yellow_goal_dir[1] + 45;
      } else if (max_size_num == 2) {
            rslt_yellow_goal_dir = yellow_goal_dir[2] + 135;
      } else if (max_size_num == 3) {
            rslt_yellow_goal_dir = yellow_goal_dir[3] + 225;
      }
      rslt_yellow_goal_dir = SimplifyDeg(rslt_yellow_goal_dir);
}

void BGoalConversion() {
      uint8_t max_size = 0;
      uint8_t max_size_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {
            if (blue_goal_size[i] > max_size) {
                  max_size = blue_goal_size[i];
                  max_size_num = i;
            }
      }
      rslt_blue_goal_size = max_size;

      if (rslt_blue_goal_size == 0) {
            rslt_blue_goal_dir = 0;
      } else if (max_size_num == 0) {
            rslt_blue_goal_dir = blue_goal_dir[0] - 45;
      } else if (max_size_num == 1) {
            rslt_blue_goal_dir = blue_goal_dir[1] + 45;
      } else if (max_size_num == 2) {
            rslt_blue_goal_dir = blue_goal_dir[2] + 135;
      } else if (max_size_num == 3) {
            rslt_blue_goal_dir = blue_goal_dir[3] + 225;
      }
      rslt_blue_goal_dir = SimplifyDeg(rslt_blue_goal_dir);
}

void MainMcu() {
      const uint8_t send_byte_num = 9;
      uint8_t send_byte[send_byte_num];
      send_byte[0] = 0xFF;
      send_byte[1] = rslt_ball_dir / 2 + 90;
      send_byte[2] = rslt_ball_dis;
      send_byte[3] = rslt_yellow_goal_dir / 2 + 90;
      send_byte[4] = rslt_yellow_goal_size;
      send_byte[5] = rslt_blue_goal_dir / 2 + 90;
      send_byte[6] = rslt_blue_goal_size;
      send_byte[7] = m1n_1.is_goal_front;
      send_byte[8] = 0xAA;

      mainSerial.write(&send_byte, send_byte_num);
}