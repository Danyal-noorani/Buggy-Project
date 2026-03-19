import processing.net.*;
import controlP5.*;
import java.awt.DisplayMode;
import java.awt.GraphicsEnvironment;


// Car variables
float carX = 0;          // Horizontal position
float previousCarX = 0;  // For smoothing horizontal movement
float carSpeed =20;      // Horizontal speed

float carY = -50;        // Start above the screen
float targetCarY;        // Final Y position above the road
float carSpeedY = 5;     // Vertical drop speed


// Client
Client myClient;
boolean connected;

// CP5
ControlP5 cp5;
Textlabel distanceLabel, speedLabel, distanceCoveredLabel, slowDownFactorLabel, obstacleLabel, movementStatusLabel, wifiStatusLabel;
Button turnLeft, turnRight, adjustLeft, adjustRight, sendNextDistance;
Textfield nextDistance;
String[] values ={"50", "1", "1"};
boolean stopped=false;


// Radius of circular buttons
int btnRadius = 50;

// Center positions
float leftX = 150, leftY = 600;
float rightX = 300, rightY = 600;

void settings() {
  int baseWidth = 1080;
  int baseHeight = 620;

  int density = displayDensity();  // 1 for normal, 2 for HiDPI
  size(baseWidth * density, baseHeight * density);
}


void setup() {

  carY = height - 130; // Position car above the road

  myClient = new Client(this, "192.168.4.1", 5200);
  cp5 = new ControlP5(this);

  // --- Font Styles ---
  PFont labelFont = createFont("Adwaita Mono", 20);
  PFont btnFont = createFont("Adwaita Mono", 24);

  // --- Buttons ---

  //   turnLeft = cp5.addButton("TurnLeft")
  //     .setPosition(leftX - btnRadius, leftY - btnRadius)
  //     .setSize(btnRadius*2, btnRadius*2)
  //     .setLabel("")   // no text
  //     .setColorBackground(color(0, 0)) // invisible
  //     .setColorActive(color(0, 0));

  //   // --- Turn Right Button ---
  //   turnRight = cp5.addButton("TurnRight")
  //     .setPosition(rightX - btnRadius, rightY - btnRadius)
  //     .setSize(btnRadius*2, btnRadius*2)
  //     .setLabel("")
  //     .setColorBackground(color(0, 0))
  //     .setColorActive(color(0, 0));

  // -- TextField --

  nextDistance = cp5.addTextfield("DistanceInput")
    .setPosition(20, 320)
    .setFont(btnFont)
    .setSize(200, 50)
    .setAutoClear(false)
    .setLabel("Distance (cm)");
  nextDistance.getCaptionLabel().setColor(color(0));


  sendNextDistance = cp5.addButton("SendDistanceInput")
    .setPosition(20, 410)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("Send Value");



  turnLeft = cp5.addButton("TurnLeft")
    .setPosition(500, 400)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("Turn Left");

  turnRight = cp5.addButton("TurnRight")
    .setPosition(700, 400)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("Turn Right");

  adjustLeft = cp5.addButton("AdjustLeft")
    .setPosition(500, 460)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("<-");

  adjustRight = cp5.addButton("AdjustRight")
    .setPosition(700, 460)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("->");


  // --- Labels ---
  distanceLabel = cp5.addLabel("Distance").setText("Distance: N/A").setFont(labelFont).setColor(color(255));
  speedLabel = cp5.addLabel("LiveSpeed").setText("Speed : N/A").setFont(labelFont).setColor(color(255));
  distanceCoveredLabel =cp5.addLabel("DistanceCovered").setText("Total Distance Covered : N/A").setFont(labelFont).setColor(color(255));

  Textlabel[] labelList = {distanceLabel, speedLabel, distanceCoveredLabel};
  int labelPadding = 50;
  int startY = 180;
  int startX = 20; // move labels to the left

  for (int i = 0; i < 3; i++) {
    labelList[i].setPosition(startX, startY + (i * labelPadding));
  }

  wifiStatusLabel = cp5.addLabel("WifiStatus")
    .setText("Disconnected")
    .setPosition(20, 20)
    .setColor(color(255, 0, 0))
    .setFont(labelFont);

  obstacleLabel = cp5.addLabel("ObstacleStatus")
    .setText("No Obstacle Detected")
    .setPosition(20, 60)
    .setColor(color(0, 255, 0))
    .setFont(labelFont);

  movementStatusLabel = cp5.addLabel("MovementStatus")
    .setText("Stopped")
    .setPosition(20, 100)
    .setColor(color(255, 0, 0))
    .setFont(labelFont);
}


// Resets Button function and colors
void resetWidgets() {
  Textlabel[] textList ={distanceLabel, speedLabel, distanceCoveredLabel};

  for ( Textlabel i : textList) {
    i.setColor(color(0));
  }

  Controller[] controllerList= {obstacleLabel, movementStatusLabel, distanceLabel, distanceCoveredLabel, speedLabel};

  for ( Controller i : controllerList) {
    i.setLock(false);
  }
}

void drawCar(float x, float y) {
  noStroke();

  // Car body
  fill(230, 30, 140);
  rect(x, y, 100, 30);
  rect(x + 15, y - 20, 60, 30);

  // Wheels
  fill(0);
  ellipse(x + 25, y + 30, 25, 25);
  ellipse(x + 75, y + 30, 25, 25);
}

