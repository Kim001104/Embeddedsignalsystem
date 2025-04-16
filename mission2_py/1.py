from gpiozero import LED, Button
from signal import pause

# GPIO 핀 연결
led = LED(20)
button = Button(25, pull_up=True)  # 내부 풀업 사용

# 버튼이 눌리면 LED 켜고, 놓으면 끄기
button.when_pressed = led.on
button.when_released = led.off

print("버튼을 누르면 LED가 켜지고, 떼면 꺼집니다.")
pause()  # 무한 대기 (Ctrl+C로 종료)


