let port; // Serial Port 객체
let connectBtn, disconnectBtn;  // Connect, Disconnect 버튼
let redSlider, yellowSlider, greenSlider; // Red, Yellow, Green 시간 조절 슬라이더
let redLabel, yellowLabel, greenLabel;  // Red, Yellow, Green 시간 레이블
let brightnessDisplay, modeDisplay; // 밝기, 모드 상태를 표시할 HTML 요소
let redTime = 2000;
let yellowTime = 500;
let greenTime = 2000;
let mode = "기본";  // 모드 상태
let lastTask = "None"; // 마지막 Task 값 저장 (중복 업데이트 방지)
let sendTimeout;  // 시간 조절 슬라이더 변경 시 시리얼 통신 지연을 위한 타임아웃
let timeDisplay;  // Red, Yellow, Green 시간을 표시할 HTML 요소
let taskDisplay; // ✅ Task 상태를 표시할 HTML 요소


let handPose; // HandPose 모델
let video;  // 비디오 스트림
let hands = []; // 손 정보

function preload() {  // 모델 로드
  handPose = ml5.handPose({flipped: true});
  console.log("HandPose Model Loaded"); // 모델 로드 완료 시 콘솔에 출력
}

function gotHands(results) {
  hands = results;
}

function setup() {

  createCanvas(640,480);  // 캔버스 생성

  video = createCapture(VIDEO, {flipped: true});  // 비디오 캡쳐 생성
  video.size(640,480); // 비디오 크기 설정
  video.hide(); // 비디오 숨김

  handPose.detectStart(video,gotHands);

  port = createSerial();

  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(windowWidth/2 - 100, 10);
  connectBtn.style("background-color", "green");
  connectBtn.size(130, 30);
  connectBtn.mousePressed(connectPort);

  disconnectBtn = createButton("Disconnect");
  disconnectBtn.position(windowWidth/2 + 100, 10);
  disconnectBtn.style("background-color", "red");
  disconnectBtn.size(100, 30);
  disconnectBtn.mousePressed(disconnectPort);

  brightnessDisplay = createP("Brightness: 0");
  brightnessDisplay.position(10, 50);

  modeDisplay = createP("Mode: 기본");
  modeDisplay.position(10, 80);

  taskDisplay = createP("Task: None");
  taskDisplay.position(windowWidth / 2 - 110, 30);
  taskDisplay.style("font-size", "20px");
  taskDisplay.style("font-weight", "bold");
  taskDisplay.style("color", "#333");
  taskDisplay.style("padding", "10px");
  taskDisplay.style("border", "2px solid #333");
  taskDisplay.style("border-radius", "5px");
  taskDisplay.style("text-align", "center");
  taskDisplay.style("width", "300px");
  taskDisplay.style("background-color", "transparent");
  timeDisplay = createP("Traffic Light Timings - Red: 2000 ms, Yellow: 500 ms, Green: 2000 ms");
  timeDisplay.position(10, 110);
  timeDisplay.style("font-size", "18px");
  timeDisplay.style("font-weight", "bold");

  redSlider = createSlider(500, 5000, redTime, 10);
  redSlider.position(10, 160);
  redSlider.size(500);
  redSlider.input(updateRedLabel);
  redLabel = createP("Red Time: " + redTime + " ms");
  redLabel.position(550, 140);

  yellowSlider = createSlider(500, 5000, yellowTime, 10);
  yellowSlider.position(10, 210);
  yellowSlider.size(500);
  yellowSlider.input(updateYellowLabel);
  yellowLabel = createP("Yellow Time: " + yellowTime + " ms");
  yellowLabel.position(550, 190);

  greenSlider = createSlider(500, 5000, greenTime, 10);
  greenSlider.position(10, 260);
  greenSlider.size(500);
  greenSlider.input(updateGreenLabel);
  greenLabel = createP("Green Time: " + greenTime + " ms");
  greenLabel.position(550, 240);
}

