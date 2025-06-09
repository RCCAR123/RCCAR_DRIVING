# camera_stream.py
# WebSocket 기반 라인트레이싱 및 시리얼 통신 서버 (Picamera2 -> Arduino -> 브라우저)

import asyncio                          # 비동기 처리(이벤트 루프, sleep 등)
import base64                           # 바이너리 데이터를 Base64로 인코딩
import cv2                              # OpenCV: 영상 처리
import serial                           # pyserial: 아두이노와 시리얼 통신
from picamera2 import Picamera2        # Raspberry Pi 카메라 제어 (libcamera 기반)
import websockets                       # WebSocket 서버 구현

# ─────────────────────────────────────────────────────────────────────────────
# 시리얼 포트 설정
# ─────────────────────────────────────────────────────────────────────────────
try:
    # '/dev/ttyACM0' 포트로 9600bps 시리얼 연결 (아두이노 기본)
    arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    print("✅ 아두이노 연결 완료")
except serial.SerialException as e:
    # 연결 실패 시 예외 처리 후 arduino 객체를 None으로 설정
    print(f"❌ 아두이노 연결 실패: {e}")
    arduino = None


# ─────────────────────────────────────────────────────────────────────────────
# 라인 중심 검출 함수
# 입력: BGR 형식의 프레임 (NumPy 배열)
# 출력: (중심 x좌표, 전처리된 바이너리 영상, ROI 시작 y좌표, 컨투어 리스트)
#    - 중심 x좌표: 검출 실패 시 -1 반환
#    - clean: 노이즈 제거된 바이너리 영상 디버그용
#    - y0: 하단 ROI 시작 위치(전체 높이의 60%)
#    - best_cnts: 검출된 컨투어 리스트 (없으면 빈 리스트)
# ─────────────────────────────────────────────────────────────────────────────
def detect_line_center(frame):
    # 1) 그레이스케일 변환 (BGR -> Gray)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # 2) 가우시안 블러 적용으로 반사광(하이라이트) 엣지 완화
    blur = cv2.GaussianBlur(gray, (5, 5), 0)

    # 3) Otsu 이진화: 자동 임계값 선택, 반전된 바이너리
    #    - cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU
    _, binary = cv2.threshold(
        blur, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU
    )

    # 4) 모폴로지 연산으로 작은 노이즈 제거
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (7, 7))
    clean = cv2.morphologyEx(binary, cv2.MORPH_OPEN, kernel, iterations=1)
    clean = cv2.morphologyEx(clean, cv2.MORPH_CLOSE, kernel, iterations=1)

    # 전체 이미지 크기 얻기
    h, w = clean.shape
    # 하단 40% 영역을 ROI로 설정 (y0: 시작 y좌표)
    y0 = int(h * 0.6)
    roi = clean[y0:h, :]

    # 5) ROI에서 외곽선(컨투어) 검출
    contours, _ = cv2.findContours(
        roi, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
    )
    if not contours:
        # 컨투어 없으면 검출 실패
        return -1, clean, y0, []

    # 6) 가장 넓은 컨투어 선택 (면적 기준)
    cnt = max(contours, key=cv2.contourArea)
    # 면적이 너무 작으면 무시
    if cv2.contourArea(cnt) < 500:
        return -1, clean, y0, []

    # 7) 모멘트 계산으로 중심점(x) 계산
    M = cv2.moments(cnt)
    if M["m00"] == 0:
        # 분모가 0이면 무시
        return -1, clean, y0, []
    cx = int(M["m10"] / M["m00"] )

    # 컨투어 좌표를 원본 프레임 기준으로 이동 (y0 만큼 offset)
    cnt[:, 0, 1] += y0

    # 결과 반환
    return cx, clean, y0, [cnt]


# ─────────────────────────────────────────────────────────────────────────────
# WebSocket 클라이언트 스트리밍 핸들러
# ─────────────────────────────────────────────────────────────────────────────
async def stream(websocket, path=None):
    # Picamera2 초기화 및 설정
    picam2 = Picamera2()
    config = picam2.create_preview_configuration(main={"size": (640, 480)})
    picam2.configure(config)
    picam2.start()
    print("🎥 Picamera2 시작됨")

    try:
        while True:
            # 1) 카메라에서 프레임 획득 (NumPy 배열, BGR)
            frame = picam2.capture_array()
            # 2) RGB로 변환 (자바스크립트 Canvas 표시용)
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            h, w = frame.shape[:2]

            # 3) 라인 중심 검출
            cx, clean, y0, best_cnts = detect_line_center(frame)
            print(f"📍 라인 중심: {cx}")

            # 4) ROI 영역 시각화 (초록 사각형)
            cv2.rectangle(frame, (0, y0), (w - 1, h - 1), (0, 255, 0), 2)

            # 5) 원하는 경우 바이너리 clean 영상을 디버깅용으로 프레임에 덮어쓰기
            # frame[0:clean.shape[0], 0:clean.shape[1], 2] = clean

            # 6) 검출된 컨투어는 빨간 윤곽으로 표시
            for cnt in best_cnts:
                cv2.drawContours(frame, [cnt], -1, (0, 0, 255), 2)

            # 7) Pi -> Arduino: 중심 x좌표 전송 (검출 성공 시)
            if arduino and cx != -1:
                arduino.write(f"{cx}\n".encode())

            # 8) Arduino -> Pi 응답 수신 (옵션: 동작 확인용)
            if arduino and arduino.in_waiting:
                try:
                    resp = arduino.readline().decode().strip()
                    if resp:
                        print(f"🔁 Arduino 응답: {resp}")
                except Exception as e:
                    print(f"⚠️ 응답 파싱 오류: {e}")

            # 9) 프레임을 JPEG로 인코딩 후 Base64로 변환
            ret, jpg = cv2.imencode('.jpg', frame)
            if ret:
                data = base64.b64encode(jpg.tobytes()).decode('utf-8')
                # WebSocket을 통해 클라이언트에 전송
                await websocket.send(data)

            # 10) 약 20fps (50ms) 대기
            await asyncio.sleep(0.05)

    except websockets.exceptions.ConnectionClosed:
        # 클라이언트가 연결 종료하면 루프 탈출
        print("🔌 클라이언트 연결 종료")
    finally:
        # 종료 시 Picamera와 시리얼 포트 정리
        picam2.close()
        if arduino:
            arduino.close()
        print("📷 종료")


# ─────────────────────────────────────────────────────────────────────────────
# 메인 함수: WebSocket 서버 시작
# ─────────────────────────────────────────────────────────────────────────────
async def main():
    # 0.0.0.0:8765 포트에서 WebSocket 서버 대기
    await websockets.serve(stream, '0.0.0.0', 8765)
    print("✅ WebSocket 서버 시작: ws://<RaspberryPi_IP>:8765")
    # 무한 대기
    await asyncio.Future()


if __name__ == '__main__':
    # asyncio 이벤트 루프를 사용해 main() 실행
    asyncio.run(main())