void draw() {
  background(173, 216, 230);


  // --- Sun (add this at the top) ---
  float sunSize = 80 + 10 * sin(radians(frameCount * 2));
  fill(255, 204, 0);
  noStroke();
  ellipse(width - 100, 100, sunSize, sunSize);


  // ---------- Triangle above Turn Left ----------
  float leftBtnX = turnLeft.getPosition()[0];
  float leftBtnY = turnLeft.getPosition()[1];
  float leftBtnW = turnLeft.getWidth();

  float leftCenterX = leftBtnX + leftBtnW/2;

  fill(230, 30, 140);   // light blue
  stroke(0);

  triangle(
    leftCenterX - 60, leftBtnY - 90, // left point
    leftCenterX + 30, leftBtnY - 120, // top right
    leftCenterX + 30, leftBtnY - 60    // bottom right
    );


  // ---------- Triangle above Turn Right ----------
  float rightBtnX = turnRight.getPosition()[0];
  float rightBtnY = turnRight.getPosition()[1];
  float rightBtnW = turnRight.getWidth();

  float rightCenterX = rightBtnX + rightBtnW/2;

  triangle(
    rightCenterX + 60, rightBtnY - 90, // right point
    rightCenterX - 30, rightBtnY - 120, // top left
    rightCenterX - 30, rightBtnY - 60   // bottom left
    );
  connected = (myClient != null && myClient.active());
  // if client is not connected then grey out everything
  if (connected) {
    resetWidgets();
    connectedStatus();
    getLiveData();
  } else {

    greyOutWidgets();
  }

  //---------- Car Animation Panel ----------
  fill(120);
  rect(0, height - 100, width, 100); // road background

  // Move car smoothly
  carX = lerp(previousCarX, carX + carSpeed, 0.1);
  previousCarX = carX;
  carY = lerp(carY, height - 90, 0.1);

  // Wrap around
  if (carX > width) {
    carX = -150;
    previousCarX = -200;
  }

  // Draw the car
  drawCar(carX, carY);
}

void greyOutWidgets() {
  Controller[] controllerList= {obstacleLabel, movementStatusLabel, distanceLabel, distanceCoveredLabel, speedLabel};
  CColor greyColor = new CColor();
  greyColor.setBackground(color(90));
  greyColor.setForeground(color(90));
  greyColor.setActive(color(90));
  greyColor.setValueLabel(color(90));

  //Greys out and locks all widgets
  for (Controller i : controllerList) {
    if (i!=null)
    {
      i.setColor(greyColor);
      i.setLock(true);
    }
  }
}

int lastCommandTime = 0;
int commandCoolDown = 250;

// On interaction with anything this function is called (Proprietary function from library)
void controlEvent(ControlEvent e) {

  if (myClient == null) return;
  if (millis()-lastCommandTime > 50) {
    lastCommandTime = millis();
    switch(e.getName()) {

    case "TurnRight":
      myClient.write("RIGHT\n");
      break;
    case "TurnLeft":
      myClient.write("LEFT\n");
      break;
    case "AdjustLeft":
      myClient.write("ALEFT\n");
    case "AdjustRight":
      myClient.write("ARIGHT\n");
    case "SendDistanceInput":
      myClient.write("MOVE:"+nextDistance.getText()+"\n");

      break;
    }
    lastRequestTime = millis() + commandCoolDown;
  }
}


void connectedStatus() {
  fill(255, 255, 255);
  // stroke(0);        // Set outline to black
  // strokeWeight(1);
  rect( 15, 10, 215, 130);
  if (connected) {
    wifiStatusLabel.setText("Connected").setColor(color(0, 255, 0));
  } else {
    wifiStatusLabel.setText("Disconnected").setColor(color(255, 0, 0));
  }
}


int lastRequestTime = 0;
int pollEveryMs = 200;

void getLiveData() {
  String data = "";

  if (int(values[0])>25) {
    obstacleLabel.setText("No Obstacle Detected").setColor(color(0, 255, 0));
  } else {
    obstacleLabel.setText("Obstacle Detected").setColor(color(255, 0, 0));
  }

  // Poll Arduino
  if (connected && ((millis() - lastRequestTime) > pollEveryMs)) { //Waits pollEveryMs (200ms) before getting new data
    lastRequestTime = millis();
    if (myClient != null) myClient.write("GET_DATA\n");
  }

  // Read incoming data
  if (myClient != null && myClient.available() > 0) {
    String incoming = myClient.readStringUntil('\n');
    if (incoming != null) {
      incoming = trim(incoming);
      data = incoming;


      if (data.length() > 0) {
        values = split(data, ':');
        if (values.length == 3) {
          distanceLabel.setText("Distance: "+values[0]);
          distanceLabel.setColor(color(0, 0, 0));
          speedLabel.setText("Speed : "+values[1]);
          speedLabel.setColor(color(0, 0, 0));
          distanceCoveredLabel.setText("Total Distance Covered : "+values[2]);
          distanceCoveredLabel.setColorValueLabel(color(0, 0, 0));
        }
      }
    }
  }
  // Draw circular left button with arrow
}



// Helpers
void alignLabels(Textlabel lbl, int y) {
  float labelWidth = lbl.getWidth();
  float centerX = width *1/ 3;
  lbl.setPosition(centerX - labelWidth/2, y);
}
