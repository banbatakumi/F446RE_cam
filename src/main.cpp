#include "moving_ave.h"
#include "simplify_deg.h"

#define CAM_QTY 4
#define CAM_UART_SPEED 115200

// UART通信定義 (TX, RX)
RawSerial mainSerial(PC_10, PC_11);
RawSerial cam1Serial(PA_2, PA_3);
RawSerial cam2Serial(PC_12, PD_2);
RawSerial cam3Serial(PA_9, PA_10);
RawSerial cam4Serial(PC_6, PC_7);

MovingAve ballDirAve;
MovingAve ballDisAve;
MovingAve yGoalDirAve;
MovingAve yGoalSizeAve;
MovingAve bGoalDirAve;
MovingAve bGoalSizeAve;

// 関数定義
void MainMcu();
void Cam1();
void Cam2();
void Cam3();
void Cam4();

void BallConversion();
void YGoalConversion();
void BGoalConversion();

// ピン定義
DigitalOut led[4] = {PB_10, PB_2, PB_1, PB_0};

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
int16_t rslt_ball_dis;
int16_t rslt_y_goal_dir;
int16_t rslt_y_goal_size;
int16_t rslt_b_goal_dir;
int16_t rslt_b_goal_size;

int8_t ball_th[4] = {29, 74, 16, 65};

int8_t y_goal_th[4] = {-20, 21, 30, 76};

int8_t b_goal_th[4] = {26, 78, -98, -51};

uint8_t send_byte_num = 15;
uint8_t send_byte[15];
bool do_th_send = 1;
uint8_t mode = 0;

Timer thSendTimer;

void setup() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      mainSerial.baud(115200);
      // mainSerial.attach(&MainMcu);
      cam1Serial.baud(CAM_UART_SPEED);
      cam1Serial.attach(&Cam1);
      cam2Serial.baud(CAM_UART_SPEED);
      cam2Serial.attach(&Cam2);
      cam3Serial.baud(CAM_UART_SPEED);
      cam3Serial.attach(&Cam3);
      cam4Serial.baud(CAM_UART_SPEED);
      cam4Serial.attach(&Cam4);

      ballDirAve.SetLength(5);
      ballDisAve.SetLength(5);
      yGoalDirAve.SetLength(5);
      yGoalSizeAve.SetLength(5);
      bGoalDirAve.SetLength(5);
      bGoalSizeAve.SetLength(5);

      send_byte[0] = 0xFF;
      send_byte[1] = ball_th[0] + 127;
      send_byte[2] = ball_th[1] + 127;
      send_byte[3] = ball_th[2] + 127;
      send_byte[4] = ball_th[3] + 127;
      send_byte[5] = y_goal_th[0] + 127;
      send_byte[6] = y_goal_th[1] + 127;
      send_byte[7] = y_goal_th[2] + 127;
      send_byte[8] = y_goal_th[3] + 127;
      send_byte[9] = b_goal_th[0] + 127;
      send_byte[10] = b_goal_th[1] + 127;
      send_byte[11] = b_goal_th[2] + 127;
      send_byte[12] = b_goal_th[3] + 127;
      send_byte[13] = 0xAA;

      __disable_irq();   // 禁止
      for (int i = 0; i < 4; i++) {
            led[i] = 1;
            wait_us(1000000);
      }
      for (int i = 0; i < 4; i++) {
            led[i] = 0;
      }
      __enable_irq();   // 許可

      thSendTimer.start();
}

