#include "m1n.h"
#include "mbed.h"
#include "moving_ave.h"
#include "simplify_deg.h"

#define CAM_QTY 4

#define SendTime(baud_, byte_num_) 1 / baud_ * 10 * byte_num_ * 1000000

// UART通信定義 (TX, RX)
RawSerial mainSerial(PC_10, PC_11);
M1n m1n[CAM_QTY] = {M1n(PA_2, PA_3), M1n(PC_12, PD_2), M1n(PA_9, PA_10), M1n(PC_6, PC_7)};

MovingAve ballDirAve;
MovingAve ballDisAve;
MovingAve yGoalDirAve;
MovingAve yGoalSizeAve;
MovingAve bGoalDirAve;
MovingAve bGoalSizeAve;

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
      mainSerial.baud(230400);

      ballDirAve.SetLength(5);
      ballDisAve.SetLength(5);
      yGoalDirAve.SetLength(15);
      yGoalSizeAve.SetLength(15);
      bGoalDirAve.SetLength(15);
      bGoalSizeAve.SetLength(15);

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
            for (int i = 0; i < CAM_QTY; i++) {
                  ball_dir[i] = m1n[i].ball_dir;
                  ball_dis[i] = m1n[i].ball_dis;
                  yellow_goal_dir[i] = m1n[i].yellow_goal_dir;
                  yellow_goal_size[i] = m1n[i].yellow_goal_size;
                  blue_goal_dir[i] = m1n[i].blue_goal_dir;
                  blue_goal_size[i] = m1n[i].blue_goal_size;
            }

            BallConversion();
            YGoalConversion();
            BGoalConversion();

            // それぞれの値に移動平均をかける
            ballDirAve.Compute(&rslt_ball_dir);
            ballDisAve.Compute(&rslt_ball_dis);
            yGoalDirAve.Compute(&rslt_yellow_goal_dir);
            yGoalSizeAve.Compute(&rslt_yellow_goal_size);
            bGoalDirAve.Compute(&rslt_blue_goal_dir);
            bGoalSizeAve.Compute(&rslt_blue_goal_size);

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
      const uint8_t send_byte_num = 12;
      uint8_t send_byte[send_byte_num];
      send_byte[0] = 0xFF;
      send_byte[1] = rslt_ball_dir > 0 ? rslt_ball_dir : 0;
      send_byte[2] = rslt_ball_dir < 0 ? rslt_ball_dir * -1 : 0;
      send_byte[3] = rslt_ball_dis;
      send_byte[4] = rslt_yellow_goal_dir > 0 ? rslt_yellow_goal_dir : 0;
      send_byte[5] = rslt_yellow_goal_dir < 0 ? rslt_yellow_goal_dir * -1 : 0;
      send_byte[6] = rslt_yellow_goal_size;
      send_byte[7] = rslt_blue_goal_dir > 0 ? rslt_blue_goal_dir : 0;
      send_byte[8] = rslt_blue_goal_dir < 0 ? rslt_blue_goal_dir * -1 : 0;
      send_byte[9] = rslt_blue_goal_size;
      send_byte[10] = m1n[0].is_goal_front;
      send_byte[11] = 0xAA;

      for (uint8_t i = 0; i < send_byte_num; i++) {
            mainSerial.putc(send_byte[i]);
      }
      wait_us(SendTime(230400, send_byte_num));
}