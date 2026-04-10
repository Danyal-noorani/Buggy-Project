import processing.net.*;
import controlP5.*;
import java.awt.DisplayMode;
import java.awt.GraphicsEnvironment;

// Speed profile
ArrayList<Float> profileTimes  = new ArrayList<Float>();
ArrayList<Float> profileSpeeds = new ArrayList<Float>();
float profileStartTime = -1;
boolean profileRunning = false;
int mseCount = 0;
float mseCumulative = 0;
float finalMSE = 0;
ArrayList<Float> mseHistory = new ArrayList<Float>();
ArrayList<Float> targetSpeedHistory = new ArrayList<Float>();
ArrayList<Float> actualSpeedTimes  = new ArrayList<Float>();
ArrayList<Float> actualSpeedValues = new ArrayList<Float>();

float targetSpeedValue = 0;
float currentMSE = 0;
int maxMSEPoints = 100;

// Graph position
float graphX = 850;
float graphY = 30;
float graphW = 380;
float graphH = 220;

// Car variables
float carX = 0;
float previousCarX = 0;
float carSpeed = 20;
float wheelAngle = 0;
float carY = -50;
float targetCarY;
float carSpeedY = 5;

// Client
Client myClient;
boolean connected;

// CP5
ControlP5 cp5;
Textlabel distanceLabel, speedLabel, distanceCoveredLabel, slowDownFactorLabel, obstacleLabel, movementStatusLabel, wifiStatusLabel;
Button turnLeft, turnRight, adjustLeft, adjustRight, sendNextDistance, sendNextTime, sendNextSpeed;
Textfield nextDistance, nextTime, nextSpeed;
String[] values = {"50", "1", "1"};
boolean stopped = false;

int btnRadius = 50;
float leftX = 150, leftY = 600;
float rightX = 300, rightY = 600;

void settings() {
  int baseWidth = 1600;
  int baseHeight = 700;
  int density = displayDensity();
  size(baseWidth * density, baseHeight * density);
}

void setup() {
  carY = height - 130;
  myClient = new Client(this, "192.168.4.1", 5200);
  cp5 = new ControlP5(this);
  PFont labelFont = createFont("Space Grotesk", 20);
  PFont btnFont   = createFont("Space Grotesk", 24);

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

  nextTime = cp5.addTextfield("TimeInput")
    .setPosition(600, 40)
    .setFont(btnFont)
    .setSize(200, 50)
    .setAutoClear(false)
    .setLabel("Time (s)");
  nextTime.getCaptionLabel().setColor(color(0));

  nextSpeed = cp5.addTextfield("SpeedInput")
    .setPosition(350, 40)
    .setFont(btnFont)
    .setSize(200, 50)
    .setAutoClear(false)
    .setLabel("Speed (cm/s)");
  nextSpeed.getCaptionLabel().setColor(color(0));

  sendNextSpeed = cp5.addButton("SendSpeedInput")
    .setPosition(350, 130)
    .setFont(btnFont)
    .setSize(260, 50)
    .setLabel("Send Speed & Time");

  turnLeft = cp5.addButton("TurnLeft")
    .setPosition(400, 400)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("Turn Left");

  turnRight = cp5.addButton("TurnRight")
    .setPosition(600, 400)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("Turn Right");

  adjustLeft = cp5.addButton("AdjustLeft")
    .setPosition(400, 460)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("\u21E6");

  adjustRight = cp5.addButton("AdjustRight")
    .setPosition(600, 460)
    .setFont(btnFont)
    .setSize(170, 50)
    .setLabel("\u21E8");

  distanceLabel        = cp5.addLabel("Distance").setText("Distance: N/A").setFont(labelFont).setColor(color(255));
  speedLabel           = cp5.addLabel("LiveSpeed").setText("Speed : N/A").setFont(labelFont).setColor(color(255));
  distanceCoveredLabel = cp5.addLabel("DistanceCovered").setText("Total Distance Covered : N/A").setFont(labelFont).setColor(color(255));

  Textlabel[] labelList = {distanceLabel, speedLabel, distanceCoveredLabel};
  for (int i = 0; i < 3; i++) {
    labelList[i].setPosition(20, 180 + (i * 50));
  }

  wifiStatusLabel = cp5.addLabel("WifiStatus")
    .setText("Disconnected").setPosition(20, 20)
    .setColor(color(255, 0, 0)).setFont(labelFont);

  obstacleLabel = cp5.addLabel("ObstacleStatus")
    .setText("No Obstacle Detected").setPosition(20, 60)
    .setColor(color(0, 255, 0)).setFont(labelFont);

  movementStatusLabel = cp5.addLabel("MovementStatus")
    .setText("Stopped").setPosition(20, 100)
    .setColor(color(255, 0, 0)).setFont(labelFont);
}