int main() {
      setup();
      while (1) {
            if (thSendTimer.read() > 5) {
                  thSendTimer.stop();
                  thSendTimer.reset();
                  do_th_send = 0;
            }

            BallConversion();
            YGoalConversion();
            BGoalConversion();

            // それぞれの値に移動平均をかける
            ballDirAve.Compute(&rslt_ball_dir);
            ballDisAve.Compute(&rslt_ball_dis);
            yGoalDirAve.Compute(&rslt_y_goal_dir);
            yGoalSizeAve.Compute(&rslt_y_goal_size);
            bGoalDirAve.Compute(&rslt_b_goal_dir);
            bGoalSizeAve.Compute(&rslt_b_goal_size);

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
}

void YGoalConversion() {
      uint8_t max_size = 0;
      uint8_t max_size_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {
            if (y_goal_size[i] > max_size) {
                  max_size = y_goal_size[i];
                  max_size_num = i;
            }
      }
      rslt_y_goal_size = max_size;

      if (max_size_num == 0) {
            rslt_y_goal_dir = y_goal_dir[0] - 45;
      } else if (max_size_num == 1) {
            rslt_y_goal_dir = y_goal_dir[1] + 45;
      } else if (max_size_num == 2) {
            rslt_y_goal_dir = y_goal_dir[2] + 135;
      } else if (max_size_num == 3) {
            rslt_y_goal_dir = y_goal_dir[3] + 225;
      }
      rslt_y_goal_dir = SimplifyDeg(rslt_y_goal_dir);
}

void BGoalConversion() {
      uint8_t max_size = 0;
      uint8_t max_size_num = 0;
      for (int i = 0; i < CAM_QTY; i++) {
            if (b_goal_size[i] > max_size) {
                  max_size = b_goal_size[i];
                  max_size_num = i;
            }
      }
      rslt_b_goal_size = max_size;

      if (max_size_num == 0) {
            rslt_b_goal_dir = b_goal_dir[0] - 45;
      } else if (max_size_num == 1) {
            rslt_b_goal_dir = b_goal_dir[1] + 45;
      } else if (max_size_num == 2) {
            rslt_b_goal_dir = b_goal_dir[2] + 135;
      } else if (max_size_num == 3) {
            rslt_b_goal_dir = b_goal_dir[3] + 225;
      }
      rslt_b_goal_dir = SimplifyDeg(rslt_b_goal_dir);
}

void MainMcu() {
      // 送信
      uint8_t send_byte_num = 11;
      uint8_t send_byte[send_byte_num];
      send_byte[0] = 0xFF;
      send_byte[1] = rslt_ball_dir > 0 ? rslt_ball_dir : 0;
      send_byte[2] = rslt_ball_dir < 0 ? rslt_ball_dir * -1 : 0;
      send_byte[3] = rslt_ball_dis;
      send_byte[4] = rslt_y_goal_dir > 0 ? rslt_y_goal_dir : 0;
      send_byte[5] = rslt_y_goal_dir < 0 ? rslt_y_goal_dir * -1 : 0;
      send_byte[6] = rslt_y_goal_size;
      send_byte[7] = rslt_b_goal_dir > 0 ? rslt_b_goal_dir : 0;
      send_byte[8] = rslt_b_goal_dir < 0 ? rslt_b_goal_dir * -1 : 0;
      send_byte[9] = rslt_b_goal_size;
      send_byte[10] = 0xAA;

      for (uint8_t i = 0; i < send_byte_num; i++) {
            mainSerial.putc(send_byte[i]);
      }

      // 受信
      if (mainSerial.readable()) mode = mainSerial.getc();
}

void Cam1() {
      if (cam1Serial.getc() == 0xFF) {
            if (mode == 0) led[0] = 1;

            // 受信
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam1Serial.getc();   // 一旦すべてのデータを格納する
            }

            if (recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
                  ball_dir[0] = recv_byte[0];
                  ball_dis[0] = recv_byte[1];
                  y_goal_dir[0] = recv_byte[2];
                  y_goal_size[0] = recv_byte[3];
                  b_goal_dir[0] = recv_byte[4];
                  b_goal_size[0] = recv_byte[5];
            } else {
                  return;
            }

            // 送信
            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam1Serial.putc(send_byte[i]);
                  }
            }

            led[0] = 0;
      } else {
            return;
      }
}

void Cam2() {
      if (cam2Serial.getc() == 0xFF) {
            if (mode == 0) led[1] = 1;

            // 受信
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam2Serial.getc();   // 一旦すべてのデータを格納する
            }

            if (recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
                  ball_dir[1] = recv_byte[0];
                  ball_dis[1] = recv_byte[1];
                  y_goal_dir[1] = recv_byte[2];
                  y_goal_size[1] = recv_byte[3];
                  b_goal_dir[1] = recv_byte[4];
                  b_goal_size[1] = recv_byte[5];
            } else {
                  return;
            }

            // 送信
            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam2Serial.putc(send_byte[i]);
                  }
            }

            led[1] = 0;
      } else {
            return;
      }
}

void Cam3() {
      if (cam3Serial.getc() == 0xFF) {
            if (mode == 0) led[2] = 1;

            // 受信
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam3Serial.getc();   // 一旦すべてのデータを格納する
            }

            if (recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
                  ball_dir[2] = recv_byte[0];
                  ball_dis[2] = recv_byte[1];
                  y_goal_dir[2] = recv_byte[2];
                  y_goal_size[2] = recv_byte[3];
                  b_goal_dir[2] = recv_byte[4];
                  b_goal_size[2] = recv_byte[5];
            } else {
                  return;
            }

            // 送信
            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam3Serial.putc(send_byte[i]);
                  }
            }

            led[2] = 0;
      } else {
            return;
      }
}

void Cam4() {
      if (cam4Serial.getc() == 0xFF) {
            if (mode == 0) led[3] = 1;

            // 受信
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam4Serial.getc();   // 一旦すべてのデータを格納する
            }

            if (recv_byte[recv_byte_num - 1] == 0xAA) {   // ヘッダーとフッターがあることを確認
                  ball_dir[3] = recv_byte[0];
                  ball_dis[3] = recv_byte[1];
                  y_goal_dir[3] = recv_byte[2];
                  y_goal_size[3] = recv_byte[3];
                  b_goal_dir[3] = recv_byte[4];
                  b_goal_size[3] = recv_byte[5];
            } else {
                  return;
            }

            // 送信
            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam4Serial.putc(send_byte[i]);
                  }
            }

            led[3] = 0;
      } else {
            return;
      }
}