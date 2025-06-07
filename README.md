# "RC Car Autonomous Driving Project"


## 🏁 1. 프로젝트 소개 (Introduction)
이 프로젝트는 Raspberry Pi 5와 Arduino UNO를 기반으로 한 RC카 자율주행 시스템입니다.

카메라를 통해 라인을 실시간으로 인식하고, 아두이노가 해당 정보를 받아 차량의 방향과 속도를 제어함으로써, RC카가 경로를 스스로 따라 주행할 수 있도록 설계되었습니다.

사용자는 조종기의 CH7 스위치를 통해 수동 주행과 자율 주행 모드 간의 전환이 가능하며, 라인을 놓치는 경우에도 후진 및 복귀 알고리즘을 통해 경로를 다시 찾도록 구현되어 있습니다.

이 프로젝트는 임베디드 시스템 개발, 컴퓨터 비전 처리, 하드웨어 제어, 실시간 통신의 통합적 학습을 목표로 하고 있습니다.

---


## 2. 자율주행 데모 영상 (Demo Video)  
[
https://imgur.com/a/JJLAdPA](https://i.imgur.com/9A8NuqC.mp4)
---

## 🎯 3. 프로젝트 개요 및 목표 (Overview & Objectives)

이 프로젝트는 RC카에 자율주행 기능을 탑재하는 것을 목표로 하며, 다음과 같은 구조로 구성되어 있습니다.

---

### 📌 전체 시스템 구성

- **Raspberry Pi 5**  
  - PiCamera2로 실시간 영상 촬영  
  - OpenCV 기반 라인 인식 알고리즘 적용  
  - 라인 중심 좌표를 계산하여 시리얼로 아두이노에 전송  
  - WebSocket을 통해 실시간 영상도 전송 가능

- **Arduino UNO**  
  - Raspberry Pi로부터 수신한 라인 중심값을 바탕으로 PWM 신호 생성  
  - 서보모터(조향)와 ESC(속도) 제어  
  - CH7 채널 입력을 통해 수동/자동 주행 모드 전환  
  - 자율주행 중 라인 손실 시 후진 및 복귀 로직 수행  
  - 지그재그 주행 알고리즘 내장

---

### 🎯 세부 목표

- 라인을 안정적으로 인식하기 위한 영상 전처리 및 중심좌표 검출
- 실시간 라인 중심값 기반의 PD 제어 조향 알고리즘 구현
- 라인 손실 상황에서 후진 → 복귀 로직을 통해 자동 복원
- 주행 중 좌/우 방향 표시용 LED 연동
- 수동/자율 모드 전환 시 시스템 상태 유지 및 안전 제어


이 프로젝트는 단순 라인트레이싱을 넘어서, 실제 주행 도중 발생할 수 있는 다양한 상황(라인 미검출, 곡선, 정지 등)을 고려한 제어 알고리즘을 설계하고 적용하였습니다.

---

## 4. 전체 시스템 구성도 (System Architecture)

📌 시스템 블록 다이어그램 추가 예정 (예: 카메라 → Pi 처리 → Arduino 신호 → 서보/ESC)

---


## 🧰 5. 사용 기술 스택 (Tech Stack)

이 프로젝트는 하드웨어와 소프트웨어가 긴밀하게 통신하는 구조로, 아래와 같은 기술이 사용되었습니다.

---

### 🧠 시스템 구성 요소

| 구성 | 기술 스택 |
|------|-----------|
| **주행 제어** | Arduino UNO, C++, Servo 라이브러리, PinChangeInterrupt |
| **영상 처리** | Raspberry Pi 5, PiCamera2, Python, OpenCV |
| **통신** | USB 시리얼 (PySerial), WebSocket (Python websockets) |
| **실시간 스트리밍** | OpenCV → WebSocket 전송 (브라우저 수신) |

---

### 📝 사용된 언어 및 주요 라이브러리

#### 🔹 Raspberry Pi (Python)
- `cv2` (OpenCV): 영상 처리 및 라인 인식
- `asyncio`: 비동기 프레임 처리 및 서버 제어
- `serial`: 아두이노와 시리얼 통신
- `websockets`: 클라이언트에 영상 전송 (WebSocket 서버)

#### 🔹 Arduino UNO (C++)
- `Servo.h`: 서보 및 ESC 제어
- `PinChangeInterrupt.h`: 수신 PWM 신호 인터럽트 처리
- 사용자 정의 알고리즘: 후진 복귀, PD 제어, 지그재그 주행

---

### 🧪 개발 환경

- Arduino IDE (C++)
- Raspberry Pi OS (64-bit)
- Python 3.9+

---


## 6. 하드웨어 구성 (Hardware Components)

RC카 섀시 + 바퀴

Raspberry Pi 5

Pi Camera Module

Arduino UNO

ESC + 서보모터

LED (좌/우 깜빡이)

배터리 팩, 점퍼케이블 등


---


## 🧩 7. 소프트웨어 구성 (Software Architecture)

이 프로젝트는 Raspberry Pi와 Arduino가 역할을 분담하며 동작하는 구조로, 소프트웨어는 다음과 같이 구성되어 있습니다.

---

### 🔹 Raspberry Pi (Python)

**📁 stream_ws_bidir.py**

- `PiCamera2`로 실시간 영상 캡처
- `OpenCV`를 이용한 라인 중심 검출
- 라인 중심 좌표를 **USB 시리얼**을 통해 아두이노로 전송
- 영상 프레임을 WebSocket으로 전송하여 클라이언트에 실시간 스트리밍 제공

> 💡 핵심 기능: 비전 처리 + 통신 + 웹 영상 송출

---

### 🔹 Arduino UNO (C++)

**📁 RC_arduino.ino**

- Raspberry Pi로부터 **라인 중심 좌표** 수신
- PD 제어 및 지그재그 주행 알고리즘으로 **조향 및 속도 제어**
- **라인 미검출 시 후진 복귀 로직** 수행
- CH7 스위치를 통해 **수동/자율 모드 전환**
- 주행 방향에 따라 **좌/우 깜빡이 LED 제어**
- **인터럽트 기반 PWM 측정**으로 신속한 RC 입력 감지

> 💡 핵심 기능: 자율주행 로직 + 모드 전환 + 하드웨어 제어

---


## 8. 라즈베리파이 & 아두이노 역할 (Raspberry Pi & Arduino Integration)

Pi: 실시간 영상 처리 및 라인 중심 계산

Arduino: 수신된 중심값 기반 주행 방향 및 속도 제어

---


## 🛣️ 9. 라인 검출 및 자율주행 알고리즘 설명 (Line Detection & Driving Logic)

이 프로젝트는 PiCamera2를 통해 실시간으로 영상을 캡처하고, OpenCV를 이용해 라인을 인식합니다. 인식된 라인의 중심 좌표를 바탕으로 Arduino가 조향(PWM)과 속도를 제어합니다.

---

### 🎥 라인 검출 알고리즘 (Python - Raspberry Pi)

`stream_ws_bidir.py` 내 `detect_line_center()` 함수에서 다음과 같은 과정을 거칩니다:

1. **Grayscale 변환**  
   → 색상 정보 제거로 라인 대비 강조

2. **Gaussian Blur 적용**  
   → 반사광/노이즈 완화

3. **Otsu Thresholding**  
   → 자동 임계값 이진화 (흰 배경 + 검은 라인 강조)

4. **Morphological 연산 (열기/닫기)**  
   → 노이즈 제거 및 끊어진 라인 복원

5. **ROI 설정**  
   → 하단 40% 영역만 분석하여 처리 속도 및 정확도 향상

6. **Contour 분석**  
   → 가장 넓은 컨투어를 기준으로 라인 중심 좌표(cx) 계산

7. **중심값 시리얼 전송**  
   → `"{cx}\n"` 형식으로 아두이노에 실시간 전송

---

### 🚗 자율주행 제어 알고리즘 (C++ - Arduino)

`RC_arduino.ino`에서 다음과 같은 로직으로 주행을 제어합니다:

#### 1. **PD 제어 (중심 오차 기반)**  
- `error = 320 - rawCenter`  
- `control = Kp * error + Kd * (이전 오차 변화량)`  
- 계산된 control 값으로 **PWM 조향값 조절**

#### 2. **지그재그 주행 알고리즘**
- 중심 오차가 일정 이하이면 직진 판단
- 좌/우로 100μs씩 주기적으로 방향을 토글하여 **지그재그로 미세조정**

#### 3. **후진 복귀 로직**
- 중심값이 일정 시간 동안 `-1`이면 **라인 상실 판단**
- 일정 시간 후 **후진 시작 → 라인 재인식 → 전진 복귀**

#### 4. **LED 방향등 연동**
- 좌회전 시 왼쪽 LED, 우회전 시 오른쪽 LED 점등
- 후진 시 양쪽 LED 점등

---

이러한 알고리즘은 단순 추종을 넘어서, 곡선 주행, 라인 손실, 경로 복구 등 **다양한 실제 상황에 대응할 수 있도록 설계**되어 있습니다.

---


## 10. 코드 설명 (Key Code Explanation)

📌 stream_ws_bidir.py, main.ino 에서 주요 함수와 흐름 요약 설명 추가 예정


---


## 11. 문제 해결 및 시행착오 기록 (Challenges & Iterative Improvements)

반사광으로 인한 오인식 → Gaussian Blur + Otsu 적용

라인 미검출 상황 → 후진 로직 추가

수동/자율 전환 지연 → CH7 인터럽트 기반 스위치 처리

PWM 정확도 불안정 → 인터럽트 기반 측정 개선

---


## 12. 팀원 소개 및 기여도 (Team & Contributions)

이름

역할

주요 기여

A

영상처리, Python

라인 인식, WebSocket 구현

B

Arduino 제어

PWM 제어, 후진 로직

---


## 13. 향후 계획 (Future Plans)

Object Detection 기능 추가 (신호등, 장애물)

OTA 업데이트 기능 구현

앱과 연동한 수동 제어 시스템 개발

---


## 14. 마무리 및 회고 (Conclusion & Retrospective)

팀워크를 통한 문제 해결 경험

실시간 시스템 동기화의 어려움과 개선 방향 인식

추후 다양한 AI 기술 접목 가능성 확인