void resetWidgets() {
  Textlabel[] textList = {distanceLabel, speedLabel, distanceCoveredLabel};
  for (Textlabel i : textList) i.setColor(color(0));

  Controller[] controllerList = {obstacleLabel, movementStatusLabel, distanceLabel, distanceCoveredLabel, speedLabel};
  for (Controller i : controllerList) i.setLock(false);
}

void drawCar(float x, float y) {
  noStroke();
  fill(0, 40);
  ellipse(x + 65, y + 48, 140, 18);

  fill(50, 160, 200);
  rect(x + 5, y + 5, 120, 35, 5);

  fill(60, 175, 215);
  rect(x + 22, y - 18, 78, 28, 6);

  fill(120, 220, 255, 80);
  rect(x + 27, y - 15, 30, 8, 3);

  fill(180, 230, 255, 180);
  quad(x + 93, y - 18, x + 100, y + 5, x + 87, y + 5, x + 82, y - 18);

  fill(180, 230, 255, 150);
  quad(x + 22, y - 18, x + 35, y - 18, x + 42, y + 5, x + 22, y + 5);

  fill(160, 215, 245, 160);
  rect(x + 38, y - 15, 42, 18, 3);

  fill(255, 255, 255, 40);
  rect(x + 5, y + 8, 120, 6, 2);

  fill(255, 255, 200);
  ellipse(x + 122, y + 12, 10, 8);
  fill(255, 240, 100, 60);
  ellipse(x + 126, y + 12, 6, 6);

  fill(220, 50, 50);
  ellipse(x + 8, y + 12, 10, 8);

  stroke(40, 140, 175);
  strokeWeight(1);
  line(x + 42, y + 5, x + 42, y + 38);

  noStroke();
  fill(30, 100, 130);
  rect(x + 10, y + 35, 108, 8, 2);

  wheelAngle += 0.12;
  for (int wx : new int[]{30, 95}) {
    float cx = x + wx;
    float cy = y + 45;

    fill(30);
    ellipse(cx, cy, 34, 34);

    stroke(50);
    strokeWeight(2);
    for (int i = 0; i < 6; i++) {
      float a  = wheelAngle + i * (PI / 3);
      float x1 = cx + cos(a) * 8;
      float y1 = cy + sin(a) * 8;
      float x2 = cx + cos(a) * 16;
      float y2 = cy + sin(a) * 16;
      line(x1, y1, x2, y2);
    }

    noStroke();
    fill(200, 205, 210);
    ellipse(cx, cy, 18, 18);

    stroke(160, 165, 170);
    strokeWeight(2);
    for (int i = 0; i < 5; i++) {
      float a = wheelAngle + i * (TWO_PI / 5);
      line(cx, cy, cx + cos(a) * 8, cy + sin(a) * 8);
    }

    noStroke();
    fill(230, 232, 235);
    ellipse(cx, cy, 7, 7);
  }
  noStroke();
  strokeWeight(1);
}