function draw() {

  image(video, 0,0, 640,480);  // 비디오 캡쳐 이미지 출력

  // // 손 정보 출력
  // for (let i = 0; i < hands.length; i++) {
  //   let hand = hands[i];
  //   for (let j = 0; j < hand.keypoints.length; j++) {
  //     let keypoint = hand.keypoints[j];
  //     fill(0, 255, 0);
  //     noStroke();
  //     circle(keypoint.x, keypoint.y, 10);
  //   }
  // }

  if(hands.length > 0) {
    processHands(hands);
    drawKeypoints(hands);
  }

  if (port.available() > 0) { // 아두이노에서 시리얼 데이터를 받아서 웹 상에 html요소로 출력 
    let str = port.readUntil("\n").trim();  // 줄바꿈까지 읽어오기
    console.log("Received: ", str); // 수신한 데이터 콘솔에 출력

    if (str.startsWith("BRIGHTNESS:")) {  // BRIGHTNESS 데이터를 수신 받으면 숫자를 파싱함.
      let brightVal = parseInt(str.split(":")[1].trim()); // 밝기 값 추출
      brightnessDisplay.html("Brightness: " + brightVal); // 밝기 값 HTML 요소에 출력
    } 
    
    else if (str.startsWith("MODE:")) {   // MODE 데이터를 수신 받으면 파싱함.
      
      let modeVal = str.split(":")[1].trim();

      if (modeVal === "Emergency") {  // Emergency 수신시 긴급모드라고 html요소로 출력함.
        modeDisplay.html("MODE: 긴급 모드");
        modeDisplay.style("color", "red");

      } 
      else if (modeVal === "Caution") { // Caution 수신시 주의 모드라고 html요소로 출력함.
        modeDisplay.html("MODE: 주의 모드");
        modeDisplay.style("color", "orange");

      } 
      else if (modeVal === "Global Blink") {  // Global Blink 수신시에 전체 깜빡임 모드라고 html요소로 출력함.
        modeDisplay.html("MODE: 전체 깜빡임 모드");
        modeDisplay.style("color", "blue");
      } 
      else {
        modeDisplay.html("MODE: 기본"); // 기본 신호등 모드
        modeDisplay.style("color", "#333");
      }
    }
    
    else if (str.startsWith("TIME:")) {     // TIME 수신 시 "TIME:" 이후 문자열만 추출해서 ["3000", "1000", "2000"]처럼 분할
      let times = str.substring(5).split(",");
      if (times.length === 3) { // 각 시간 값을 정수형 int로 분할하여 newRedTime,newYellowTime,newGreenTime에 저장장
        let newRedTime = parseInt(times[0]);
        let newYellowTime = parseInt(times[1]);
        let newGreenTime = parseInt(times[2]);

        if(newRedTime !== redTime || newYellowTime !== yellowTime || newGreenTime !== greenTime) {  //기존 값과 다를 때만 갱신신
          redTime = newRedTime;
          yellowTime = newYellowTime;
          greenTime = newGreenTime;
          timeDisplay.html(`Traffic Light Timings - Red: ${redTime} ms, Yellow: ${yellowTime} ms, Green: ${greenTime} ms`);
          redSlider.value(newRedTime);  //슬라이더 UI 변환
          yellowSlider.value(newYellowTime);
          greenSlider.value(newGreenTime);
          sendSignalTime(); // 그 후에 현재 시간값을 다시 아두이노로 보냄
        }
      }
    }
    else if (str.startsWith("TASK:")) {
      let taskInfo = str.substring(5);
      taskDisplay.html("Task: " + taskInfo);
    }
  }
}


// 손 모델 인식 함수
function drawKeypoints(hands) {
  for (let i = 0; i < hands.length; i++) {
    let hand = hands[i];
    for (let j = 0; j < hand.keypoints.length; j++) {
      let keypoint = hand.keypoints[j];
      fill(0, 255, 0);
      noStroke();
      circle(keypoint.x, keypoint.y, 10);
    }
  }
}

// 손 제스처 인식 함수
function processHands(hands) {
  if (hands.length === 2) {
    let leftHand = hands[0]; // 왼손: 어떤 LED를 조절할지 결정
    let rightHand = hands[1]; // 오른손: 👍(증가) 또는 👎(감소)

    let selectedColor = null;

    // 왼손으로 LED 색상 선택
    if (isOnlyThumbFinger(leftHand)) {
      selectedColor = "red";
    } else if (isThumbAndIndex(leftHand)) {
      selectedColor = "yellow";
    } else if (isThumbIndexMiddle(leftHand)) {
      selectedColor = "green";
    }

    if (selectedColor) {
      adjustLedTime(selectedColor, rightHand);
    }
    else{
      detectModeFromLeftHand(leftHand);
    }
  }
}

// LED를 조절하기 위한 함수들 조합 

//엄지만 펼쳐진 함수(빨간색 LED주기 조절)
function isOnlyThumbFinger(hand) {
  let k = hand.keypoints;
  return (
    k[4].y < k[3].y && // 엄지 펼침
    k[8].y > k[6].y && // 검지 접힘
    k[12].y > k[10].y && // 중지 접힘
    k[16].y > k[14].y && // 약지 접힘
    k[20].y > k[18].y // 새끼손가락 접힘
  );
}


// 엄지,검지 펼쳐진 함수(노란색 LED주기 조절)
function isThumbAndIndex(hand) {
  let k = hand.keypoints;
  return (
    k[4].y < k[3].y &&  // thumb 펼침
    k[8].y < k[6].y &&  // index 펼침
    k[12].y > k[10].y &&
    k[16].y > k[14].y &&
    k[20].y > k[18].y
  );
}

