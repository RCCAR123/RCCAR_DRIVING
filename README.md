# "🏎️ RC Car Autonomous Driving Project"

[![Watch the video](https://img.youtube.com/vi/NpoDqATXl5s/hqdefault.jpg)](https://youtu.be/NpoDqATXl5s)

---

# 목차
📋 목차

[1. 프로젝트 소개 (Introduction)](#1-프로젝트-소개-introduction)  
[2. 자율주행 데모 (Demo)](#2-자율주행-데모-demo)  
[3. 프로젝트 개요 및 목표 (Overview & Objectives)](#3-프로젝트-개요-및-목표-overview--objectives)  
[4. 전체 시스템 구성도 (System Architecture)](#4-전체-시스템-구성도-system-architecture)  
[5. 사용 기술 스택 (Tech Stack)](#5-사용-기술-스택-tech-stack)  
[6. 하드웨어 구성 (Hardware Components)](#6-하드웨어-구성-hardware-components)  
[7. 소프트웨어 구성 (Software Architecture)](#7-소프트웨어-구성-software-architecture)  
[8. 라즈베리파이 & 아두이노 역할 (Raspberry Pi & Arduino Integration)](#8-라즈베리파이--아두이노-역할-raspberry-pi--arduino-integration)  
[9. 이미지 처리 및 라인트레이싱을 위한 제어 방법 (Image Processing & Line Detection)](#9-이미지-처리-및-라인트레이싱을-위한-제어-방법-image-processing--line-detection)  
[10. 핵심 코드 설명 (Key Code Explanation)](#10-핵심-코드-설명-key-code-explanation)  
[11. 문제 해결 및 시행착오 기록 (Challenges & Iterative Improvements)](#11-문제-해결-및-시행착오-기록-challenges--iterative-improvements)  
[12. 팀원 소개 및 기여도 (Team & Contributions)](#12-팀원-소개-및-기여도-team--contributions)  
[13. 향후 계획 (Future Plans)](#13-향후-계획-future-plans)  
[14. 마무리 및 회고 (Conclusion & Retrospective)](#14-마무리-및-회고-conclusion--retrospective)  




# 1. 프로젝트 소개 (Introduction)

![image](https://github.com/user-attachments/assets/b3e743b1-003d-4b0a-a06c-40ddd3d92df5)


이 프로젝트는 Raspberry Pi 5와 Arduino UNO를 기반으로 한 RC카 자율주행 시스템입니다.

카메라를 통해 라인을 실시간으로 인식하고, 아두이노가 해당 정보를 받아 차량의 방향과 속도를 제어함으로써, RC카가 경로를 스스로 따라 주행할 수 있도록 설계되었습니다.

사용자는 조종기의 CH7 스위치를 통해 수동 주행과 자율 주행 모드 간의 전환이 가능하며, 라인을 놓치는 경우에도 후진 및 복귀 알고리즘을 통해 경로를 다시 찾도록 구현되어 있습니다.

또한, 자율주행 모드에서 라인의 편차가 작을 경우, 차량이 좌우로 가볍게 진동하며 주행하는 Zigzag 모드를 적용하여 직진 안정성을 향상시키고, 단조로운 움직임에 따른 트랙 이탈을 방지하도록 구현하였습니다.

이 프로젝트는 임베디드 시스템 개발, 컴퓨터 비전 처리, 하드웨어 제어, 실시간 통신의 통합적 학습으로 정해진 트랙을 자율주행으로 완주하는 것이 목표입니다.


---    
--- 


# 2. 자율주행 데모 (Demo)  
![자율주행 데모](https://i.imgur.com/E7pzR8D.gif)

![중간 주행](https://i.imgur.com/9A8NuqC.gif)

--- 
---  

# 3. 프로젝트 개요 및 목표 (Overview & Objectives)

이 프로젝트는 RC카에 수동주행 기능과 자율주행 기능을 탑재하는 것을 목표로 하며, 방향에 따른 LED 제어를 실시간으로 ON/OFF하는 동작을 구현했습니다.
구조는 다음과 같이 구성되어 있습니다.


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


### 🎯 세부 목표
> 이 프로젝트는 단순 라인트레이싱을 넘어서, 실제 주행 도중 발생할 수 있는 다양한 상황(라인 미검출, 곡선, 정지 등)을 고려한 제어 알고리즘을 설계하고 적용하였습니다.
- 라인을 안정적으로 인식하기 위한 영상 전처리 및 중심좌표 검출  
- 실시간 라인 중심값 기반의 PD 제어 조향 알고리즘 구현  
- 라인 손실 상황에서 후진 → 복귀 로직을 통해 자동 복원  
- 직진 구간에서 미세한 조향 반복을 통한 **지그재그 주행 알고리즘 적용**으로 안정성 향상  
- 주행 중 좌/우 방향 표시용 LED 연동  
- 수동/자율 모드 전환 시 시스템 상태 유지 및 안전 제어




---
--- 

# 4. 전체 시스템 구성도 (System Architecture)

### 📌 시스템 블록 다이어그램


> 이 시스템은 다음과 같은 흐름으로 작동합니다:

1. **PiCamera2**  
   - 실시간으로 트랙(라인)을 촬영하여 Raspberry Pi에 영상 데이터 전달

2. **Raspberry Pi 5**  
   - OpenCV를 이용한 라인 인식 알고리즘 수행  
   - 라인의 중심좌표 계산  
   - 계산된 중심값을 **Serial (UART)**로 Arduino에 전송  
   - 동시에 **WebSocket**을 통해 실시간 영상도 송출 가능

3. **Arduino UNO**  
   - Raspberry Pi로부터 받은 중심값을 바탕으로 **PWM 제어 신호 생성**  
   - 서보모터(조향), ESC(속도) 제어  
   - 라인 손실 시 후진 → 복귀 알고리즘 실행  
   - 수동/자율 모드 전환 및 Zigzag 주행 알고리즘 포함  
   - 좌/우/후진 방향에 따라 LED 제어로 진행 방향 표시


4. **Servo & ESC**  
   - Servo: 좌/우 방향 조향  
   - ESC: 전진/후진 속도 제어


---  
--- 


# 5. 사용 기술 스택 (Tech Stack)

> 이 프로젝트는 하드웨어와 소프트웨어가 긴밀하게 통신하는 구조로, 아래와 같은 기술이 사용되었습니다.

### 🔹 Raspberry Pi 5 (Python 기반)
Python 3.11

Picamera2: 카메라를 통한 실시간 영상 캡처

OpenCV: 라인 중심 검출, 노이즈 제거, 컨투어 기반 객체 인식

asyncio + WebSocket: 실시간 영상 스트리밍 (브라우저 대응)

pyserial: UART 기반 아두이노와 시리얼 통신

### 🔹 Arduino UNO (C++ 기반)
Arduino IDE: 펌웨어 개발 및 업로드

Servo 라이브러리: 서보모터(PWM) 제어

PinChangeInterrupt 라이브러리: PPM 신호 인터럽트 처리

PWM 제어: 조향(서보) 및 속도(ESC) 조절

PD 제어 알고리즘: 라인 중심 오차 기반의 조향 계산

Zigzag 주행 알고리즘: 직진 시 미세 조향 반복을 통한 안정성 확보

라인 손실 복구 로직: 선 미검출 시 후진 후 재진입

LED 제어: 좌/우 방향, 후진 상황 표시용

### 🔹 통신 및 영상 전송
UART Serial 통신: Raspberry Pi → Arduino로 라인 중심 좌표 전송

WebSocket: Raspberry Pi → 웹 클라이언트로 영상 전송

### 🔹 하드웨어 구성 요소
PiCamera2: 640×480 해상도, 프레임 단위 영상 인식

Servo Motor: 좌우 방향 조향 제어

ESC + 브러시리스 모터: 전진/후진 속도 제어

LED: 좌우 및 후진 방향 시각화

### 🔹 개발 및 문서화 도구
Arduino IDE: 펌웨어 작성 및 업로드

VSCode: Raspberry Pi 측 Python 코드 작성 및 관리

GitHub: 코드 버전 관리 및 프로젝트 문서화

Imgur: GIF 기반 시각 자료 업로드 및 README 연동

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
- Python 3.11


---  
--- 


# 6. 하드웨어 구성 (Hardware Components)

| 구성 요소                             |  설명                                                                     
| ------------------------------------- | -------------------------------------------------------                   
| **RC카 프레임 + 바퀴**                | 브러시리스 모터 장착형 RC카 프레임 (서스펜션 포함)                          
| **Raspberry Pi 5**                    | 영상 처리 및 중심 좌표 계산, Arduino로 전송 (UART), WebSocket 영상 스트리밍 
| **Pi Camera Module**                  | 실시간 트랙 인식 (640×480 해상도, Picamera2 사용)                          
| **Arduino UNO**                       | PWM 기반 모터/조향 제어, 후진 복귀 로직, Zigzag 알고리즘, LED 표시          
| **ESC (Electronic Speed Controller)** | 브러시리스 모터 속도 제어 (Arduino PWM 제어 신호 수신)                      
| **서보모터 (Steering Servo)**         | RC카의 좌/우 방향 조향 제어 (PWM 기반)                                      
| **RadioLink AT9 송신기**              | 수동 조작, CH7 스위치를 통한 자율/수동 모드 전환 신호 송신                   
| **RadioLink R9DS 수신기**             | AT9에서 송신된 PWM 신호 수신, 특히 CH7 채널을 Arduino로 전달                
| **LED (좌/우 방향, 후진 표시용)**     | 실시간 방향 조향 및 후진 시 상태 시각화 표시 (디지털 출력 제어)              
| **배터리 팩 (7.4V\~12V)**             | Raspberry Pi 및 모터 시스템에 전원 공급       
| **점퍼 케이블 / 브레드보드**          | 각 하드웨어 간 연결 및 프로토타이핑에 사용                                
| **UART 시리얼 케이블**                | Raspberry Pi ↔ Arduino 간 중심좌표 데이터 송수신 (TX/RX 연결)        



### 🔌 핀 연결표 (Wiring Table)

| 장치 / 역할         | 아두이노 핀 번호 | 연결 대상                      | 설명                              |
|---------------------|------------------|-------------------------------|-----------------------------------|
| 서보모터 (조향)     | D7               | Steering Servo (신호선)       | PWM으로 조향 제어                  |
| ESC (모터 속도)     | D6               | ESC (신호선)                  | PWM으로 모터 속도 제어             |
| 수신기 CH2 (조향)   | D3               | R9DS CH2            | RC 수동 조향 입력 (PWM)           |
| 수신기 CH3 (스로틀) | D4               | R9DS CH3            | RC 수동 가속 입력 (PWM)           |
| 수신기 CH7 (모드)   | D8               | R9DS CH7            | 자율/수동 모드 스위치 입력 (PWM)  |
| 왼쪽 방향 LED       | D10              | LED (+극)                     | 좌회전 시 점등                     |
| 오른쪽 방향 LED     | D9               | LED (+극)                     | 우회전 또는 후진 시 점등          |
| 배터리              | VIN/GND          | 7.4V~12V 배터리 팩            | 전원 공급                         |



### 🖼️ 회로 구성도 (Circuit Diagram)


<img src="https://github.com/user-attachments/assets/aa4922d7-8114-475a-bb79-e73b63f94f1e" width="50%"/>

<img src="https://github.com/user-attachments/assets/6c50ad38-2635-4e4b-afbb-a3ce42ccfabb" width="50%">

![compressed_image2](https://github.com/user-attachments/assets/f1ecae5a-4e0d-4c0b-b115-6355f86344e9)



---
--- 

# 7. 소프트웨어 구성 (Software Architecture)

이 프로젝트는 Raspberry Pi와 Arduino가 역할을 분담하며 동작하는 구조로, 소프트웨어는 다음과 같이 구성되어 있습니다.


### 🔹 Raspberry Pi (Python)

**📁 stream_ws_bidir.py**

- `PiCamera2`로 실시간 영상 캡처
- `OpenCV`를 이용한 라인 중심 검출
- 라인 중심 좌표를 **USB 시리얼**을 통해 아두이노로 전송
- 영상 프레임을 WebSocket으로 전송하여 클라이언트에 실시간 스트리밍 제공

> 💡 핵심 기능: 비전 처리 + 통신 + 웹 영상 송출


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
---


# 8. 라즈베리파이 & 아두이노 역할 (Raspberry Pi & Arduino Integration)

> 자율주행 시스템은 라즈베리파이와 아두이노가 각각의 역할을 분담하여 협력적으로 작동하도록 설계되었습니다.

### 🔹 Raspberry Pi 5
- PiCamera2를 이용해 실시간 영상 캡처
- OpenCV 기반 라인 중심 좌표 검출
- 중심 좌표를 UART 시리얼로 Arduino에 전송
- WebSocket을 통해 실시간 영상 스트리밍 제공

### 🔹 Arduino UNO
- Raspberry Pi로부터 수신한 중심 좌표 기반으로 주행 방향 판단
- 서보모터(PWM)로 조향, ESC로 속도 제어
- PD 제어 및 Zigzag 주행 알고리즘 수행
- 라인 손실 시 후진 → 복귀 알고리즘 실행
- CH7 채널 신호로 수동/자율 주행 전환 판단
- 방향 및 후진 상태에 따른 실시간 LED 점등 제어

## 🔄 라즈베리파이 ↔ 아두이노 간 통신 프로토콜

| 방향                   | 내용                         | 형식                                       |
|------------------------|------------------------------|--------------------------------------------|
| Raspberry Pi → Arduino | 라인 중심 좌표 전송         | 정수형 문자열 + 개행 문자 (`\n`) 구분       |
| Arduino → Raspberry Pi | 없음 (단방향 통신)          | —                                          |

- **예시**:  
  `320\n` → 중심 좌표가 이미지 중간(320px)일 때  
- Arduino는 이 좌표값을 수신하여 **PD 제어 및 주행 알고리즘에 즉시 반영**함

---
---


# 9. 이미지 처리 및 라인트레이싱을 위한 제어 방법 (Image Processing & Line Detection)

> 이 프로젝트는 PiCamera2를 통해 실시간으로 영상을 캡처하고, OpenCV를 이용해 라인을 인식합니다. 인식된 라인의 중심 좌표를 바탕으로 Arduino가 조향(PWM)과 속도를 제어합니다.


### 📷 Raspberry Pi - 이미지 처리 및 라인 검출 알고리즘

> `camera_stream.py` 내 `detect_line_center()` 함수 기준

1. **Grayscale 변환**  
   - RGB → Gray로 변환하여 색상 제거 및 라인 대비 강조

2. **Gaussian Blur 적용**  
   - 반사광 및 이미지 노이즈를 완화

3. **Otsu Thresholding (이진화)**  
   - 자동 임계값 설정으로 검은색 라인을 강조하는 바이너리 이미지 생성

4. **Morphological 연산**  
   - Open/Close 연산을 통해 작은 노이즈 제거 및 끊어진 라인 연결

5. **ROI 설정 (하단 40%)**  
   - 전체 영상 중 하단 40%만 선택하여 분석  
   - 처리 속도 향상 및 검출 정확도 개선

6. **Contour 분석**  
   - 가장 넓은 외곽선(컨투어)을 찾아 해당 중심 x좌표(cx)를 계산  
   - 중심을 찾지 못하면 -1 반환

7. **중심값 시리얼 전송**  
   - `"{cx}\n"` 형식으로 Arduino에 실시간 전송  
   - 예: `"320\n"` → 중심이 화면 중간일 때

---

### 🔁 Raspberry Pi → Arduino 시리얼 통신

- **통신 방식**: UART (문자열 기반)
- **전송 예시**:
320\n → 중심이 중앙일 때
250\n → 왼쪽으로 치우쳤을 때
-1\n → 라인을 검출하지 못한 경우 (생략 가능)



--- 
---



# 10. 핵심 코드 설명 (Key Code Explanation)

## 📌 Arduino 핵심 코드

### 🔁 1) 라인 중심 좌표 수신 (Serial 입력)

🔹Raspberry Pi로부터 직선 중심 좌표를 Serial로 받아 차량 제어의 기준으로 사용합니다.

```cpp
while (Serial.available()) {
  char c = Serial.read();
  if (c == '\n') {
    rawCenter = serialBuffer.toInt();      //  Raspberry Pi로부터 "320\n" 형태의 중심 좌표를 수신
    lastSerialTime = millis();             //  수신 시간 기록
    serialBuffer = "";                     //  버퍼 초기화
  } else {
    serialBuffer += c;                     //  문자 누적
  }
}
```



### 🔁 2) 라인 손실 시 후진 복귀 로직

🔹 라인을 인식하지 못하면 자동으로 후진하고, 라인을 다시 찾으면 일정 시간 뒤 전진 모드로 전환됩니다.

```cpp
if (!reversing && rawCenter == -1)          //  라인을 감지하지 못하면
  reversing = true;                         //    후진 모드로 전환

if (reversing && rawCenter >= 0 && reversingReleaseTime == 0)
  reversingReleaseTime = millis() + 20;     //  라인 재인식 시 지연 후 전진 복귀 예약

if (reversing && reversingReleaseTime > 0 && millis() >= reversingReleaseTime) {
  reversing = false;                        //  전진 모드 복귀
  reversingReleaseTime = 0;                 //  타이머 초기화
  prevTime = millis();                      //  타이밍 기록
}
```



### 🔁 3) 지그재그 직진 보정

🔹오차가 작을 경우 직진으로 간주하고, 좌우 조향을 주기적으로 전환하여 라인 이탈을 방지합니다.

```cpp
if (abs(error) < straightThreshold) {                               //  오차가 작을 경우 직진 간주
  if (now - zigzagLastToggle >= zigzagPeriod) {                      //  전환 주기 도달 시
    zigzagDir = !zigzagDir;                                          //  좌우 방향 전환
    zigzagLastToggle = now;                                          //  시점 업데이트
  }
  steerOut = pwmCenter + (zigzagDir ? zigzagAmplitude : -zigzagAmplitude); //  조향 보정
  throttleOut = straightSpeed;                                      // 직진 속도 유지
}
```



### 🔁 4) PD 제어 기반 조향 보정

🔹 중심 오차와 변화량을 기반으로 조향 PWM을 정밀하게 보정하여 부드럽고 정확한 주행을 구현합니다.

```cpp
float Pout = kp * float(error);                                 //  비례 제어(P)
float derivative = (error - prevError) / (dt > 0 ? dt : 1e-3);   //  변화율 계산(D)
float Dout = kd * derivative;
float control = Pout + Dout;                                     //  최종 제어값

steerOut = constrain(pwmCenter + int(control), 1000, 2000);      //  PWM 범위 내로 조향값 설정
```


---

## 📌 Raspberry 핵심 코드

### 🔁 1) detect_line_center 함수 (단계별 설명)

🔹 반사광과 작은 잡음을 부드럽게 처리하여 이진화 안정성을 높입니다. 

```cpp
Gaussian Blur: blur = cv2.GaussianBlur(gray, (5,5), 0) 
```
___
 
🔹 모폴로지 열림 (Open): 침식→팽창으로 작은 스팟 노이즈를 제거합니다.  

```cpp
open_img = cv2.morphologyEx(binary, cv2.MORPH_OPEN, kernel)
```
___

🔹 모폴로지 닫힘 (Close): 팽창→침식으로 선 내부의 구멍을 메웁니다.

```cpp
clean = cv2.morphologyEx(open_img, cv2.MORPH_CLOSE, kernel)
```
___

🔹 ROI 설정: 하단 40% 영역만 사용하도록 연산 범위를 축소합니다.

```cpp
y0 = int(h * 0.6); roi = clean[y0:h, :]
```
___

🔹 컨투어 검출 & 필터링: 면적 최대 컨투어를 선택하고, 500픽셀 미만은 무시합니다.

```cpp
 contours,_ = cv2.findContours(...); cnt = max(contours, key=cv2.contourArea); if cv2.contourArea(cnt) < 500: return -1,...
```
___

🔹 중심점 계산 & 보정: 컨투어 모멘트로 중심 x좌표를 계산하고 ROI 오프셋을 적용합니다.
 
```cpp
M = cv2.moments(cnt); cx = int(M['m10']/M['m00']) if M['m00'] else -1; cnt[:,0,1] += y0 
```  


  
### 🔁 2) WebSocket 스트리밍 핸들러: stream (단계별 설명)

🔹 카메라 초기화 > Picamera2를 640×480 해상도로 설정 후 스트리밍 준비
```cpp
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={'size':(640,480)}))
picam2.start()
```

___

🔹 프레임 캡처 & RGB 변환 > BGR 프레임을 캡처하고 웹용 RGB 포맷으로 변환
```cpp
frame = picam2.capture_array()
frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
```

___

🔹 라인 검출 & 시각화 > 중심 x 계산 후 ROI를 초록, 컨투어를 빨강으로 표시
```cpp
cx, clean, y0, cnts = detect_line_center(frame)
cv2.rectangle(frame, (0,y0),(frame.shape[1]-1,frame.shape[0]-1),(0,255,0),2)
for c in cnts: cv2.drawContours(frame,[c],-1,(0,0,255),2)
```
___

🔹 Arduino 전송 & 응답 확인
```cpp
if arduino and cx!=-1: arduino.write(f"{cx}
```
___

🔹 예외 처리 & 종료 > 연결 종료 시 루프 탈출 후 카메라와 시리얼 포트 정리 Arduino 전송 & 응답 확인
```cpp
except websockets.exceptions.ConnectionClosed:
    break
finally:
    picam2.close(); arduino and arduino.close()
```
___

### 🔁 3) 메인 함수: main   
🔹asyncio.run(main()): 이벤트 루프 실행

🔹websockets.serve: stream 핸들러 등록

🔹Future()로 종료 방지

```cpp
async def main():
    # 0.0.0.0:8765 에서 WebSocket 대기
    await websockets.serve(stream, '0.0.0.0', 8765)
    print("✅ WebSocket 서버 시작: ws://<RaspberryPi_IP>:8765")
    await asyncio.Future()  # 무한 대기

if __name__ == '__main__':
    asyncio.run(main())
``` 
---

> 자세한 코드 설명은 코드 주석 참고 바랍니다.
---
---  



# 11. 문제 해결 및 시행착오 기록 (Challenges & Iterative Improvements)


### 🔹 [①] 카메라 지연으로 자율주행 실패

- **원인 / 현상**: USB 웹캠 + MJPEG 사용 → 영상 지연 0.5초 이상  
- **해결 조치**: `PiCamera2` + `WebSocket` 기반 스트리밍 적용  
- **결과**: 지연이 0.05초 이하로 감소 → 실시간 주행 가능


### 🔹 [②] 반사광 오인식

- **원인 / 현상**: 밝은 바닥 반사광을 라인으로 오인 → 후진 반복  
- **해결 조치**: `Gaussian Blur(고주파 반사광 엣지 완화)` + `Otsu Thresholding(자동으로 최적 임계값 적용)` 적용  
- **결과**: 반사광 제거 → 인식 정확도 향상

![light](https://github.com/user-attachments/assets/b13aef01-f7f9-4a11-a9ca-50302dfa0b03)



### 🔹 [③] 라인 검출 시 노이즈
- **원인 / 현상**: 반사광·작은 이물질에 의한 점(스팟)이 바이너리 영상에 섞여 컨투어가 끊기거나 잡음으로 검출

- **해결 조치**: 모폴로지 열림/닫힘 연산으로 작은 노이즈(스팟) 제거

- **결과**: 노이즈가 제거돼 깨끗한 바이너리 영상 확보 → 컨투어 검출이 안정화

![image](https://github.com/user-attachments/assets/32fc1695-ad4c-4be9-bfbc-d986b71fcbb4)



### 🔹 [④] 전진 후 바로 후진 불가

- **원인 / 현상**: ESC 기본 모드에선 전진 후 바로 후진 불가 (보호 로직 작동)
- **해결 조치**: RC카 내장 모드 전환 버튼으로 자유 전/후진 모드로 변경
- **결과**: 전진 ↔ 후진 자유로운 전환 가능, 자율주행 중 회복 가능

![mode](https://github.com/user-attachments/assets/cb8f691d-fa5e-4723-b5aa-5c5321dec413)



### 🔹 [⑤] PWM 값 오류

- **원인 / 현상**: PWM 값 부정확해 속도 부족 또는 급발진 → 차체 손상
- **해결 조치**: 인터럽트 기반 `PWM 측정` + 최적값 보정  
- **결과**: 안정적인 속도 제어 가능

<img src="https://github.com/user-attachments/assets/a622392a-3a75-42b0-a4fc-fe2de697828a" width="40%">




### 🔹 [⑥] 직진 중 선 이탈

- **원인 / 현상**: 중심 오차가 작아도 직선 주행 중 선을 놓침  
- **해결 조치**: `Zigzag 알고리즘`으로 미세 조향 보정  
- **결과**: 중심선을 따라 안정적으로 주행 가능

![linetracing_gif](https://github.com/user-attachments/assets/772c3a6e-45e4-44d4-b623-4fa6217a8a12)



### 🔹 [⑦] 곡선 구간 회전 부족

- **원인 / 현상**: 곡선에서 회전 반응이 부족 → 차체 이탈  
- **해결 조치**: `steerRange` 확대 + `Expo 조향 곡선` 적용  
- **결과**: 급커브에서도 안정적인 회전 유지


### 🔹 [⑧] 범퍼 오인식 문제

- **원인 / 현상**: 카메라 하단에 범퍼가 잡혀 라인으로 오인식  
- **해결 조치**: 카메라 위치를 상단으로 조정 후 고정  
- **결과**: 라인만 인식 → 오작동 방지

<img src="https://github.com/user-attachments/assets/37377039-813f-4e88-ad1e-6a22b0e1bb9e" width="50%"/>
<br>
<img src="https://github.com/user-attachments/assets/56ccdb59-1874-4f2f-ad1e-e08480864740" width="50%"/>








---
---



# 12. 팀원 소개 및 기여도 (Team & Contributions)

## 📌 핵심 분담 요약
> 박기주: 인식 알고리즘 설계자 + 상위 자율 로직 설계자
→ Vision 처리, 판단 흐름 설계, 실시간 스트리밍 및 통신 구조 담당

> 황주옥: 하드웨어 제어 책임자 + 제어 로직 작성자
→ PWM 제어, 모드 분기, 시리얼 수신 처리, LED 피드백 및 RC카 실제 구동 담당


### 박기주 (Raspberry Pi / Vision / 상위 로직 담당)
| 기능/내용                        | 상세 설명                                                |
| ---------------------------- | ---------------------------------------------------- |
| **카메라 스트리밍 구현**              | Picamera2 + OpenCV + WebSocket을 이용해 실시간 영상 전송 시스템 구현 |
| **WebSocket 양방향 통신 설계**      | 영상 전송 + 라인 좌표 송수신을 위한 비동기 웹소켓 구조 설계 및 구현             |
| **라인 중심 검출 알고리즘**            | 하단 70% ROI 기반으로 이진화, 윤곽선 검출, 중심좌표 계산 수행              |
| **라인 중심 좌표 시리얼 송신**          | `serial.write(f"{cx}\n")`로 라인 중심 좌표를 Arduino로 전송     |
| **자율주행 판단 로직 설계**            | `straightThreshold`, `zigzag`, `후진→재진입` 판단 흐름 전체 설계  |
| **PWM 반응 계수 설정**             | `forwardFactor`, `backwardFactor` 계수를 실험 기반으로 최적화    |
| **LED 방향 시스템 필요성 제안**        | error 기반 LED 방향 피드백 아이디어 제안                          |
| **조명 환경 실험 및 threshold 테스트** | 다양한 조도 환경에서 라인 인식 정확도 향상을 위한 threshold 실험 반복         |


---  

### 황주옥 (Arduino / 하드웨어 / 하위 제어 담당)
| 기능/내용                   | 상세 설명                                                  |
| ----------------------- | ------------------------------------------------------ |
| **PWM 중심값 설정**          | 기준값 `pwmCenter = 1500` 설정 및 전체 PWM 시스템 기준 마련           |
| **라인 검출 ROI 조정 제안**     | 하단 70% ROI에서 발생한 노이즈 문제를 해결하기 위해 40%로 조정 제안            |
| **라인 좌표 시리얼 수신 및 파싱**   | `Serial.readStringUntil()` 사용하여 Pi로부터 중심좌표 수신 및 정수로 변환 |
| **PWM 출력 로직 구현**        | `writeMicroseconds()`로 서보/ESC에 PWM 신호 적용 (실제 주행 작동 처리) |
| **자율주행 PWM 계산 로직 구현**   | `error` 기반 steer/throttle PWM 계산 알고리즘 작성               |
| **조향 민감도 파라미터 조정**      | `steerRange`, `steerExpo` 등 곡선 조향 반응 제어 파라미터 설정        |
| **CH7 모드 전환 조건 분석**     | 수동/자율 모드 전환을 위한 CH7 입력 신호 범위 기준 분석                     |
| **모드별 PWM 분기 처리 로직 구현** | `manualMode` 여부에 따라 다른 제어 흐름으로 분기하는 로직 구현              |
| **LED 실시간 방향 시스템 구현**   | 조향 및 후진 상황에 따라 좌/우/후진 LED 점등 로직 구현 (과제 필수 항목)          |
| **하드웨어 회로 구성 및 핀 설계**   | ESC, 서보, LED 핀 배치 및 보호회로 포함 전체 하드웨어 설계 구성              |




---  
---


# 13. 향후 계획 (Future Plans)

> 본 프로젝트는 현재 라인트레이싱 기반의 기본 자율주행 기능까지 구현된 상태이며,
다음과 같은 기능 확장을 통해 더 정교하고 실용적인 RC카 자율주행 시스템으로 발전시킬 예정


1. **신호등 및 장애물 인식 기능 추가**  
   - OpenCV 또는 YOLO 기반 객체 검출을 통해 빨간불 정지, 초록불 주행 구현  
   - 초음파/IR 센서를 활용한 전방 장애물 인식 및 자동 정지 기능 적용

2. **앱 또는 노트북 기반 수동 제어 시스템 개발**  
   - WebSocket/Bluetooth 통신으로 앱 UI에서 RC카 조작  
   - 주행 중 수동 개입 및 원격 제어 가능

3. **주행 모드 통합 및 전환 기능 도입**  
   - 수동 / 라인트레이싱 / 객체 인식 기반 모드 구성  
   - CH7 또는 웹 UI를 통한 실시간 모드 전환 구현 예정



--- 
---


# 14. 마무리 및 회고 (Conclusion & Retrospective)

### ▶ 박기주 - 이번 RC_Car 프로젝트를 마무리하며,

이번 프로젝트를 통해 Raspberry Pi와 Arduino 간의 실시간 양방향 통신 구조를 직접 설계하고,  
카메라 기반 인식 결과를 제어에 연결하는 흐름을 구현해볼 수 있었습니다.

특히 시리얼 모니터를 활용해 디버깅하고,  
조정값들을 테스트하며 시스템을 안정화하는 과정을 통해 많은 실전 경험을 얻었습니다.

자율주행에서 필요한 기능들이 무엇인지 직접 생각해보고 그에 따른 모드를 하나씩 살을 붙여가면서 
아직 부족하지만 프로젝트를 성공할 수 있었습니다. 

혼자였으면 어려웠을 기능들도 팀원과의 협업을 통해 하나씩 해결해 나갈 수 있었고,  
실생활에서 사용되는 자율주행 기술이 실제로 얼마나 정교한 판단과 제어를 필요로 하는지 체감할 수 있었습니다.

---

### ▶ 황주옥 - 이번 RC_Car 프로젝트를 마무리하며,
이번 RC 자율주행 프로젝트를 통해 단순한 라인트레이싱을 넘어서,
실제 주행 중 발생할 수 있는 다양한 상황(곡선, 미검출, 후진 등)을 직접 고려하며 제어 로직을 설계해볼 수 있었습니다.

특히 Raspberry Pi에서 실시간으로 영상 처리를 수행하고,
검출된 중심값을 시리얼 통신을 통해 Arduino로 안정적으로 전달하는 구조를 구현하며
하드웨어와 소프트웨어의 연동에 대한 이해를 깊게 할 수 있었습니다.

문제가 생길 때마다 원인을 추적하고 수정하는 과정을 반복하며 시행착오를 많이 겪었지만,
그 과정에서 실력이 가장 많이 성장했다고 느꼈습니다.
혼자가 아닌 팀원과 함께 설계부터 디버깅, 최종 주행까지 완성할 수 있어서 의미가 컸던 프로젝트였습니다.

