# "RC Car Autonomous Driving Project"


## 🏁 1. 프로젝트 소개 (Introduction)
이 프로젝트는 Raspberry Pi 5와 Arduino UNO를 기반으로 한 RC카 자율주행 시스템입니다.

카메라를 통해 라인을 실시간으로 인식하고, 아두이노가 해당 정보를 받아 차량의 방향과 속도를 제어함으로써, RC카가 경로를 스스로 따라 주행할 수 있도록 설계되었습니다.

사용자는 조종기의 CH7 스위치를 통해 수동 주행과 자율 주행 모드 간의 전환이 가능하며, 라인을 놓치는 경우에도 후진 및 복귀 알고리즘을 통해 경로를 다시 찾도록 구현되어 있습니다.

이 프로젝트는 임베디드 시스템 개발, 컴퓨터 비전 처리, 하드웨어 제어, 실시간 통신의 통합적 학습을 목표로 하고 있습니다.

---


## 2. 자율주행 데모 영상 (Demo Video)  

> 📌 여기에 유튜브 링크 또는 gif 첨부 예정

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

---

이 프로젝트는 단순 라인트레이싱을 넘어서, 실제 주행 도중 발생할 수 있는 다양한 상황(라인 미검출, 곡선, 정지 등)을 고려한 제어 로직까지 포함하고 있어, 임베디드 자율주행 시스템의 기초를 실습하기에 적합한 구조를 가집니다.

---

## 4. 전체 시스템 구성도 (System Architecture)

📌 시스템 블록 다이어그램 추가 예정 (예: 카메라 → Pi 처리 → Arduino 신호 → 서보/ESC)

---


## 5. 사용 기술 스택 (Tech Stack)

Raspberry Pi 5

Arduino UNO

PiCamera2

Python (OpenCV, asyncio, websockets)

C++ (Arduino IDE)

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


## 7. 소프트웨어 구성 (Software Architecture)

stream_ws_bidir.py: PiCamera2 영상 캡처 + 라인 중심 인식 + WebSocket 전송 + Arduino 시리얼 통신

main.ino: PWM 제어 기반 자율주행 로직 + 후진 및 지그재그 주행 알고리즘

---


## 8. 라즈베리파이 & 아두이노 역할 (Raspberry Pi & Arduino Integration)

Pi: 실시간 영상 처리 및 라인 중심 계산

Arduino: 수신된 중심값 기반 주행 방향 및 속도 제어

---


## 9. 라인 검출 및 자율주행 알고리즘 설명 (Line Detection & Driving Logic)

Otsu Threshold + Morphology 연산으로 라인 검출

최대 컨투어 중심값 계산 후 좌우 편차 기반 steering

Zigzag 주행 모드 + 후진 복구 기능 내장

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

