# 3rd Assignment
ml5를 통하여 모션을 인식하여 동작하고 virtual camera를 통하여 화면녹화 및 zoom 송출

## 유투브 링크 
[유투브 링크] https://www.youtube.com/watch?v=cEOIsHahqM0&feature=youtu.be

## 과제 설명
p5의 ml5 카메라에서 각기 다른 총 4가지 이상의 모션을 인식하고 해당 모션에 알맞은 이모지가 웹 화면에 출력이 될 수 있도록 하였고, 오른손 검지로 화면 부분에 그림을 그릴 수 있고 오른손 🖐️ 동작을 통해서 그린 그림들을 지울 수 있도록 하였습니다.
이러한 과정을 many cam virtual camera를 통하여 화면 녹화를 하고 최종적으로 녹화 화면을 zoom으로 송출될 수 있도록 하였습니다.
many cam과 zoom 송출 관련해서는 아래에 이미지를 통하여 첨부하였습니다.


## p5 코드
![Image](https://github.com/user-attachments/assets/d309535a-2e25-4c4a-baf9-790bd4a4e831)
![Image](https://github.com/user-attachments/assets/6200dd67-e2d2-475f-9570-0f103c6d4490)
![Image](https://github.com/user-attachments/assets/4398ce5c-944c-4bd4-91f5-1b676abefbf5)
![Image](https://github.com/user-attachments/assets/c67dc9b0-eea3-4e40-9404-ca01f1268fb6)


## 코드 설명

본 과제에서 왼손과 오른손을 통하여 오른손은 그림을 그릴 수 있고, 지울 수 있게 하였고,
왼손은 총 5가지의 모션 동작을 통해서 모션 동작을 ml5 카메라가 인식을 하고 이모지 함수를 사용하여 이모지가 화면에 나타날 수 있도록 구성하였습니다.

총 5가지의 모션을 취해서 해당 모션에 알맞은 이모티콘이 나올 수 있게 하였습니다.
1. 첫번째 동작으로 활짝핀 🖐️의 동작을 통해서 🖐️이모지가 화면에 나올 수 있게 구성하였습니다
2. 두번째 동작으로 👍 동작을 통해서 👍 이모지가 화면에 나올 수 있게 구성하였습니다.
3. 세번째 동작으로 🤙 동작을 통해서 🤙 이모지가 화면에 나올 수 있도록 하였습니다.
4. 네번째 동작으로 ✌️ 동작을 통해서 ✌️ 이모지가 화면에 나올 수 있도록 하였습니다.
5. 다섯번째 동작으로 ✊ 동작을 통해서 ✊ 이모지가 화면에 나올 수 있도록 하였습니다.

오른손 검지를 사용하여 비디오 화면 있는 부분에서 빨간색 펜을 통해서 그림을 그릴 수 있게 하였고, 그렸던 그림을 다 지우기 위해서 오른손 🖐️ 동작을 통하여 그렸던 그림을 지울 수 있도록 코드를 설정하였습니다.

## manycam 동작시키기
![Image](https://github.com/user-attachments/assets/b2153447-4db5-4194-9bad-44f88a07c0fa)
![Image](https://github.com/user-attachments/assets/4979f9a8-62bd-4f03-848e-c5c7d0572149)

## zoom 화면 송출하기 
![Image](https://github.com/user-attachments/assets/3e253bb8-af12-44ee-bc03-f0d8d1d59947)
![Image](https://github.com/user-attachments/assets/44ffc9e6-5183-46ad-ba27-4be07ffb4742)
![Image](https://github.com/user-attachments/assets/fbfad78d-4331-471b-9add-6dd8af420673)


