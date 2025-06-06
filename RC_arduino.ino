#include <Servo.h>
#include <PinChangeInterrupt.h>

// ── 핀 설정 ───────────────────────────────────────────
const int throttleInPin  = 4;
const int steerInPin     = 3;
const int throttleOutPin = 6;
const int steerOutPin    = 7;
const int ch7_pin        = 8;

const int leftLedPin  = 10;
const int rightLedPin = 9;

// ── 주행 설정 ────────────────────────────────────────
const int pwmCenter = 1500;         // ESC/서보 중립 신호
const float forwardFactor  = 0.2f;  // RC 모드 전진 반응계수
const float backwardFactor = 0.2f;  // RC 모드 후진 반응계수

// 자율 주행 속도 설정
const int straightThreshold = 20;   // error 절댓값이 이내면 직진(또는 zigzag) 속도
const int straightSpeed     = 1570;
const int cornerSpeed       = 1560;

// 후진 속도를 약간만 차이나게 조정해서 너무 멀리 가지 않도록 변경
const int reverseSpeed      = 1440;  

// PD 제어 이득 (전진 모드에서 사용)
float kp = 1.5;
float kd = 0.3;

// Expo 매핑 파라미터 (후진/전진 시 조향 곡률 계산용)
float steerExpo = 1.0;
// steerRange를 늘려 회전 폭 확장
int steerRange = 1500;  

// ── Zigzag 파라미터 ───────────────────────────────────
const int zigzagAmplitude = 100;       // 중앙으로부터 ±100µs 정도 흔들기
const unsigned long zigzagPeriod = 100; // 100ms마다 방향 토글

// ── 인터럽트 변수 ────────────────────────────────────
volatile uint32_t throttleStart = 0, throttlePulse = 0;
volatile uint32_t steerStart    = 0, steerPulse    = 0;
volatile uint32_t ch7Start      = 0, ch7Pulse      = 0;

Servo throttleESC;
Servo steerServo;

String serialBuffer = "";
unsigned long lastSerialTime = 0;
unsigned long lastCH7Time   = 0;

int rawCenter = -1;         // Raspberry Pi로부터 받은 ‘line center’ (실시간 좌표, –1=미검출)
int prevError  = 0;         // PD 미분용 이전 오차
unsigned long prevTime = 0; // PD 미분용 이전 시간

// ── 후진 로직 관련 변수 ──────────────────────────────
bool reversing = false;
unsigned long reversingReleaseTime = 0;  // 선을 다시 인식한 순간 + 20ms 타이머

// ── Zigzag 토글용 ─────────────────────────────────────
bool zigzagDir = false;           // false: 오른쪽 흔들, true: 왼쪽 흔들
unsigned long zigzagLastToggle = 0;

// ── 함수 선언 ────────────────────────────────────────
void throttleISR();
void steerISR();
void ch7ISR();

