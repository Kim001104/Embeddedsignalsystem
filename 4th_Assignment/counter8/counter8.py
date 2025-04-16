from gpiozero import LED
import time
import signal
import sys

# 사용할 GPIO 핀 번호 (LSB → MSB 순서)
pins = [8, 7, 16, 20]

# LED 객체 생성
leds = [LED(pin) for pin in pins]

# 시그널 핸들러 (Ctrl+C 또는 Ctrl+Z 시 LED 끄고 종료)
def handle_exit(signum, frame):
    print(f"\n시그널 {signum} 감지됨. 종료 중...")
    for led in leds:
        led.off()
    sys.exit(0)

# 시그널 등록
signal.signal(signal.SIGINT, handle_exit)   # Ctrl+C
signal.signal(signal.SIGTSTP, handle_exit)  # Ctrl+Z

# 숫자를 4비트로 표현해서 LED 점등
def display_binary(value):
    for i in range(4):
        if (value >> i) & 1:
            leds[i].on()
        else:
            leds[i].off()

# 카운터 루프 (0 ~ 15 반복)
try:
    count = 0
    while True:
        display_binary(count)
        print(f"카운트: {count}")
        time.sleep(1)
        count = (count + 1) % 16
except KeyboardInterrupt:
    handle_exit(signal.SIGINT, None)
