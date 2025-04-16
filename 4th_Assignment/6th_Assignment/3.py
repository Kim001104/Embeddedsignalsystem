from gpiozero import LED, Button
from signal import pause
from time import sleep

# 사용할 LED 핀 번호 (LSB → MSB)
pins = [8, 7, 16, 20]
leds = [LED(pin) for pin in pins]

# 버튼: GPIO 25번, bounce_time = 0.3초 (300ms)
button = Button(25, bounce_time=0.3)

# 초기화
for led in leds:
    led.off()

# 도미노 1회 실행
def run_domino():
    print("버튼 눌림: 도미노 1회 실행")
    for led in leds:
        led.on()
        sleep(1)
        led.off()

button.when_pressed = run_domino

pause()