void setup() {
  Serial.begin(9600);
  throttleESC.attach(throttleOutPin);
  steerServo.attach(steerOutPin);

  // 초기화: ESC/서보 중립 신호 출력
  delay(2000);
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
  // ── 1) Raspberry Pi → rawCenter 읽기 ───────────────────
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

  // ── 2) Pi로부터 일정 시간(10ms) 내에 값이 안 들어오면 rawCenter = -1 ──
  if (millis() - lastSerialTime > 10) {
    rawCenter = -1;
  }

  // CH7 스위치 타임아웃 (1초 이상 신호 없으면 0으로 초기화)
  if (millis() - lastCH7Time > 1000) {
    ch7Pulse = 0;
  }

  bool useSerial = (ch7Pulse > 1700);  // CH7 채널이 1700 이상이면 자율 주행 모드
  int steerOut    = pwmCenter;
  int throttleOut = pwmCenter;

  // AUTO 모드에서도 LED는 필요에 따라 켜 줄 것
  digitalWrite(leftLedPin, LOW);
  digitalWrite(rightLedPin, LOW);

  if (useSerial) {
    // ── (A) 후진 시작 조건 ──
    if (!reversing && rawCenter == -1) {
      reversing = true;
      reversingReleaseTime = 0;  // 선을 못 찾은 최초 시점
    }

    // ── (B) 후진 중 선을 찾은 순간 ──
    if (reversing && rawCenter >= 0 && reversingReleaseTime == 0) {
      reversingReleaseTime = millis() + 20;  // 20ms 뒤 전진
    }

    // ── (C) 20ms 지나면 전진 모드로 전환 ──
    if (reversing && reversingReleaseTime > 0 && millis() >= reversingReleaseTime) {
      reversing = false;
      reversingReleaseTime = 0;
      // 전진 시작 시 PD 제어 초기화
      prevTime = millis();
    }

    // ── (D) reversing 상태에 따라 후진 or 전진 실행 ──
    if (reversing) {
      // (D1) 후진 동작: 이전 error의 반대 방향으로 조향
      int reverseOffset = -prevError;
      float norm   = float(reverseOffset) / 320.0;
      float curved = copysign(pow(abs(norm), steerExpo), norm);
      int steerSignal = pwmCenter + int(curved * steerRange);
      steerSignal = constrain(steerSignal, 1000, 2000);
      steerOut    = steerSignal;
      throttleOut = reverseSpeed;

      // 후진 중: 양쪽 LED 켜기
      digitalWrite(leftLedPin, HIGH);
      digitalWrite(rightLedPin, HIGH);
    }
    else if (rawCenter >= 0) {
      // (D2) 전진 모드 (PD & Zigzag)
      int error = 320 - rawCenter;
      unsigned long now = millis();
      float dt = (now - prevTime) / 1000.0;  // 초 단위

      if (abs(error) < straightThreshold) {
        // Zigzag 모드: 일정 주기마다 좌/우 토글
        if (now - zigzagLastToggle >= zigzagPeriod) {
          zigzagDir       = !zigzagDir;
          zigzagLastToggle = now;
        }
        // 좌우 흔들기(steer만)
        steerOut    = pwmCenter + (zigzagDir ? zigzagAmplitude : -zigzagAmplitude);
        throttleOut = straightSpeed;

        // 지그재그 방향에 따라 LED
        if (zigzagDir) {
          digitalWrite(leftLedPin, HIGH);
        } else {
          digitalWrite(rightLedPin, HIGH);
        }
      }
      else {
        // PD 제어 + 코너 속도
        float Pout       = kp * float(error);
        float derivative = (error - prevError) / (dt > 0 ? dt : 1e-3);
        float Dout       = kd * derivative;
        float control    = Pout + Dout;

        int steerSignal = pwmCenter + int(control);
        steerSignal     = constrain(steerSignal, 1000, 2000);
        steerOut        = steerSignal;
        throttleOut     = cornerSpeed;

        // 조향값이 pwmCenter보다 크면 “왼쪽으로 꺾는 것”으로 보고 좌측 LED 켬
        // 작으면 “오른쪽으로 꺾는 것”으로 보고 우측 LED 켬
        if (steerSignal > pwmCenter) {
          digitalWrite(leftLedPin, HIGH);
        } else if (steerSignal < pwmCenter) {
          digitalWrite(rightLedPin, HIGH);
        }
      }

      prevError = error;
      prevTime  = now;
    }
    // rawCenter < 0이면서 reversing==false인 경우: throttle/steer는 중립 그대로(LED도 모두 꺼짐)
  }
  else {
    // ── 수동(RC) 모드 ──
    int tOff = throttlePulse - pwmCenter;
    if (tOff > 50) {
      throttleOut = pwmCenter + int(tOff * forwardFactor);
    } else if (tOff < -50) {
      throttleOut = pwmCenter + int(tOff * backwardFactor);
      // 수동 후진(양쪽 LED 깜빡이)
      digitalWrite(leftLedPin, HIGH);
      digitalWrite(rightLedPin, HIGH);
    }

    int sOff = steerPulse - pwmCenter;
    if (abs(sOff) > 30) {
      float norm   = float(sOff) / 500.0;
      float curved = copysign(pow(abs(norm), steerExpo), norm);
      steerOut = pwmCenter + int(curved * steerRange);

      // 수동 조향 방향에 따라 LED 깜빡이
      if (sOff > 50) {
        if (millis() % 500 < 250) digitalWrite(leftLedPin, HIGH);
      } else if (sOff < -50) {
        if (millis() % 500 < 250) digitalWrite(rightLedPin, HIGH);
      }
    }
  }

  // ── (E) 출력 제어 신호 제한 및 송신 ──
  steerOut    = constrain(steerOut,    1000, 2000);
  throttleOut = constrain(throttleOut, 1000, 2000);

  steerServo.writeMicroseconds(steerOut);
  throttleESC.writeMicroseconds(throttleOut);

  // ── (F) 디버그 로그 (필요 시 제거) ──
  Serial.print("MODE: ");
  Serial.print(useSerial ? "AUTO" : "RC");
  Serial.print(" | rawCenter: "); Serial.print(rawCenter);
  Serial.print(" | Reversing: ");   Serial.print(reversing);
  Serial.print(" | ReleaseTime: ");
  Serial.print(reversingReleaseTime > 0 ? (reversingReleaseTime - millis()) : 0);
  Serial.print(" | Steer: ");       Serial.print(steerOut);
  Serial.print(" | Throttle: ");    Serial.println(throttleOut);

  // ── 루프 주기 2ms (반응 속도 더 빠르게) ──
  delay(2);
}

// ── 인터럽트 핸들러 ─────────────────────────────────
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
