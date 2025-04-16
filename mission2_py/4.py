from gpiozero import LED, Button
from time import sleep

# GPIO 설정
button = Button(25, pull_up=True)  # 내부 Pull-Up 사용
led_pins = [8, 7, 16, 20]  # LSB → MSB
leds = [LED(pin) for pin in led_pins]

# 초기화: 모든 LED 끄기
for led in leds:
    led.off()

count = 0
prev_state = button.is_pressed  # 처음 상태 기록

while True:
    curr_state = button.is_pressed

    # 버튼 눌림 감지 (False: 눌림, True: 뗌)
    if prev_state and not curr_state:
        count = (count + 1) % 16  # 4-bit 카운터: 0~15

        # 이진 비트로 LED 제어
        for i in range(4):
            bit = (count >> i) & 1
            if bit == 1:
                leds[i].on()
            else:
                leds[i].off()

    prev_state = curr_state
    sleep(0.05)
# 무한 루프 (Ctrl+C로 종료)
# 버튼이 눌리면 카운트 증가
# 버튼이 떼어지면 카운트 유지
# 카운트는 0~15 (4-bit)
# 각 비트에 해당하는 LED를 켜고 끔
