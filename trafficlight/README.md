# Embeddedsignalsystem


## 프로젝트 시연 영상 
[▶ YouTube 영상 보기]
https://www.youtube.com/watch?v=5vl53Ch-KCA

## 아날로그 회로 이미지
[브레드보드 아날로그 회로 이미지]
![Image](https://github.com/user-attachments/assets/fc6dd807-61b5-401b-8be0-af45e79eadca)
[Wokwi 아날로그 회로 이미지]
![Image](https://github.com/user-attachments/assets/ef407d50-382e-436c-b1e7-436d7e044b5f)

### 회로 구성도
기본적으로 아날로그 회로를 만들 때 스위치를 풀업저항으로 구성하였고, 아두이노 코드에서 내부풀업을 사용할 수 있도록 만듦.
3개의 LED의 경우 아두이노 우노 보드 기준으로 디지털 9,10,11번 핀이 PWM을 사용할 수 있기에 가변 저항으로 부터 오는 신호를 PWM 변조를 하기 위해서 9,10,11번핀에 각각 위치 시켰고, 스위치의 경우 우노 보드 기준으로 2,3번 핀은 하드웨어적인 인터럽트 처리가 가능하지만 4번핀은 소프트웨어 인터럽트를 사용 해야 하기에, 스위치 1번(emergency mode)과 스위치 2번(caution mode)은 각각 디지털 2,3번 핀에 위치 시켰고 스위치3번(blink mode)의 경우 디지털 4번 핀에 위치시킴.

## 코드 

### 아두이노 코드

#### 아두이노 코드 이미지 첨부
![Image](https://github.com/user-attachments/assets/d220eb03-1f57-4630-a9ab-28c061fcaad4)
![Image](https://github.com/user-attachments/assets/2148074b-ac39-456e-bfc2-30e338a8f05f)
![Image](https://github.com/user-attachments/assets/923cb999-72f3-40e5-b17b-6def9a591bda)
![Image](https://github.com/user-attachments/assets/3094f6f8-507e-4e5e-8e80-e439722741d0)
![Image](https://github.com/user-attachments/assets/5a1cd493-fe65-40e0-987e-3bb037151040)
![Image](https://github.com/user-attachments/assets/a394679f-f647-4c71-9591-7429c030a145)
![Image](https://github.com/user-attachments/assets/66d6c8d1-e97e-4227-95b2-fa5101b161e7)

#### 아두이노 코드 해석
본 프로젝트는 비동기적인 신호등 시스템을 만드는 것이기에 delay()함수를 써서 cpu가 불필요한 동작 혹은 인터럽트 수행 시에 원활한 인터럽트 서비스 루틴 함수로 넘어가지 못하는 것을 막기위해서 taskscheduler 라이브러르를 사용하여 비동기적인 신호등 시스템을 구현하였다.

기본 동작은 각각의 task로 나누어 자신의 task가 끝나면 그 다음에 오는 task가 와서 동작할 수 있게 하였고, task들이 진행중에 인터럽트가 생긴다면 즉각적으로 인터럽트 서비스 루틴으로 넘어가 각각의 모드를 실행 할 수 있도록 하였다.

또한 위에서 언급한 바와 같이 아두이노 우노 보드 기준으로 하드웨어적인 인터럽트를 처리할 수 있는 디지털 2,3번핀 밖에 사용을 하지 못하여 스위치 3번의 경우 소프트웨어 인터럽트를 처리하기 위해서 pinchangeinterrupt 라이브러리를 사용하여 인터럽트를 처리할 수 있게 하였다.

추가적으로 아날로그 회로에서 스위치를 눌렀을 때 모드가 변경이 되는데 시리얼 통신을 통한 데이터 송수신으로 모드의 변경을 p5의 웹상에서 볼 수 있게 하였고, p5에서 슬라이더를 통하여 주기를 변경함을 인지하고 실제적인 아날로그 회로에서 각각의 LED가 변경 될 수 있도록 하였다.

### p5 코드

#### p5 코드 이미지 
![Image](https://github.com/user-attachments/assets/81d76b44-83ca-414d-a569-aaafde397bfc)
![Image](https://github.com/user-attachments/assets/8fa7cf46-3a02-4172-bd15-3d72ba1de866)
![Image](https://github.com/user-attachments/assets/9cd61326-1308-4b1b-b42e-255dd48efef5)
![Image](https://github.com/user-attachments/assets/9ef648e2-d782-49a7-a118-8c0e0aa6fe97)

#### p5 코드 설명
기본적으로 아두이노와 p5는 시리얼 통신을 지원하기 때문에 board rate(9600)으로 맞추어 주었고 시리얼 통신을 통해서 p5에서 슬라이더를 통해서 주기의 값이 변경되었을 때 아두이노에서 LED의 주기를 변경 시켜줄 수 있게 그리고 변경된 값을 다시 p5웹상에서 UI를 통하여 주기의 변화를 알 수 있게 구성하였다. 추가적으로 아두이노 회로에서 모드의 변경이 있을 경우 p5웹상에서 모드의 변경을 가시적으로 볼 수 있게 구성을 하였다.

웹상에는 아두이노와 연결 할 수 있는 버튼을 생성하여 아두이노와 시리얼 통신을 할 수 있게 하였다.
또한, 네가지 동작을 통하여 아두이노에서 동작을 웹상에서 가시적으로 불 수 있게 하였다.
1. 가변저항 조작을 웹 상에서 볼 수 있게 함.
2. 각각의 모드가 무슨 모드인지 볼 수 있게 함.(기본동작, 비상모드, 위험 모드, 글로벌 깜빡임 모드)
3. 슬라이더를 이용하여 led의 주기를 변경할 수 있게 함.(실제적으로 led의 주기를 볼 수 있게 Task: RedLEd 2700ms와 같이 볼 UI로 볼 수 있게 함)
4. 현재 각각의 무슨 task를 하고 있는지 보이게 함.(task1, task2...task5) + 주기를 나타냄.
