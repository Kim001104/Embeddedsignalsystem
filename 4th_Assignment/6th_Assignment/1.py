from gpiozero import LED, Button
from signal import pause

led = LED(20)
button = Button(25)

button.when_pressed = led.on
button.when_released = led.off

print("버튼 누르면 LED ON, 떼면 OFF")
pause()
