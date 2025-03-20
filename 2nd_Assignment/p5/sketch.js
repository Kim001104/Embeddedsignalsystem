let port;
let connectBtn, disconnectBtn;
let redSlider, yellowSlider, greenSlider;
let redLabel, yellowLabel, greenLabel;
let brightnessDisplay, modeDisplay;
let redTime = 2000;
let yellowTime = 500;
let greenTime = 2000;
let mode = "기본";

function setup() {
  createCanvas(windowWidth, windowHeight);
  background(220);

  port = createSerial(); // 시리얼 포트 객체 생성

  // 아두이노 연결 버튼 생성
  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(windowWidth/2 - 100, 10);
  connectBtn.style("background-color", "#4CAF50");
  connectBtn.size(130, 30);
  connectBtn.mousePressed(connectPort);

  // 아두이노 해제 버튼 생성
  disconnectBtn = createButton("Disconnect");
  disconnectBtn.position(windowWidth/2 + 100, 10);
  disconnectBtn.style("background-color", "#f44336");
  disconnectBtn.size(100, 30);
  disconnectBtn.mousePressed(disconnectPort);
  
  // 현재 실행 중인 Task 표시
  taskDisplay = createP("Task: None");
  taskDisplay.position(windowWidth/2-110, 30);
  taskDisplay.style("font-size", "20px");
  taskDisplay.style("font-weight", "bold");
  taskDisplay.style("color", "#333");
  taskDisplay.style("padding", "10px");
  taskDisplay.style("border", "2px solid #333");
  taskDisplay.style("border-radius", "5px");
  taskDisplay.style("text-align", "center");
  taskDisplay.style("width", "300px");
  taskDisplay.style("background-color", "transparent"); // 초기 배경색

  // 가변 저항 밝기 값 표시
  brightnessDisplay = createP("Brightness: 0");
  brightnessDisplay.position(10, 50);
  brightnessDisplay.style("font-size", "20px");
  brightnessDisplay.style("font-weight", "bold");
  brightnessDisplay.style("color", "#333");
  brightnessDisplay.size(200);

  // 현재 모드 표시
  modeDisplay = createP("Mode: 기본");
  modeDisplay.position(10, 80);
  modeDisplay.style("font-size", "20px");
  modeDisplay.style("font-weight", "bold");
  modeDisplay.style("color", "#333");
  modeDisplay.size(200);

  // 빨강 신호 길이 슬라이더 생성
  redSlider = createSlider(500, 5000, redTime, 10);
  redSlider.position(10, 130);
  redSlider.size(500);
  redSlider.input(updateRedLabel);
  redLabel = createP("Red Time: " + redTime + " ms");
  redLabel.position(windowWidth/2 - 300, 100);

  // 노랑 신호 길이 슬라이더 생성
  yellowSlider = createSlider(500, 5000, yellowTime, 10);
  yellowSlider.position(10, 180);
  yellowSlider.size(500);
  yellowSlider.input(updateYellowLabel);
  yellowLabel = createP("Yellow Time: " + yellowTime + " ms");
  yellowLabel.position(windowWidth/2 - 300, 150);

  // 초록 신호 길이 슬라이더 생성
  greenSlider = createSlider(500, 5000, greenTime, 10);
  greenSlider.position(10, 230);
  greenSlider.size(500);
  greenSlider.input(updateGreenLabel);
  greenLabel = createP("Green Time: " + greenTime + " ms");
  greenLabel.position(windowWidth/2 - 300, 200);
}

let lastTask = "None"; // 마지막 Task 값 저장 (중복 업데이트 방지)

function draw() {
  if (port.available() > 0) {
    let str = port.readUntil("\n").trim(); // 시리얼 데이터 읽기
    console.log("Received: ", str); // 수신된 데이터를 콘솔에 출력

    if (str.startsWith("BRIGHTNESS:")) { // 밝기 값인 경우
      let brightVal = parseInt(str.split(":")[1].trim());
      console.log("Parsed Brightness: ", brightVal); // 파싱된 밝기 값 출력
      brightnessDisplay.html("Brightness: " + brightVal);
    } 
    else if (str.startsWith("MODE:")) {
      let modeVal = str.split(":")[1].trim();
      modeDisplay.html("Mode: " + modeVal);
    } 
    else if (str.startsWith("TASK:")) {
      let taskVal = str.split(":")[1].trim();
      if (taskVal !== lastTask) { // 기존 Task와 다를 때만 업데이트
        lastTask = taskVal; 
        taskDisplay.html("Task: " + taskVal);
        taskDisplay.style("background-color", "red"); // 강조 효과
      }
    }
  }
}

// 아두이노 연결 함수
function connectPort() {
  if (!port.opened()) {
    port.open(9600);
    console.log("Serial Port Opened");
  }
}

// 아두이노 해제 함수
function disconnectPort() {
  if (port.opened()) {
    port.close();
    console.log("Serial Port Closed");
  }
}

// 슬라이더 업데이트 함수 (빨강 신호 시간)
function updateRedLabel() {
  redTime = redSlider.value();
  redLabel.html("Red Time: " + redTime + " ms");
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (노랑 신호 시간)
function updateYellowLabel() {
  yellowTime = yellowSlider.value();
  yellowLabel.html("Yellow Time: " + yellowTime + " ms");
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (초록 신호 시간)
function updateGreenLabel() {
  greenTime = greenSlider.value();
  greenLabel.html("Green Time: " + greenTime + " ms");
  sendSignalTime();
}

// 아두이노로 신호 시간 전송
function sendSignalTime() {
  let signalData = `TIME:${redTime},${yellowTime},${greenTime}\n`;
  console.log("Sending:", signalData);
  port.write(signalData);
}



// 모션 인식
// let handPose;
// let video;
// let hands = [];

// function preload() {
//   handPose = ml5.handPose();
// }

// function setup() {
//   createCanvas(640, 480);
//   video = createCapture(VIDEO, {flipped:true});
//   video.size(640, 480);
//   video.hide();
//   handPose.detectStart(video, gotHands);
// }

// function draw() {
//   image(video, 0, 0, width, height);
//   for (let i = 0; i < hands.length; i++) {
//     let hand = hands[i];
//     for (let j = 0; j < hand.keypoints.length; j++) {
//       let keypoint = hand.keypoints[j];
//       fill(255, 0, 0);
//       noStroke();
//       circle(640-keypoint.x, keypoint.y, 10);
//     }
//   }
// }

// function gotHands(results) {
//   hands = results;
// }
