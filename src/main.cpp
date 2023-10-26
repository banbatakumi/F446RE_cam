#include "m1n.h"
#include "moving_ave.h"
#include "simplify_deg.h"

#define CAM_QTY 4
#define CAM_UART_SPEED 115200

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

int8_t ball_th[4] = {29, 74, 16, 65};

int8_t yellow_goal_th[4] = {-20, 21, 30, 76};

int8_t blue_goal_th[4] = {26, 78, -98, -51};

void setup() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      mainSerial.baud(115200);

      ballDirAve.SetLength(5);
      ballDisAve.SetLength(5);
      yGoalDirAve.SetLength(5);
      yGoalSizeAve.SetLength(5);
      bGoalDirAve.SetLength(5);
      bGoalSizeAve.SetLength(5);

      for (int i = 0; i < CAM_QTY; i++) {
            m1n[i].SetBallTh(ball_th);
            m1n[i].SetYellowGoalTh(yellow_goal_th);
            m1n[i].SetBlueGoalTh(blue_goal_th);
      }

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
                  ball_dir[i] = m1n[i].GetBallDir();
                  ball_dis[i] = m1n[i].GetBallDis();
                  yellow_goal_dir[i] = m1n[i].GetYellowGoalDir();
                  yellow_goal_size[i] = m1n[i].GetYellowGoalSize();
                  blue_goal_dir[i] = m1n[i].GetBlueGoalDir();
                  blue_goal_size[i] = m1n[i].GetBlueGoalSize();
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

      if (rslt_ball_dis == 0) {
            rslt_ball_dir = 0;
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

      if (rslt_ball_dis == 0) {
            rslt_ball_dir = 0;
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
      uint8_t send_byte_num = 10;
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

      for (uint8_t i = 0; i < send_byte_num; i++) {
            mainSerial.putc(send_byte[i]);
      }
      wait_us(100);
}