void draw() {
  background(245, 245, 230);

  float leftBtnX   = turnLeft.getPosition()[0];
  float leftBtnY   = turnLeft.getPosition()[1];
  float leftBtnW   = turnLeft.getWidth();
  float leftCenterX = leftBtnX + leftBtnW / 2;
  fill(71, 185, 222);
  stroke(0);
  beginShape();
  vertex(leftCenterX - 60, leftBtnY - 90);
  vertex(leftCenterX, leftBtnY - 120);
  vertex(leftCenterX, leftBtnY - 105);
  vertex(leftCenterX + 30, leftBtnY - 105);
  vertex(leftCenterX + 30, leftBtnY - 75);
  vertex(leftCenterX, leftBtnY - 75);
  vertex(leftCenterX, leftBtnY - 60);
  endShape(CLOSE);

  float rightBtnX    = turnRight.getPosition()[0];
  float rightBtnY    = turnRight.getPosition()[1];
  float rightBtnW    = turnRight.getWidth();
  float rightCenterX = rightBtnX + rightBtnW / 2;
  fill(71, 185, 222);
  stroke(0);
  beginShape();
  vertex(rightCenterX + 60, rightBtnY - 90);
  vertex(rightCenterX, rightBtnY - 120);
  vertex(rightCenterX, rightBtnY - 105);
  vertex(rightCenterX - 30, rightBtnY - 105);
  vertex(rightCenterX - 30, rightBtnY - 75);
  vertex(rightCenterX, rightBtnY - 75);
  vertex(rightCenterX, rightBtnY - 60);
  endShape(CLOSE);

  connected = (myClient != null && myClient.active());
  if (connected) {
    resetWidgets();
    connectedStatus();
    getLiveData();
  } else {
    greyOutWidgets();
  }

  fill(120);
  rect(0, height - 100, width, 100);

  carX = lerp(previousCarX, carX + carSpeed, 0.1);
  previousCarX = carX;
  carY = lerp(carY, height - 90, 0.1);

  if (carX > width) {
    carX = -150;
    previousCarX = -200;
  }

  drawCar(carX, carY);
  drawMSEGraph();
}

void greyOutWidgets() {
  Controller[] controllerList = {obstacleLabel, movementStatusLabel, distanceLabel, distanceCoveredLabel, speedLabel};
  CColor greyColor = new CColor();
  greyColor.setBackground(color(90));
  greyColor.setForeground(color(90));
  greyColor.setActive(color(90));
  greyColor.setValueLabel(color(90));
  for (Controller i : controllerList) {
    if (i != null) {
      i.setColor(greyColor);
      i.setLock(true);
    }
  }
}

int lastCommandTime = 0;
int commandCoolDown = 250;

void controlEvent(ControlEvent e) {
  if (myClient == null) return;
  if (millis() - lastCommandTime > 50) {
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
      break;
    case "AdjustRight":
      myClient.write("ARIGHT\n");
      break;
    case "SendDistanceInput":
      myClient.write("MOVE:" + nextDistance.getText() + "\n");
      break;
    case "SendSpeedInput":
      String speedText = nextSpeed.getText().trim();
      String timeText  = nextTime.getText().trim();
      if (speedText.length() > 0 && timeText.length() > 0) {
        profileTimes.clear();
        profileSpeeds.clear();
        actualSpeedTimes.clear();
        actualSpeedValues.clear();
        mseHistory.clear();
        targetSpeedHistory.clear();
        mseCumulative = 0;
        mseCount = 0;
        finalMSE = 0;

        String[] speedParts = speedText.split(",");
        String[] timeParts  = timeText.split(",");
        float cursor = 0;
        for (int i = 0; i < speedParts.length; i++) {
          float spd = float(trim(speedParts[i]));
          float dur = (i < timeParts.length) ? float(trim(timeParts[i])) : 5;
          profileTimes.add(cursor);
          profileSpeeds.add(spd);
          cursor += dur;
        }
        profileTimes.add(cursor);
        profileSpeeds.add(profileSpeeds.get(profileSpeeds.size() - 1));
        targetSpeedValue = profileSpeeds.get(0);
        println("MOVET:" + speedText + ":" + timeText + "\n");
        myClient.write("MOVET:" + speedText + ":" + timeText + "\n");
        profileStartTime = millis();
        profileRunning   = true;
      }
      break;
    }
    lastRequestTime = millis() + commandCoolDown;
  }
}

void connectedStatus() {
  fill(255, 255, 255);
  rect(15, 10, 215, 130);
  if (connected) {
    wifiStatusLabel.setText("Connected").setColor(color(0, 255, 0));
  } else {
    wifiStatusLabel.setText("Disconnected").setColor(color(255, 0, 0));
  }
}

int lastRequestTime = 0;
int pollEveryMs = 200;

