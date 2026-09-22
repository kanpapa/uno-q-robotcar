/*
 *   Uno Q Robot Car -- MCUスケッチ
 */

#include <Arduino_RouterBridge.h>

// DRV8833 ピン設定
const int MOTOR_L_IN1 = 3;  // D3 (PWM)
const int MOTOR_L_IN2 = 9;  // D9 (PWM)
const int MOTOR_R_IN1 = 10; // D10 (PWM)
const int MOTOR_R_IN2 = 11; // D11 (PWM)

const int DRIVE_SPEED = 200;

// 前進（LED点灯: 赤）
void moveForward() {
  analogWrite(MOTOR_L_IN1, DRIVE_SPEED);
  analogWrite(MOTOR_L_IN2, 0);
  analogWrite(MOTOR_R_IN1, DRIVE_SPEED);
  analogWrite(MOTOR_R_IN2, 0);

  // アクティブLOW（LOWで点灯）
  digitalWrite(LED3_R, LOW);
  digitalWrite(LED3_B, HIGH);
  digitalWrite(LED4_R, LOW);
  digitalWrite(LED4_B, HIGH);
}

// 停止（LED消灯）
void stopMotors() {
  analogWrite(MOTOR_L_IN1, 0);
  analogWrite(MOTOR_L_IN2, 0);
  analogWrite(MOTOR_R_IN1, 0);
  analogWrite(MOTOR_R_IN2, 0);

  digitalWrite(LED3_R, HIGH);
  digitalWrite(LED3_B, HIGH);
  digitalWrite(LED4_R, HIGH);
  digitalWrite(LED4_B, HIGH);
}

// RPC受信関数: 呼び出されたタイミングで即座に出力反映
String rpc_motor_state(int state) {
  if (state > 0) {
    moveForward();
  } else {
    stopMotors();
  }
  return "{\"ok\":true}";
}

void setup() {
  pinMode(MOTOR_L_IN1, OUTPUT);
  pinMode(MOTOR_L_IN2, OUTPUT);
  pinMode(MOTOR_R_IN1, OUTPUT);
  pinMode(MOTOR_R_IN2, OUTPUT);

  pinMode(LED3_R, OUTPUT);
  pinMode(LED3_B, OUTPUT);
  pinMode(LED4_R, OUTPUT);
  pinMode(LED4_B, OUTPUT);

  stopMotors();

  // Bridge 通信の開始とRPC登録
  Bridge.begin();
  Bridge.provide("motor_state", rpc_motor_state);
}

void loop() {
  // RPCディスパッチ処理などのために短いウェイト
  delay(10);
}