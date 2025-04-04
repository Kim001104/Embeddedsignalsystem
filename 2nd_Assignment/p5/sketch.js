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

// 비디오 객체 설정
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
  brightnessDisplay.position(10, 10);
  brightnessDisplay.style("font-size", "20px");
  brightnessDisplay.style("font-weight", "bold");
  brightnessDisplay.style("color", "#333");
  brightnessDisplay.size(200);

  // 현재 모드 표시
  modeDisplay = createP("MODE: 기본");
  modeDisplay.position(10, 50);
  modeDisplay.style("font-size", "20px");
  modeDisplay.style("font-weight", "bold");
  modeDisplay.style("color", "green"); // ← 기본색을 눈에 띄게
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

  // 신호등 주기 정보 표시 요소 추가 (슬라이더 아래에)
  timeDisplay = createP(`Traffic Light Timings - Red: ${redTime} ms, Yellow: ${yellowTime} ms, Green: ${greenTime} ms`);
  timeDisplay.position(10, 280);
  timeDisplay.style("font-size", "16px");
  timeDisplay.style("color", "#333");
  timeDisplay.style("font-weight", "bold");
}

function draw() {

  image(video, 0,0, 640,480);  // 비디오 캡쳐 이미지 출력

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
    // 이 함수는 스위치 동작을 통한 모드에 대한 시리얼 데이터를 파싱받아서 UI로 보여주는 함수수
    else if (str.includes("MODE:")) {
      let modePart = str.split("MODE:")[1]?.trim();
      console.log("📥 수신된 modeVal:", JSON.stringify(modePart));
    
      switch (modePart) {
        case "Emergency":
          modeDisplay.html("MODE: 긴급 모드");
          modeDisplay.style("color", "red");
          break;
        case "Caution":
          modeDisplay.html("MODE: 주의 모드");
          modeDisplay.style("color", "orange");
          break;
        case "Global Blink":
          modeDisplay.html("MODE: 전체 깜빡임 모드");
          modeDisplay.style("color", "blue");
          break;
        case "Normal":
          modeDisplay.html("MODE: 기본");
          modeDisplay.style("color", "green");
          break;
        default:
          console.log("⚠️ 알 수 없는 모드:", JSON.stringify(modePart));
          break;
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


//왼손 오른손이 올바르게 감지 되는지
function getHandTypes(hands) {
  let leftHand = null;
  let rightHand = null;

  hands.forEach(hand => {
    if (hand.handedness === "Left") leftHand = hand;
    else if (hand.handedness === "Right") rightHand = hand;
  });

  return { leftHand, rightHand };
}

// 손 제스처 인식 함수
function processHands(hands) {
  const { leftHand, rightHand } = getHandTypes(hands);

  if (leftHand && rightHand) {
    const selectedColor = getLeftGestureMode(leftHand); // LED 색상 선택 (예: 엄지+검지 = yellow)

    if (selectedColor) {
      adjustLedTime(rightHand, selectedColor); // 오른손으로 엄지 up/down → 조절
    } else {
      detectModeFromLeftHand(leftHand); // LED 선택 안 되면 → 모드 판단
    }
  } else if (leftHand) {
    detectModeFromLeftHand(leftHand);
  }
}

// 손가락이 펼쳐졌는지 여부를 판단하는 함수
function isFingerExtended(tip, dip, pip, mcp) {
  return (
    tip.y < dip.y &&  // TIP이 DIP보다 위쪽
    dip.y < pip.y &&  // DIP이 PIP보다 위쪽
    pip.y < mcp.y     // PIP이 MCP보다 위쪽
  );
}


/*
  이곳은 신호등의 주기를 모션을 통하여 컨트롤 하는 함수입니다.
*/

// 손 제스처에 따라 조절할 LED 색상을 결정하는 함수
function getLeftGestureMode(hand) {
  let k = hand.keypoints;

  const isThumb  = isFingerExtended(k[4], k[3], k[2], k[1]);   // 엄지
  const isIndex  = isFingerExtended(k[8], k[7], k[6], k[5]);   // 검지
  const isMiddle = isFingerExtended(k[12], k[11], k[10], k[9]); // 중지
  const isRing   = isFingerExtended(k[16], k[15], k[14], k[13]); // 약지
  const isPinky  = isFingerExtended(k[20], k[19], k[18], k[17]); // 새끼손가락

  const extended = [isThumb, isIndex, isMiddle, isRing, isPinky].filter(v => v).length;

  // 🔴 엄지
  if (extended === 1 && isThumb) {
    return "red";
  }
  // 🟡 엄지 + 소지
  else if (extended === 2 && isThumb && isIndex) {
    return "yellow";
  }
  // 🟢 엄지 + 검지 + 중지
  else if (extended === 3  && isIndex && isIndex && isMiddle) {
    return "green";
  }
  return null;
}


// 따봉을 통한 주기 올리기 함수수
function isThumbsUp(hand) {
  if (!hand || !hand.keypoints || hand.keypoints.length < 21) return false;
  const k = hand.keypoints;

  // 엄지만 펼침 + 엄지 tip이 MCP보다 위
  return (
    k[4].y < k[2].y && // TIP이 MCP보다 위 → 위로 향함
    k[8].y > k[6].y && // 검지 접힘
    k[12].y > k[10].y &&
    k[16].y > k[14].y &&
    k[20].y > k[18].y
  );
}

// 브이를 통한 주기 내리기 함수수
function isIndexAndMiddle(hand) {
  if (!hand || !hand.keypoints || hand.keypoints.length < 21) return false;
  const k = hand.keypoints;

  return (
    k[8].y < k[6].y &&   // 검지 펼침
    k[12].y < k[10].y && // 중지 펼침
    k[16].y > k[14].y && // 약지 접힘
    k[20].y > k[18].y && // 새끼 접힘
    k[4].y > k[3].y      // 엄지 접힘
  );
}


// LED 주기 조절
let lastUpdateTime = 0;

function adjustLedTime(rightHand, selectedColor) {
  const now = Date.now();

  if (now - lastUpdateTime > 800) {
    if (isThumbsUp(rightHand)) {
      if (selectedColor === "red") redTime += 100;
      if (selectedColor === "yellow") yellowTime += 100;
      if (selectedColor === "green") greenTime += 100;
    } else if (isIndexAndMiddle(rightHand)) {
      if(selectedColor === "red") {
        console.log("Before:", redTime); // 이걸 추가해봐!
        redTime = Math.max(100, redTime - 100);
        console.log("After:", redTime);  // 이걸로 실제 감소했는지 확인
      }
      if (selectedColor === "yellow") {
        console.log("Before:",yellowTime);
        yellowTime = Math.max(100, yellowTime - 100);
        console.log("After:",yellowTime);
      }
      if (selectedColor === "green") {
        console.log("Before:",greenTime);
        greenTime = Math.max(100, greenTime - 100);
        console.log("After:",greenTime);
      }
    }
    // 슬라이더와 레이블 동기화 (조정된 경우에만)
    if (selectedColor === "red") {
      redSlider.value(redTime);
      redLabel.html("Red Time: " + redTime + " ms");
    }
    if (selectedColor === "yellow") {
      yellowSlider.value(yellowTime);
      yellowLabel.html("Yellow Time: " + yellowTime + " ms");
    }
    if (selectedColor === "green") {
      greenSlider.value(greenTime);
      greenLabel.html("Green Time: " + greenTime + " ms");
    }
    

    console.log(`${selectedColor} 조정됨 →`, eval(selectedColor + "Time"));
    sendSignalTime(); //아두이노로 전송
    updateTimeDisplay(); //전체 시간 표시 업데이트
    lastUpdateTime = now; //시간 업데이트
  }

  
}



/* 
  이곳은 신호등 모드를 변경하는 함수
  비상모드: 엄지와 소지
  위험모드: 주먹
  GlobalBlink모드: 검지 중지 약지지 인식(첫번째 두번째 세번째 손가락)
  Normal모드: 다섯 손가락 다 핌핌
*/
let currentMode = "";
let modeTimeout = null;

function detectModeFromLeftHand(hand) {
  if (!hand) {
    console.warn("⚠️ detectModeFromLeftHand()가 null hand를 받음!");
    return; // 함수 실행 중단
  }

  let k = hand.keypoints;

  if (!k) {
    console.warn("⚠️ keypoints가 없음!");
    return; // 함수 실행 중단
  }

  let isThumb = k[4].y < k[3].y;  // 엄지 펼침 여부
  let isIndex = k[8].y < k[6].y;  // 검지 펼침 여부
  let isMiddle = k[12].y < k[10].y; // 중지 펼침 여부
  let isRing = k[16].y < k[14].y; // 약지 펼침 여부
  let isPinky = k[20].y < k[18].y; // 소지 펼침 여부

  let detectedMode = "";

  if (isThumb && !isIndex && !isMiddle && !isRing && isPinky) { // 엄지 소지만 펼쳤을 때 emergency 모드
    detectedMode = "Emergency";
  } else if (!isThumb && !isIndex && !isMiddle && !isRing && !isPinky) {  // 주먹? Caution 모드드
    detectedMode = "Caution";  
  } else if (!isThumb && isIndex && isMiddle && isRing) { // 검지 중지 약지만 펼쳤을 때 Globalblink모드
    detectedMode = "Global Blink";
  } else if (isThumb && isIndex && isMiddle && isRing && isPinky) {  // 모든 손 활짝 핌 → Normal 모드 복귀
    detectedMode = "Normal";
  }

  if (detectedMode !== "" && detectedMode !== currentMode) {
    console.log("🖐️ 모드 전환:", detectedMode);
    currentMode = detectedMode;
    sendMode(detectedMode);  // ✅ sendMode()로 전송
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

function updateTimeDisplay() {
  timeDisplay.html(`Traffic Light Timings - Red: ${redTime} ms, Yellow: ${yellowTime} ms, Green: ${greenTime} ms`);
}


// 슬라이더 업데이트 함수 (빨강 신호 시간)
function updateRedLabel() {
  redTime = redSlider.value();
  redLabel.html("Red Time: " + redTime + " ms");
  updateTimeDisplay();  // ✅ 추가
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (노랑 신호 시간)
function updateYellowLabel() {
  yellowTime = yellowSlider.value();
  yellowLabel.html("Yellow Time: " + yellowTime + " ms");
  updateTimeDisplay();  // ✅ 추가
  sendSignalTime();
}

// 슬라이더 업데이트 함수 (초록 신호 시간)
function updateGreenLabel() {
  greenTime = greenSlider.value();
  greenLabel.html("Green Time: " + greenTime + " ms");
  updateTimeDisplay();  // ✅ 추가
  sendSignalTime();
}

// 시간 조절 슬라이더 변경 시 시리얼 통신
function sendSignalTime() { 
  clearTimeout(sendTimeout);
  sendTimeout = setTimeout(() => {
    let signalData = `TIME:${redTime},${yellowTime},${greenTime}\n`;
    console.log("Sending:", signalData);
    port.write(signalData);
  }, 200);
}

// 비상모드,위험모드,Global Blink모드 전송 함수
function sendMode(modeName) {
  let msg = `MODE:${modeName}\n`;
  console.log(`🚀 sendMode 호출됨: ${msg.trim()}`);

  if (port && port.opened()) {
    port.write(msg);
    console.log("➡️ 모드 전송 성공:", msg.trim());
  } else {
    console.warn("⚠️ 포트가 닫혀있어서 전송 실패:", msg.trim());
  }

  currentMode = modeName;
}