// 엄지,검지,중지 펼쳐진 함수(초록색 LED주기 조절)
function isThumbIndexMiddle(hand) {
  let k = hand.keypoints;
  return (
    k[4].y < k[3].y &&  // thumb 펼침
    k[8].y < k[6].y &&  // index 펼침
    k[12].y < k[10].y && // middle 펼침
    k[16].y > k[14].y &&
    k[20].y > k[18].y
  );
}

// 검지만 펼쳐짐 (주기 증가)
function isOnlyIndexFinger(hand) {
  let k = hand.keypoints;
  return (
    k[8].y < k[6].y && // 검지 펼침
    k[12].y > k[10].y && // 중지 접힘
    k[16].y > k[14].y && // 약지 접힘
    k[20].y > k[18].y // 새끼손가락 접힘
  );
}

// 검지 + 중지 펼쳐짐 (주기 감소)
function isIndexAndMiddle(hand) {
  let k = hand.keypoints;
  return (
    k[8].y < k[6].y && // 검지 펼침
    k[12].y < k[10].y && // 중지 펼침
    k[16].y > k[14].y && // 약지 접힘
    k[20].y > k[18].y // 새끼손가락 접힘
  );
}

// LED 주기 조절
let lastUpdateTime = 0;

function adjustLedTime(color, hand) {
  let currentTime = millis(); // 현재 시간

  // 500ms마다 조정
  if (currentTime - lastUpdateTime > 500) {
    if (isOnlyIndexFinger(hand)) {
      if (color === "red") redTime += 100;
      if (color === "yellow") yellowTime += 100;
      if (color === "green") greenTime += 100;
    } else if (isIndexAndMiddle(hand)) {
      if (color === "red") redTime = max(100, redTime - 100);
      if (color === "yellow") yellowTime = max(100, yellowTime - 100);
      if (color === "green") greenTime = max(100, greenTime - 100);
    }

    console.log(`${color} LED 주기:`, eval(color + "Time"));
    sendSignalTime();

    lastUpdateTime = currentTime;
  }
}

function detectModeFromLeftHand(hand) {
  let k = hand.keypoints;

  let isThumb = k[4].y < k[3].y;
  let isIndex = k[8].y < k[6].y;
  let isMiddle = k[12].y < k[10].y;
  let isRing = k[16].y < k[14].y;

  let detectedMode = "";

  if (!isThumb && isIndex && !isMiddle) {
    detectedMode = "Emergency";
  } else if (!isThumb && isIndex && isMiddle && !isRing) {
    detectedMode = "Caution";
  } else if (isThumb && isIndex && isMiddle && !isRing) {
    detectedMode = "Global Blink";
  } 
  // ✅ 엄지 + 검지 + 중지 + 약지 → Normal 모드 복귀
  else if (isThumb && isIndex && isMiddle && isRing) {
    detectedMode = "Normal";
  }

  if (detectedMode !== "" && detectedMode !== currentMode) {
    port.write(`MODE:${detectedMode}\n`);
    console.log("🖐️ 모드 전환:", detectedMode);
    currentMode = detectedMode;
    resetModeTimeout(); // 선택사항: 자동 복귀 타이머 초기화
  }
}

// 일정 시간 지나면 Normal 모드 전송
function resetModeTimeout() {
  if (modeTimeout) clearTimeout(modeTimeout);
  modeTimeout = setTimeout(() => {
    if (currentMode !== "Normal") {
      port.write("MODE:Normal\n");
      console.log("🕒 모드 자동 복귀: Normal");
      currentMode = "Normal";
    }
  }, 3000); // 3초 동안 새로운 제스처 없으면 복귀
}

// 비상모드,위험모드,Global Blink모드 전송 함수
function sendMode(modeName) {
  if (currentMode !== modeName) {
    let msg = `MODE:${modeName}\n`;
    port.write(msg);
    currentMode = modeName;
    console.log("➡️ 모드 전송:", msg.trim());
  }
}

function connectPort() {  // 시리얼 포트 연결
  if (!port.opened()) {
    port.open(9600);
    console.log("Serial Port Opened");
  }
}

function disconnectPort() { // 시리얼 포트 연결 해제
  if (port.opened()) {
    port.close();
    console.log("Serial Port Closed");
  }
}

function updateRedLabel() { // Red 시간 레이블 업데이트
  redTime = redSlider.value();
  sendSignalTime();
}

function updateYellowLabel() {  //  Yellow 시간 레이블 업데이트
  yellowTime = yellowSlider.value();
  sendSignalTime();
}

function updateGreenLabel() {     // Green 시간 레이블 업데이트
  greenTime = greenSlider.value();
  sendSignalTime();
}

function sendSignalTime() { // 시간 조절 슬라이더 변경 시 시리얼 통신
  clearTimeout(sendTimeout);
  sendTimeout = setTimeout(() => {
    let signalData = `TIME:${redTime},${yellowTime},${greenTime}\n`;
    console.log("Sending:", signalData);
    port.write(signalData);
  }, 200);
}