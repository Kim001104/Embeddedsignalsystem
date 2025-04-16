# 6주차 과제

## 유투브 링크 
https://www.youtube.com/shorts/-7hk8cr83CY

## GPIO 핀 설정
SW : GPIO 25번
LEDS : GPIO 8,7,16,20

## 핀 배치도 사진
![Image](https://github.com/user-attachments/assets/7292964e-7f2e-46a2-9c1d-f2565a865d24)

## 파이썬 과제 설명
gpiozero 라이브러리를 사용해서 간소화한 파이썬 코드를 통해서 각각 1,2,3,4번의 문제를 실행 할 수 있도록 함.

1. 버튼을 누르고 있는 동안 led on 떼면 off
2. 버튼을 한번 누를 때 마다 상태가 변함.(on->off , off->on)
3. 버튼을 한번 누르면 4개의 led가 순차적으로 켜지게 함.
4. 버튼을 누를 때마다 4비트 2진 카운터 값을 증가 시키며 led를 통해서 1(001)~15(111)까지 표현할수 있도록 함.
