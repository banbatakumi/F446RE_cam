#include "m1n.h"
#include "mbed.h"
#include "simplify_deg.h"

#define CAM_QTY 4
#define BALL_MISS_TIME 500   // ボールを見失った時にゼロにするまでの時間
#define GOAL_MISS_TIME 1000  // ゴールを見失った時にゼロにするまでの時間

#define readms(timer_name_) chrono::duration_cast<chrono::milliseconds>((timer_name_).elapsed_time()).count()  // MbedOS6のタイマー

// UART通信定義 (TX, RX)
UnbufferedSerial mainSerial(PC_10, PC_11);
M1n m1n_1(PA_2, PA_3);
M1n m1n_2(PC_12, PD_2);
M1n m1n_3(PA_9, PA_10);
M1n m1n_4(PC_6, PC_7);

// 関数定義
void MainMcu();
void BallConversion();
void YellowGoalConversion();
void BlueGoalConversion();
void OwnPositionConversion();
void ValueAssignment();

// タイマー
Timer missYellowGoalTimer;
Timer missBlueGoalTimer;
Timer missBallTimer;

// ピン定義
DigitalOut led[4] = {PB_10, PB_2, PB_1, PB_0};

// グローバル変数定義
uint8_t ball_dir[CAM_QTY];
uint8_t ball_dis[CAM_QTY];
uint8_t yellow_goal_dir[CAM_QTY];
uint8_t yellow_goal_size[CAM_QTY];
uint8_t blue_goal_dir[CAM_QTY];
uint8_t blue_goal_size[CAM_QTY];
uint8_t court_dis[CAM_QTY];

int16_t rslt_ball_dir;
int16_t rslt_ball_dis;
int16_t rslt_yellow_goal_dir;
int16_t rslt_yellow_goal_size;
int16_t rslt_blue_goal_dir;
int16_t rslt_blue_goal_size;
int8_t rslt_own_x;
int8_t rslt_own_y;

int16_t pre_rslt_ball_dir;
int16_t pre_rslt_ball_dis;
int16_t pre_rslt_yellow_goal_dir;
int16_t pre_rslt_yellow_goal_size;
int16_t pre_rslt_blue_goal_dir;
int16_t pre_rslt_blue_goal_size;

void setup() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      mainSerial.baud(230400);

      __disable_irq();  // 禁止
      for (int i = 0; i < 4; i++) {
            led[i] = 1;
            wait_us(500000);
      }
      for (int i = 0; i < 4; i++) {
            led[i] = 0;
      }
      __enable_irq();  // 許可
}

int main() {
      setup();
      while (1) {
            ValueAssignment();

            BallConversion();
            YellowGoalConversion();
            BlueGoalConversion();
            OwnPositionConversion();

            MainMcu();  // UART
      }
}

void BallConversion() {
      uint8_t min_dis = 200;
      uint8_t min_dis_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {  // ロボットから一番近いボールを捉えているカメラを特定
            if (ball_dis[i] > 0 && ball_dis[i] < min_dis) {
                  min_dis = ball_dis[i];
                  min_dis_num = i;
            }
      }
      if (min_dis == 200) min_dis = 0;
      rslt_ball_dis = min_dis;

      if (rslt_ball_dis != 0) {
            missBallTimer.reset();
            missBallTimer.stop();
      }

      if (rslt_ball_dis == 0) {  // ボールが見つからない
            missBallTimer.start();
            if (readms(missBallTimer) > BALL_MISS_TIME) {
                  rslt_ball_dir = 0;
                  rslt_ball_dis = 0;
                  missBallTimer.stop();
            } else {
                  rslt_ball_dir = pre_rslt_ball_dir;
                  rslt_ball_dis = pre_rslt_ball_dis;
            }
      } else if (min_dis_num == 0) {
            rslt_ball_dir = ball_dir[0] + 315;
      } else if (min_dis_num == 1) {
            rslt_ball_dir = ball_dir[1] + 45;
      } else if (min_dis_num == 2) {
            rslt_ball_dir = ball_dir[2] + 135;
      } else if (min_dis_num == 3) {
            rslt_ball_dir = ball_dir[3] + 225;
      }
      rslt_ball_dir = SimplifyDeg(rslt_ball_dir);

      pre_rslt_ball_dir = rslt_ball_dir;
      pre_rslt_ball_dis = rslt_ball_dis;
}