void getLiveData() {
  if (int(values[0]) > 25) {
    obstacleLabel.setText("No Obstacle Detected").setColor(color(0, 255, 0));
  } else {
    obstacleLabel.setText("Obstacle Detected").setColor(color(255, 0, 0));
  }

  if (connected && ((millis() - lastRequestTime) > pollEveryMs)) {
    lastRequestTime = millis();
    if (myClient != null) myClient.write("GET_DATA\n");
  }

  if (myClient != null && myClient.available() > 0) {
    String incoming = myClient.readStringUntil('\n');
    if (incoming != null) {
      incoming = trim(incoming);
      if (incoming.length() > 0) {
        values = split(incoming, ':');
        if (values.length == 3) {
          distanceLabel.setText("Distance: " + values[0]);
          distanceLabel.setColor(color(0, 0, 0));

          float actualSpeed = float(values[1]);

          if (profileRunning && profileTimes.size() > 0) {
            float elapsed = (millis() - profileStartTime) / 1000.0;
            targetSpeedValue = profileSpeeds.get(0);
            for (int i = profileTimes.size() - 1; i >= 0; i--) {
              if (elapsed >= profileTimes.get(i)) {
                targetSpeedValue = profileSpeeds.get(i);
                break;
              }
            }
          }

          speedLabel.setText("Speed : " + actualSpeed);

         float error = actualSpeed - targetSpeedValue;
          currentMSE  = error * error;
         
          mseHistory.add(currentMSE);
          targetSpeedHistory.add(targetSpeedValue);
         
                if (profileRunning) {
        mseCumulative += currentMSE;
        mseCount++;
       
        float elapsed = (millis() - profileStartTime) / 1000.0;
        float totalDuration = profileTimes.get(profileTimes.size() - 1);
        if (elapsed >= 0 && elapsed <= totalDuration + 4) {
          actualSpeedTimes.add(elapsed);
          actualSpeedValues.add(actualSpeed);
        } else if (elapsed > totalDuration + 4) {
          profileRunning = false;
          finalMSE = (mseCount > 0) ? mseCumulative / mseCount : 0;
        }
      }

          if (mseHistory.size() > maxMSEPoints) {
            mseHistory.remove(0);
            targetSpeedHistory.remove(0);
          }

          speedLabel.setColor(color(0, 0, 0));
          distanceCoveredLabel.setText("Total Distance Covered : " + values[2]);
          distanceCoveredLabel.setColorValueLabel(color(0, 0, 0));
        }
      }
    }
  }
}

void alignLabels(Textlabel lbl, int y) {
  float labelWidth = lbl.getWidth();
  float centerX    = width * 1 / 3;
  lbl.setPosition(centerX - labelWidth / 2, y);
}

