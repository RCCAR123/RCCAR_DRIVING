## RCCAR 자율주행 프로젝트

## 1. 프로젝트 소개 (Introduction)

# 라즈베리 파이와 아두이노를 활용한 RC카 자율주행 프로젝트입니다. 본 프로젝트는 라인을 인식하여 자동으로 경로를 따라 주행하는 RC카를 제작하는 것을 목표로 합니다.

## 2. 자율주행 데모 영상 (Demo Video)

📌 여기에 유튜브 링크 또는 gif 첨부 예정

3. 프로젝트 개요 및 목표 (Overview & Objectives)

라즈베리파이 5 + 파이 카메라로 라인 인식

아두이노 우노로 주행 제어 및 인터럽트 기반 PWM 신호 처리

자율주행 + 수동모드 전환 가능 (CH7 스위치)

4. 전체 시스템 구성도 (System Architecture)

📌 시스템 블록 다이어그램 추가 예정 (예: 카메라 → Pi 처리 → Arduino 신호 → 서보/ESC)

5. 사용 기술 스택 (Tech Stack)

Raspberry Pi 5

Arduino UNO

PiCamera2

Python (OpenCV, asyncio, websockets)

C++ (Arduino IDE)

6. 하드웨어 구성 (Hardware Components)

RC카 섀시 + 바퀴

Raspberry Pi 5

Pi Camera Module

Arduino UNO

ESC + 서보모터

LED (좌/우 깜빡이)

배터리 팩, 점퍼케이블 등

7. 소프트웨어 구성 (Software Architecture)

stream_ws_bidir.py: PiCamera2 영상 캡처 + 라인 중심 인식 + WebSocket 전송 + Arduino 시리얼 통신

main.ino: PWM 제어 기반 자율주행 로직 + 후진 및 지그재그 주행 알고리즘

8. 라즈베리파이 & 아두이노 역할 (Raspberry Pi & Arduino Integration)

Pi: 실시간 영상 처리 및 라인 중심 계산

Arduino: 수신된 중심값 기반 주행 방향 및 속도 제어

9. 라인 검출 및 자율주행 알고리즘 설명 (Line Detection & Driving Logic)

Otsu Threshold + Morphology 연산으로 라인 검출

최대 컨투어 중심값 계산 후 좌우 편차 기반 steering

Zigzag 주행 모드 + 후진 복구 기능 내장

10. 코드 설명 (Key Code Explanation)

📌 stream_ws_bidir.py, main.ino 에서 주요 함수와 흐름 요약 설명 추가 예정

11. 문제 해결 및 시행착오 기록 (Challenges & Iterative Improvements)

반사광으로 인한 오인식 → Gaussian Blur + Otsu 적용

라인 미검출 상황 → 후진 로직 추가

수동/자율 전환 지연 → CH7 인터럽트 기반 스위치 처리

PWM 정확도 불안정 → 인터럽트 기반 측정 개선

12. 팀원 소개 및 기여도 (Team & Contributions)

이름

역할

주요 기여

A

영상처리, Python

라인 인식, WebSocket 구현

B

Arduino 제어

PWM 제어, 후진 로직

13. 향후 계획 (Future Plans)

Object Detection 기능 추가 (신호등, 장애물)

OTA 업데이트 기능 구현

앱과 연동한 수동 제어 시스템 개발

14. 마무리 및 회고 (Conclusion & Retrospective)

팀워크를 통한 문제 해결 경험

실시간 시스템 동기화의 어려움과 개선 방향 인식

추후 다양한 AI 기술 접목 가능성 확인

