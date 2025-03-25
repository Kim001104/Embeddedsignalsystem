function setup() {
  createCanvas(windowWidth, windowHeight);
  background('lightgray');
  console.log('p5 version: ', VERSION);
  textFont('Courier New');
  textSize(height / 5);
  textAlign(LEFT, CENTER);
}

function draw() {
  text(" Versions", width/10, height/4);
  text(" p5: " +  p5.VERSION, width/10, 2*height/4);
  text("ml5: " + ml5.version, width/10, 3*height/4);
}
