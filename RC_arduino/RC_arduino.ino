#include <Servo.h>
#include <PinChangeInterrupt.h>

// ─────────────────────────────────────────────
// 핀 설정
const int throttleInPin  = 4;   // RC 스로틀 입력
const int steerInPin     = 3;   // RC 조향 입력
const int throttleOutPin = 6;   // ESC 출력
const int steerOutPin    = 7;   // 서보 출력
const int ch7_pin        = 8;   // CH7 (모드 전환)

const int leftLedPin  = 10;     // 좌측 LED (깜빡이)
const int rightLedPin = 9;      // 우측 LED (깜빡이)

// ─────────────────────────────────────────────
// 기본 설정
const int pwmCenter = 1500;         // 중립 PWM
const float forwardFactor  = 0.2f;  // 수동 전진 계수
const float backwardFactor = 0.2f;  // 수동 후진 계수

// ─────────────────────────────────────────────
// 자율 주행 속도 설정
const int straightThreshold = 20;   // 직진 판정 범위
const int straightSpeed     = 1570; // 직진 속도
const int cornerSpeed       = 1560; // 회전 시 속도
const int reverseSpeed      = 1440; // 후진 속도

// PD 제어 이득
float kp = 1.5;
float kd = 0.3;

// 조향 설정
float steerExpo = 1.0;     // 조향 곡선 강도
int steerRange  = 1500;    // 조향 PWM 범위

// ─────────────────────────────────────────────
// Zigzag 설정
const int zigzagAmplitude = 100;         // 지그재그 폭
const unsigned long zigzagPeriod = 100;  // 지그재그 주기(ms)

// ─────────────────────────────────────────────
// 인터럽트 관련 변수
volatile uint32_t throttleStart = 0, throttlePulse = 0;
volatile uint32_t steerStart    = 0, steerPulse    = 0;
volatile uint32_t ch7Start      = 0, ch7Pulse      = 0;

// ─────────────────────────────────────────────
// 기타 전역 변수
Servo throttleESC;
Servo steerServo;

String serialBuffer = "";
unsigned long lastSerialTime = 0;
unsigned long lastCH7Time    = 0;

int rawCenter      = -1; // 라인트래킹 중심 좌표 (-1: 없음)
int prevError      = 0;  // PD용 이전 오차
unsigned long prevTime = 0; // PD용 이전 시간

bool reversing = false;                   // 후진 중 여부
unsigned long reversingReleaseTime = 0;   // 후진 해제 시간

bool zigzagDir = false;           // 지그재그 방향
unsigned long zigzagLastToggle = 0; // 지그재그 마지막 전환 시간

// ─────────────────────────────────────────────
// 함수 선언
void throttleISR();
void steerISR();
void ch7ISR();

void setup() {
  Serial.begin(9600);
  throttleESC.attach(throttleOutPin);
  steerServo.attach(steerOutPin);

  delay(2000); // ESC 초기화 대기
  throttleESC.writeMicroseconds(pwmCenter);
  steerServo.writeMicroseconds(pwmCenter);

  pinMode(throttleInPin, INPUT);
  pinMode(steerInPin,    INPUT);
  pinMode(ch7_pin,       INPUT);
  pinMode(leftLedPin, OUTPUT);
  pinMode(rightLedPin, OUTPUT);

  attachPCINT(digitalPinToPCINT(throttleInPin), throttleISR, CHANGE);
  attachPCINT(digitalPinToPCINT(steerInPin),    steerISR,   CHANGE);
  attachPCINT(digitalPinToPCINT(ch7_pin),       ch7ISR,     CHANGE);

  prevTime = millis();
  zigzagLastToggle = millis();
}

