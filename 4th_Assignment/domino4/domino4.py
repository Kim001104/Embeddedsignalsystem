from gpiozero import LED
import time
import signal
import sys

# 사용할 LED 핀 번호
pins = [8, 7, 16, 20]
leds = [LED(pin) for pin in pins]

# 시그널 핸들러 정의
def handle_exit(signum, frame):
    print(f"\n시그널 {signum} 감지됨. 프로그램 종료 중...")
    for led in leds:
        led.off()
    sys.exit(0)

# 시그널 처리 등록
signal.signal(signal.SIGINT, handle_exit)   # Ctrl+C
signal.signal(signal.SIGTSTP, handle_exit)  # Ctrl+Z

# LED 도미노 출력 루프
try:
    while True:
        for led in leds:
            led.on()
            time.sleep(1)
            led.off()
except KeyboardInterrupt:
    handle_exit(signal.SIGINT, None)
