from gpiozero import LED, Button
from time import sleep

# 사용할 LED 핀 번호 (LSB → MSB)
pins = [8, 7, 16, 20]

# LED 객체 생성 및 초기화
leds = [LED(pin) for pin in pins]
for led in leds:
    led.off()  # 초기 상태: 꺼짐

# 도미노 출력
for led in leds:
    led.on()
    sleep(1)
    led.off()