void loop() {
  // ── 라인트래킹 데이터 수신 ──
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      rawCenter = serialBuffer.toInt();
      lastSerialTime = millis();
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // ── 타임아웃 처리 ──
  if (millis() - lastSerialTime > 10) rawCenter = -1;
  if (millis() - lastCH7Time > 1000)  ch7Pulse = 0;

  bool useSerial = (ch7Pulse > 1700);  // AUTO 모드 판단
  int steerOut = pwmCenter;
  int throttleOut = pwmCenter;

  // 기본 LED OFF
  digitalWrite(leftLedPin, LOW);
  digitalWrite(rightLedPin, LOW);

  if (useSerial) {
    // ── 후진 판단 및 모드 전환 ──
    if (!reversing && rawCenter == -1) reversing = true;
    if (reversing && rawCenter >= 0 && reversingReleaseTime == 0)
      reversingReleaseTime = millis() + 20;
    if (reversing && reversingReleaseTime > 0 && millis() >= reversingReleaseTime) {
      reversing = false;
      reversingReleaseTime = 0;
      prevTime = millis();
    }

    if (reversing) {
      // 후진 시 조향 (이전 에러 반대 방향)
      int reverseOffset = -prevError;
      float norm = float(reverseOffset) / 320.0;
      float curved = copysign(pow(abs(norm), steerExpo), norm);
      steerOut = constrain(pwmCenter + int(curved * steerRange), 1000, 2000);
      throttleOut = reverseSpeed;
      digitalWrite(leftLedPin, HIGH);
      digitalWrite(rightLedPin, HIGH);
    } else if (rawCenter >= 0) {
      // 전진: 직진/PD/지그재그
      int error = 320 - rawCenter;
      unsigned long now = millis();
      float dt = (now - prevTime) / 1000.0;

      if (abs(error) < straightThreshold) {
        if (now - zigzagLastToggle >= zigzagPeriod) {
          zigzagDir = !zigzagDir;
          zigzagLastToggle = now;
        }
        steerOut = pwmCenter + (zigzagDir ? zigzagAmplitude : -zigzagAmplitude);
        throttleOut = straightSpeed;
        if (zigzagDir) digitalWrite(leftLedPin, HIGH);
        else digitalWrite(rightLedPin, HIGH);
      } else {
        float Pout = kp * float(error);
        float derivative = (error - prevError) / (dt > 0 ? dt : 1e-3);
        float Dout = kd * derivative;
        float control = Pout + Dout;

        steerOut = constrain(pwmCenter + int(control), 1000, 2000);
        throttleOut = cornerSpeed;
        if (steerOut > pwmCenter) digitalWrite(leftLedPin, HIGH);
        else if (steerOut < pwmCenter) digitalWrite(rightLedPin, HIGH);
      }
      prevError = error;
      prevTime = now;
    }
  } else {
    // ── 수동 모드 ──
    int tOff = throttlePulse - pwmCenter;
    if (tOff > 50) throttleOut = pwmCenter + int(tOff * forwardFactor);
    else if (tOff < -50) {
      throttleOut = pwmCenter + int(tOff * backwardFactor);
      digitalWrite(leftLedPin, HIGH);
      digitalWrite(rightLedPin, HIGH);
    }

    int sOff = steerPulse - pwmCenter;
    if (abs(sOff) > 30) {
      float norm = float(sOff) / 500.0;
      float curved = copysign(pow(abs(norm), steerExpo), norm);
      steerOut = pwmCenter + int(curved * steerRange);
      if (sOff > 50 && millis() % 500 < 250) digitalWrite(leftLedPin, HIGH);
      else if (sOff < -50 && millis() % 500 < 250) digitalWrite(rightLedPin, HIGH);
    }
  }

  // 출력 적용
  steerOut = constrain(steerOut, 1000, 2000);
  throttleOut = constrain(throttleOut, 1000, 2000);
  steerServo.writeMicroseconds(steerOut);
  throttleESC.writeMicroseconds(throttleOut);

  // 디버그 출력
  Serial.print("MODE: "); Serial.print(useSerial ? "AUTO" : "RC");
  Serial.print(" | rawCenter: "); Serial.print(rawCenter);
  Serial.print(" | Reversing: "); Serial.print(reversing);
  Serial.print(" | ReleaseTime: ");
  Serial.print(reversingReleaseTime > 0 ? (reversingReleaseTime - millis()) : 0);
  Serial.print(" | Steer: "); Serial.print(steerOut);
  Serial.print(" | Throttle: "); Serial.println(throttleOut);

  delay(2); // 루프 주기 2ms
}

// ─────────────────────────────────────────────
// 인터럽트 핸들러
void throttleISR() {
  if (digitalRead(throttleInPin) == HIGH)
    throttleStart = micros();
  else
    throttlePulse = micros() - throttleStart;
}

void steerISR() {
  if (digitalRead(steerInPin) == HIGH)
    steerStart = micros();
  else
    steerPulse = micros() - steerStart;
}

void ch7ISR() {
  if (digitalRead(ch7_pin) == HIGH)
    ch7Start = micros();
  else {
    ch7Pulse = micros() - ch7Start;
    lastCH7Time = millis();
  }
}
