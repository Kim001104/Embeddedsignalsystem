// 비디오 객체 설정
let handPose; // HandPose 모델
let video;  // 비디오 스트림
let hands = []; // 손 정보

let drawingPoints = []; // 그림 좌표 저장
let isDrawing = true;   // 오른손 주먹이면 false

let isThumb = k[4].y < k[3].y;  //엄지
let isIndex = k[8].y < k[6].y;  //검지
let isMiddle = k[12].y < k[10].y; //중지
let isRing = k[16].y < k[14].y; //약지
let isPinky = k[20].y < k[18].y;  //소지


function preload() {  // 모델 로드
  handPose = ml5.handPose({flipped: true});
  console.log("HandPose Model Loaded"); // 모델 로드 완료 시 콘솔에 출력
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

function setup() {
  createCanvas(640,480);  // 캔버스 크기 설정
  video = createCapture(VIDEO, {  // 비디오 스트림 설정
    width: 640,
    height: 480,
    flipped: true
  });
  video.size(640, 480); // 비디오 크기 설정
  
  video.hide(); // 비디오 숨김

  handPose.detectStart(video, gotHands);  // 손 인식 시작
}

function draw() {
  image(video, 0, 0, 640, 480); // 비디오 이미지 출력

  if (hands.length > 0) { // 손이 인식되면
    drawKeypoints(hands); // 손 모델 인식 함수 실행

    let { leftHand, rightHand } = getHandTypes(hands);  // 왼손, 오른손 구분

    // 왼손 이모지 표시
    if (leftHand) { // 왼손이 인식되면
      let emoji = detectLeftHandGesture(leftHand);  // 왼손 제스처 인식
      if (emoji) {  // 제스처가 인식되면
        let pos = getPointByName(leftHand.keypoints, "palm_base") || leftHand.keypoints[0]; // 손바닥 위치
        drawEmoji(emoji, pos.x, pos.y); // 이모지 출력
      }
    }

    // 오른손 그리기
    if (rightHand) {
      let k = rightHand.keypoints;
      if (k.length >= 21) {
        let isIndex = k[8].y < k[6].y;  // 검지 펼침 여부
    
        if (isRightHandFist(rightHand)) {
          drawingPoints = [];  // 다 펴짐 → 전체 그림 삭제
          isDrawing = false;
        } else if (isIndex) {
          isDrawing = true;
          drawingPoints.push({ x: k[8].x, y: k[8].y });  // 검지 끝 좌표로 그림
        }
      }
    }
    
  }

  // 그림 그리기
  noFill(); // 채우기 없음
  stroke(255, 0, 0);  // 선 색상
  strokeWeight(4);  // 선 두께
  beginShape();   // 도형 시작
  for (let pt of drawingPoints) { // 그리기 좌표
    vertex(pt.x, pt.y); // 좌표 설정
  }
  endShape(); // 도형 끝
}


// 손가락 위치 추출
function getPointByName(keypoints, name) {
  return keypoints.find(pt => pt.name === name);
}

// 손 인식 결과
function gotHands(results) {
  hands = results;
}

// 왼손, 오른손 구분
function getHandTypes(hands) {  // 왼손, 오른손 구분
  let leftHand = null;
  let rightHand = null;

  hands.forEach((hand, idx) => {  // 손 정보 출력
    console.log("hand[" + idx + "] handedness:", hand.handedness);  // 왼손, 오른손 출력
    if (!hand.handedness || hand.handedness === "Left") leftHand = hand;  // 왼손이거나 왼손이면 왼손
    else if (hand.handedness === "Right") rightHand = hand; // 오른손이면 오른손
  });

  return { leftHand, rightHand };
}

function detectLeftHandGesture(leftHand) {    // 왼손 제스처 인식
  let k = leftHand.keypoints; // 손가락 위치

  if (k.length < 21) return null; // 손가락 위치가 21개가 아니면 null

  // 손가락 펼침 판단 (TIP vs PIP)
  let isThumb = k[4].y < k[3].y;
  let isIndex = k[8].y < k[6].y;
  let isMiddle = k[12].y < k[10].y;
  let isRing = k[16].y < k[14].y;
  let isPinky = k[20].y < k[18].y;

  // 👍 엄지척
  if (isThumb && !isIndex && !isMiddle && !isRing && !isPinky) {
    return "👍";
  }

  // 🤙 샤카
  if (isThumb && isPinky && !isIndex && !isMiddle && !isRing) {
    return "🤙";
  }

  // ✌️ 브이
  if (!isThumb && isIndex && isMiddle && !isRing && !isPinky) {
    return "✌️";
  }

  // ✊ 주먹
  if (!isThumb && !isIndex && !isMiddle && !isRing && !isPinky) {
    return "✊";
  }

  // 🖐️제스처
  if(isThumb && isIndex && isMiddle && isRing && isPinky){
    return "🖐️";
  }
  return null;
}

// 손가락 펴짐 함수
function isFingerExtended(fingerTip, fingerMCP) {
  return fingerTip.y < fingerMCP.y; // 화면 기준 위로 올라가 있으면 펼쳐짐
}


// 이모지 출력 함수
function drawEmoji(emoji, x, y) {
  textSize(64);
  textAlign(CENTER, CENTER);
  text(emoji, x, y);
}

// 오른손이 펴져 있는지 판단하는 함수수
function isRightHandFist(rightHand) {
  let k = rightHand.keypoints;

  // keypoints가 충분히 감지되지 않으면 false 반환
  if (k.length < 21) return false;

  // 각 손가락 펼침 여부 계산 (TIP vs PIP)
  let isThumb = k[4].y < k[3].y;
  let isIndex = k[8].y < k[6].y;
  let isMiddle = k[12].y < k[10].y;
  let isRing = k[16].y < k[14].y;
  let isPinky = k[20].y < k[18].y;

  // 모두 펴져 있는 것으로 판단단
  return (
    isThumb &&
    isIndex &&
    isMiddle &&
    isRing &&
    isPinky
  );
}