void YellowGoalConversion() {
      uint8_t max_size = 0;
      uint8_t max_size_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {  // ゴールの縦サイズが一番大きいカメラを特定
            if (yellow_goal_size[i] > max_size) {
                  max_size = yellow_goal_size[i];
                  max_size_num = i;
            }
      }
      rslt_yellow_goal_size = max_size;

      if (rslt_yellow_goal_size != 0) {
            missYellowGoalTimer.reset();
            missYellowGoalTimer.stop();
      }

      if (rslt_yellow_goal_size == 0) {  // ゴールが見つからない
            missYellowGoalTimer.start();
            if (readms(missYellowGoalTimer) > GOAL_MISS_TIME) {
                  rslt_yellow_goal_dir = 0;
                  rslt_yellow_goal_size = 0;
                  missYellowGoalTimer.stop();
            } else {
                  rslt_yellow_goal_dir = pre_rslt_yellow_goal_dir;
                  rslt_yellow_goal_size = pre_rslt_yellow_goal_size;
            }
      } else if (max_size_num == 0) {
            rslt_yellow_goal_dir = yellow_goal_dir[0] + 315;
      } else if (max_size_num == 1) {
            rslt_yellow_goal_dir = yellow_goal_dir[1] + 45;
      } else if (max_size_num == 2) {
            rslt_yellow_goal_dir = yellow_goal_dir[2] + 135;
      } else if (max_size_num == 3) {
            rslt_yellow_goal_dir = yellow_goal_dir[3] + 225;
      }
      rslt_yellow_goal_dir = SimplifyDeg(rslt_yellow_goal_dir);

      pre_rslt_yellow_goal_dir = rslt_yellow_goal_dir;
      pre_rslt_yellow_goal_size = rslt_yellow_goal_size;
}

void BlueGoalConversion() {
      uint8_t max_size = 0;
      uint8_t max_size_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {  // ゴールの縦サイズが一番大きいカメラを特定
            if (blue_goal_size[i] > max_size) {
                  max_size = blue_goal_size[i];
                  max_size_num = i;
            }
      }
      rslt_blue_goal_size = max_size;

      if (rslt_blue_goal_size != 0) {
            missBlueGoalTimer.reset();
            missBlueGoalTimer.stop();
      }

      if (rslt_blue_goal_size == 0) {  // ゴールが見つからない
            missBlueGoalTimer.start();
            if (readms(missBlueGoalTimer) > GOAL_MISS_TIME) {
                  rslt_blue_goal_dir = 0;
                  rslt_blue_goal_size = 0;
                  missBlueGoalTimer.stop();
            } else {
                  rslt_blue_goal_dir = pre_rslt_blue_goal_dir;
                  rslt_blue_goal_size = pre_rslt_blue_goal_size;
            }
      } else if (max_size_num == 0) {
            rslt_blue_goal_dir = blue_goal_dir[0] + 315;
      } else if (max_size_num == 1) {
            rslt_blue_goal_dir = blue_goal_dir[1] + 45;
      } else if (max_size_num == 2) {
            rslt_blue_goal_dir = blue_goal_dir[2] + 135;
      } else if (max_size_num == 3) {
            rslt_blue_goal_dir = blue_goal_dir[3] + 225;
      }
      rslt_blue_goal_dir = SimplifyDeg(rslt_blue_goal_dir);

      pre_rslt_blue_goal_dir = rslt_blue_goal_dir;
      pre_rslt_blue_goal_size = rslt_blue_goal_size;
}

