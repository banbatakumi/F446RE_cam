#include "moving_ave.h"
#include "simplify_deg.h"

#define CAM_QTY 4
#define CAM_UART_SPEED 115200

// UART通信定義 (TX, RX)
RawSerial mainMcu(PC_10, PC_11);
RawSerial cam1(PA_2, PA_3);
RawSerial cam2(PC_12, PD_2);
RawSerial cam3(PA_9, PA_10);
RawSerial cam4(PC_6, PC_7);

MovingAve ballDirAve;
MovingAve ballDisAve;
MovingAve yGoalDirAve;
MovingAve yGoalSizeAve;
MovingAve bGoalDirAve;
MovingAve bGoalSizeAve;

// 関数定義
// void MainMcuRx();
void Cam1Rx();
void Cam2Rx();
void Cam3Rx();
void Cam4Rx();

void BallConversion();
void YGoalConversion();
void BGoalConversion();

// ピン定義
DigitalOut led1(PB_10);
DigitalOut led2(PB_2);
DigitalOut led3(PB_1);
DigitalOut led4(PB_0);

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

int8_t ball_th[4] = {22, 70, 29, 77};

int8_t y_goal_th[4] = {-15, 14, 47, 77};

int8_t b_goal_th[4] = {20, 45, -61, -41};

uint8_t send_byte_num = 15;
uint8_t send_byte[15];
bool do_th_send = 1;

Timer thSendTimer;

int main() {
      // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200
      mainMcu.baud(115200);
      // mainMcu.attach(MainMcuRx, Serial::RxIrq);
      cam1.baud(CAM_UART_SPEED);
      cam1.attach(&Cam1Rx, Serial::RxIrq);
      cam2.baud(CAM_UART_SPEED);
      cam2.attach(&Cam2Rx, Serial::RxIrq);
      cam3.baud(CAM_UART_SPEED);
      cam3.attach(&Cam3Rx, Serial::RxIrq);
      cam4.baud(CAM_UART_SPEED);
      cam4.attach(&Cam4Rx, Serial::RxIrq);

      ballDirAve.SetLength(10);
      ballDisAve.SetLength(10);
      yGoalDirAve.SetLength(10);
      yGoalSizeAve.SetLength(10);
      bGoalDirAve.SetLength(10);
      bGoalSizeAve.SetLength(10);

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

      led1 = 1;
      wait_us(1000000);
      led2 = 1;
      wait_us(1000000);
      led3 = 1;
      wait_us(1000000);
      led4 = 1;
      wait_us(2000000);
      led1 = 0;
      led2 = 0;
      led3 = 0;
      led4 = 0;

      thSendTimer.start();

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
                  mainMcu.putc(send_byte[i]);
            }
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

void Cam1Rx() {
      if (cam1.getc() == 0xFF) {
            led1 = 1;
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam1.getc();   // 一旦すべてのデータを格納する
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

            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam1.putc(send_byte[i]);
                  }
            }

            led1 = 0;
      } else {
            return;
      }
}

void Cam2Rx() {
      if (cam2.getc() == 0xFF) {
            led2 = 1;
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam2.getc();   // 一旦すべてのデータを格納する
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

            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam2.putc(send_byte[i]);
                  }
            }

            led2 = 0;
      } else {
            return;
      }
}

void Cam3Rx() {
      if (cam3.getc() == 0xFF) {
            led3 = 1;
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam3.getc();   // 一旦すべてのデータを格納する
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

            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam3.putc(send_byte[i]);
                  }
            }

            led3 = 0;
      } else {
            return;
      }
}

void Cam4Rx() {
      if (cam4.getc() == 0xFF) {
            led4 = 1;
            const uint8_t recv_byte_num = 7;
            uint8_t recv_byte[recv_byte_num];

            for (int i = 0; i < recv_byte_num; i++) {
                  recv_byte[i] = cam4.getc();   // 一旦すべてのデータを格納する
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

            if (do_th_send == 1) {
                  for (uint8_t i = 0; i < send_byte_num; i++) {
                        cam4.putc(send_byte[i]);
                  }
            }

            led4 = 0;
      } else {
            return;
      }
}