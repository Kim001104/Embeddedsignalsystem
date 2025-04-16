from gpiozero import LED, Button
from signal import pause
import time

# 핀 번호 설정
led = LED(20)
button = Button(25, pull_up=True)  # 내부 Pull-Up 사용

# 초기 LED 상태
led_state = False

def toggle_led():
    global led_state
    if led_state:
        led.off()
        led_state = False
    else:
        led.on()
        led_state = True

# 버튼이 눌리는 순간만 감지 (하강 에지)
button.when_pressed = toggle_led

print("버튼을 누를 때마다 LED가 토글됩니다.")
pause()  # 무한 대기 (Ctrl+C로 종료)