void MainMcu() {
      const uint8_t send_byte_num = 13;
      uint8_t send_byte[send_byte_num];
      send_byte[0] = 0xFF;
      send_byte[1] = (uint8_t)(((uint16_t)(rslt_ball_dir + 32768) & 0xFF00) >> 8);
      send_byte[2] = (uint8_t)((uint16_t)(rslt_ball_dir + 32768) & 0x00FF);
      send_byte[3] = rslt_ball_dis;
      send_byte[4] = rslt_yellow_goal_dir / 2 + 90;
      send_byte[5] = rslt_yellow_goal_size;
      send_byte[6] = rslt_blue_goal_dir / 2 + 90;
      send_byte[7] = rslt_blue_goal_size;
      send_byte[8] = m1n_1.enemy_dir;
      send_byte[9] = rslt_own_x + 127;
      send_byte[10] = rslt_own_y + 127;
      send_byte[11] = m1n_1.is_goal_front;
      send_byte[12] = 0xAA;

      mainSerial.write(&send_byte, send_byte_num);
}

void OwnPositionConversion() {
      rslt_own_x = court_dis[3] - court_dis[1];
      rslt_own_y = court_dis[2] - court_dis[0];
}

void ValueAssignment() {  // それぞれのカメラからの情報を配列にまとめる
      // 前カメラ
      ball_dir[0] = m1n_1.ball_dir;
      ball_dis[0] = m1n_1.ball_dis;
      yellow_goal_dir[0] = 0;
      yellow_goal_size[0] = 0;
      blue_goal_dir[0] = 0;
      blue_goal_size[0] = 0;
      court_dis[0] = m1n_1.court_dis;
      if (m1n_1.is_goal_yellow == 1) {
            yellow_goal_dir[0] = m1n_1.goal_dir;
            yellow_goal_size[0] = m1n_1.goal_size;
      } else {
            blue_goal_dir[0] = m1n_1.goal_dir;
            blue_goal_size[0] = m1n_1.goal_size;
      }
      // 右カメラ
      ball_dir[1] = m1n_2.ball_dir;
      ball_dis[1] = m1n_2.ball_dis;
      yellow_goal_dir[1] = 0;
      yellow_goal_size[1] = 0;
      blue_goal_dir[1] = 0;
      blue_goal_size[1] = 0;
      court_dis[1] = m1n_2.court_dis;
      if (m1n_2.is_goal_yellow == 1) {
            yellow_goal_dir[1] = m1n_2.goal_dir;
            yellow_goal_size[1] = m1n_2.goal_size;
      } else {
            blue_goal_dir[1] = m1n_2.goal_dir;
            blue_goal_size[1] = m1n_2.goal_size;
      }
      // 後カメラ
      ball_dir[2] = m1n_3.ball_dir;
      ball_dis[2] = m1n_3.ball_dis;
      yellow_goal_dir[2] = 0;
      yellow_goal_size[2] = 0;
      blue_goal_dir[2] = 0;
      blue_goal_size[2] = 0;
      court_dis[2] = m1n_3.court_dis;
      if (m1n_3.is_goal_yellow == 1) {
            yellow_goal_dir[2] = m1n_3.goal_dir;
            yellow_goal_size[2] = m1n_3.goal_size;
      } else {
            blue_goal_dir[2] = m1n_3.goal_dir;
            blue_goal_size[2] = m1n_3.goal_size;
      }
      // 左カメラ
      ball_dir[3] = m1n_4.ball_dir;
      ball_dis[3] = m1n_4.ball_dis;
      yellow_goal_dir[3] = 0;
      yellow_goal_size[3] = 0;
      blue_goal_dir[3] = 0;
      blue_goal_size[3] = 0;
      court_dis[3] = m1n_4.court_dis;
      if (m1n_4.is_goal_yellow == 1) {
            yellow_goal_dir[3] = m1n_4.goal_dir;
            yellow_goal_size[3] = m1n_4.goal_size;
      } else {
            blue_goal_dir[3] = m1n_4.goal_dir;
            blue_goal_size[3] = m1n_4.goal_size;
      }
}