void drawMSEGraph() {
  fill(255, 245);
  stroke(0);
  rect(graphX + 20, graphY, graphW + 60, graphH + 215, 12);

  fill(0);
  textSize(16);
  text("Buggy speed in response to the given profile", graphX + 95, graphY + 25);

  fill(0);
  textSize(13);
  text("Time (s)", graphX + graphW / 2 + 45, graphY + graphH + 207);
  pushMatrix();
  translate(graphX + 37, graphY + graphH / 2 + 130);
  rotate(-HALF_PI);
  text("Speed (cm/s)", 0, 0);
  popMatrix();

  float left   = graphX + 45;
  float right  = graphX + graphW - 20;
  float top    = graphY + 40;
  float bottom = graphY + graphH - 35;
  float plotLeft   = left   + 20;
  float plotRight  = right  + 90;
  float plotTop    = top    + 10;
  float plotBottom = bottom + 200;

  float xMax = 10;
  if (profileTimes.size() > 0) {
    float lastT = profileTimes.get(profileTimes.size() - 1);
    xMax = max(lastT * 1.2, 10);
  }

  stroke(0);
  strokeWeight(1);
  line(plotLeft, plotBottom, plotRight, plotBottom);
  line(plotLeft, plotTop, plotLeft, plotBottom);

  textSize(12);
  for (int yVal = 0; yVal <= 50; yVal += 5) {
    if (yVal == 0) continue;
    float y = map(yVal, 0, 50, plotBottom, plotTop);
    stroke(220);
    line(plotLeft, y, plotRight, y);
    stroke(0);
    line(plotLeft, y, plotLeft - 5, y);
    fill(0);
    text(yVal, plotLeft - 25, y + 4);
  }

  textSize(12);
  float tickStep = (xMax <= 15) ? 2 : (xMax <= 30) ? 5 : (xMax <= 60) ? 10 : 15;
  for (float xVal = 0; xVal <= xMax; xVal += tickStep) {
    float x = map(xVal, 0, xMax, plotLeft, plotRight);
    stroke(220);
    line(x, plotTop, x, plotBottom);
    stroke(0);
    line(x, plotBottom, x, plotBottom + 5);
    fill(0);
    text((int)xVal, x - 8, plotBottom + 18);
  }

if (profileTimes.size() >= 2) {
  stroke(30, 144, 255);
  strokeWeight(2);
  noFill();
  beginShape();
  float x0 = map(profileTimes.get(0), 0, xMax, plotLeft, plotRight);
  float y0 = map(profileSpeeds.get(0), 0, 50, plotBottom, plotTop);
  vertex(x0, y0);
  for (int i = 1; i < profileTimes.size(); i++) {
    float x    = map(profileTimes.get(i),     0, xMax, plotLeft, plotRight);
    float xPrev = map(profileTimes.get(i - 1), 0, xMax, plotLeft, plotRight);
    float y    = map(profileSpeeds.get(i),     0, 50, plotBottom, plotTop);
    float yPrev = map(profileSpeeds.get(i - 1), 0, 50, plotBottom, plotTop);
    vertex(xPrev, yPrev); // start of this segment at previous speed
    vertex(x, yPrev);     // horizontal across to next time marker
    vertex(x, y);         // vertical drop/rise to new speed
  }
  endShape();

    noStroke();
    fill(30, 144, 255);
    for (int i = 0; i < profileTimes.size() - 1; i++) {
      float x = map(profileTimes.get(i), 0, xMax, plotLeft, plotRight);
      float y = map(profileSpeeds.get(i), 0, 50, plotBottom, plotTop);
      ellipse(x, y, 7, 7);
    }
  }

  if (actualSpeedValues.size() >= 2) {
    int smoothWindow = 5;
    stroke(230, 30, 140);
    strokeWeight(2);
    noFill();
    beginShape();
    for (int i = 0; i < actualSpeedValues.size(); i++) {
      float smoothed = 0;
      int count = 0;
      for (int j = max(0, i - smoothWindow); j <= min(actualSpeedValues.size() - 1, i + smoothWindow); j++) {
        smoothed += actualSpeedValues.get(j);
        count++;
      }
      smoothed /= count;
      float x = map(actualSpeedTimes.get(i), 0, xMax, plotLeft, plotRight);
      float y = map(smoothed, 0, 50, plotBottom, plotTop);
      vertex(x, y);
    }
    endShape();
  } else {
    fill(120);
    textSize(13);
    text("Waiting for speed data...", plotLeft + 20, (plotTop + plotBottom) / 2);
  }

  float legendX = plotRight - 140;
  float legendY = plotTop + 10;
  fill(255, 245);
  stroke(0);
  strokeWeight(1);
  rect(legendX - 5, legendY - 5, 140, 52, 4);

  stroke(30, 144, 255);
  strokeWeight(2);
  line(legendX, legendY + 7, legendX + 20, legendY + 7);
  fill(30, 144, 255);
  noStroke();
  ellipse(legendX + 10, legendY + 7, 6, 6);
  fill(0);
  textSize(12);
  text("Reference profile", legendX + 25, legendY + 11);

  stroke(230, 30, 140);
  strokeWeight(2);
  line(legendX, legendY + 25, legendX + 7, legendY + 25);
  line(legendX + 11, legendY + 25, legendX + 18, legendY + 25);
  fill(0);
  noStroke();
  text("Buggy Speed", legendX + 25, legendY + 29);

  fill(0);
  textSize(13);
  text("Target Speed: " + nf(targetSpeedValue, 1, 1) + " cm/s", graphX + 25, graphY + graphH + 230);
  text("MSE: " + nf(finalMSE, 1, 2), graphX + 25, graphY + graphH + 248);
  strokeWeight(1);
}
