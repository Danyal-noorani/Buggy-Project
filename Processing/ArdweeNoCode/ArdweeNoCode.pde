import processing.net.*;
import controlP5.*;
import java.awt.DisplayMode;
import java.awt.GraphicsEnvironment;
// Client
Client myClient;
boolean connected;

// CP5
ControlP5 cp5;
Slider sBalance, sSpeed, sTurnMult, sSlowDown;
Textlabel distanceLabel, leftSensorLabel, rightSensorLabel, speedLabel, distanceCoveredLabel, turningMultiplierLabel, balanceLabel, slowDownFactorLabel, obstacleLabel, movementStatusLabel, wifiStatusLabel;
Button start, stop, sendValues;
CColor defaultSliderColor;


String[] values ={"50", "1", "1"};
boolean stopped=false;

//Scaling Factor for different screen resolution
float s;

void settings() {
  int baseWidth = 1280;
  int baseHeight = 720;

  int density = displayDensity();  // 1 for normal, 2 for HiDPI
  size(baseWidth * density, baseHeight * density);
}


void setup() {

  myClient = new Client(this, "nONe", 5200);
  cp5 = new ControlP5(this);

  // --- Font Styles ---
  PFont labelFont = createFont("Adwaita Mono", 20);
  PFont sliderCaptionFont = createFont("Adwaita Mono", 14);
  PFont sliderLabelFont = createFont("Adwaita Mono", 14);
  PFont btnFont = createFont("Adwaita Mono", 24);


  // --- Labels ---
  distanceLabel = cp5.addLabel("Distance").setText("Distance: N/A").setFont(labelFont).setColor(color(255));
  leftSensorLabel = cp5.addLabel("L_IR").setText("Left IR Sensor: N/A").setFont(labelFont).setColor(color(255));
  rightSensorLabel = cp5.addLabel("R_IR").setText("Right IR Sensor: N/A").setFont(labelFont).setColor(color(255));
  speedLabel = cp5.addLabel("LiveSpeed").setText("Speed : N/A").setFont(labelFont).setColor(color(255));
  distanceCoveredLabel =cp5.addLabel("DistanceCovered").setText("Total Distance Covered : N/A").setFont(labelFont).setColor(color(255));

  Textlabel[] labelList= {distanceLabel, leftSensorLabel, rightSensorLabel, speedLabel, distanceCoveredLabel};
  int labelPadding = 40;
  for (int i =0; i<5; i++) {
    alignLabels(labelList[i], 70 + (i*labelPadding) ); //sets the position of the labels with given padding
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

  // ---- Vertical sliders ----
  int sliderW = 30;
  int sliderH = 180;
  int gap = 70;
  int totalWidth = sliderW * 4 + gap * 3;
  int startX = (width - totalWidth)* 9/ 10;
  int startY = height*1 / 4 - 160;

  sSpeed = cp5.addSlider("Speed")
    .setPosition(startX + (sliderW + gap), startY)
    .setSize(sliderW, sliderH)
    .setRange(0, 100)
    .setValue(55);

  sBalance = cp5.addSlider("Balance")
    .setPosition(startX, startY)
    .setSize(sliderW, sliderH)
    .setRange(0.0, 1.0)
    .setValue(0.93);

  sTurnMult = cp5.addSlider("TurningMultiplier")
    .setLabel("Turning\nMultiplier")
    .setPosition(startX + (sliderW + gap) * 2, startY)
    .setSize(sliderW, sliderH)
    .setRange(0.0, 1.0)
    .setValue(0.14);

  sSlowDown = cp5.addSlider("SlowDownFactor")
    .setLabel("Slow Down\nFactor")
    .setPosition(startX + (sliderW + gap) * 3, startY)
    .setSize(sliderW, sliderH)
    .setRange(0.0, 1.0)
    .setValue(0.65);

  Controller[] sliders = {
    sBalance, sSpeed, sTurnMult, sSlowDown
  };

  for (Controller s : sliders) { //Sets the Text Properties on the sliders

    s.getCaptionLabel()
      .setSize(16)
      .setColor(color(255))
      .setPaddingY(10).setFont(sliderCaptionFont);


    s.getValueLabel()
      .setSize(14)
      .setColor(color(255)).setFont(sliderLabelFont);
  }


  //--- Start/Stop Buttons ---
  int btnWidth = 200;
  int btnHeight = 60;
  int centerX = width/2 - btnWidth - 10;
  int stopX   = width/2 + 10;
  int buttonY = 520;

  start = cp5.addButton("START")
    .setPosition(centerX, buttonY)
    .setSize(btnWidth, btnHeight)
    .setFont(btnFont);

  stop = cp5.addButton("STOP")
    .setPosition(stopX, buttonY)
    .setSize(btnWidth, btnHeight)
    .setFont(btnFont);

  sendValues = cp5.addButton("SEND_VAL")
    .setLabel("Send Values")
    .setPosition(startX+100, startY+250)
    .setSize(btnWidth, btnHeight)
    .setFont(btnFont);


  Slider tempSlider = cp5.addSlider("temp").setPosition(-100, -100).setSize(100, 20); //Temporary slider to get and save default color scheme
  defaultSliderColor = tempSlider.getColor();
  cp5.remove("temp");
}


// Resets Button function and colors
void resetWidgets() {

  stop.setColorBackground(color(190, 0, 0))
    .setColorForeground(color(255, 0, 0))
    .setColorActive(color(100, 0, 0));

  start.setColorBackground(color(0, 190, 0))
    .setColorForeground(color(0, 255, 0))
    .setColorActive(color(0, 100, 0));

  Slider[] sliderList = {sSpeed, sBalance, sTurnMult, sSlowDown};

  for (Slider i : sliderList) {
    i.setColor(defaultSliderColor);
  }

  Textlabel[] textList ={distanceLabel, rightSensorLabel, leftSensorLabel, speedLabel, distanceCoveredLabel};

  for ( Textlabel i : textList) {
    i.setColor(color(255));
  }

  Controller[] controllerList= {obstacleLabel, movementStatusLabel, distanceLabel, rightSensorLabel, leftSensorLabel, distanceCoveredLabel, speedLabel, stop, start, sSpeed, sBalance, sTurnMult, sSlowDown};

  for ( Controller i : controllerList) {
    i.setLock(false);
  }
}

void draw() {
  background(22);
  connected = (myClient != null && myClient.active());
  // if client is not connected then grey out everything
  if (!connected) {
    resetWidgets();
    connectedStatus();
    getLiveData();
  } else {

    greyOutWidgets();
  }
}

void greyOutWidgets() {
  Controller[] controllerList= {obstacleLabel, movementStatusLabel, distanceLabel, rightSensorLabel, leftSensorLabel, distanceCoveredLabel, speedLabel, stop, start, sSpeed, sBalance, sTurnMult, sSlowDown};
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
  // --------------------------------------------- UPDATES ---------------------------------------------

  if (myClient == null) return;
  if (millis()-lastCommandTime > 50) {
    lastCommandTime = millis();
    switch(e.getName()) {


    case "STOP":
      stopped = true;
      myClient.write("STOP\n");
      break;

    case "SEND_VAL":
      println("Somehi");
      myClient.write("VALUES:" +nf(sSpeed.getValue(), 1, 0)+":"+ nf(sBalance.getValue(), 1, 2)+":"+nf(sTurnMult.getValue(), 1, 2)+":"+nf(sSlowDown.getValue(), 1, 2) +"\n");
      break;
    }
    lastRequestTime = millis() + commandCoolDown;
  }
  // --------------------------------------------- UPDATES ---------------------------------------------
}


void connectedStatus() {

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
          leftSensorLabel.setText("Left IR Sensor: "+values[1]);
          rightSensorLabel.setText("Right IR Sensor: "+values[2]);
          speedLabel.setText("Speed : "+values[3]);
          distanceCoveredLabel.setText("Total Distance Covered : "+values[4]);
        }
      }
    }
  }
  buildGraphics(int(values[0]), int(values[1]), int(values[2]));
}




// Helpers
void alignLabels(Textlabel lbl, int y) {
  float labelWidth = lbl.getWidth();
  float centerX = width *1/ 3;
  lbl.setPosition(centerX - labelWidth/2, y);
}

void buildGraphics(int distance, int L_IR, int R_IR) {

  float centerX = width/2;
  float centerY = height/2;
  float hexSize = 60;

  // Left triangle
  if (L_IR==0&&R_IR==1) {
    fill(255, 255, 0);
  } else {
    noFill();
  }
  stroke(255, 255, 0);
  triangle(centerX - 150 - hexSize, centerY, centerX - 50 - hexSize, centerY - 40, centerX - 50 -hexSize, centerY + 40);

  // Right triangle
  if (R_IR==0&&L_IR==1) {
    fill(255, 255, 0);
  } else {
    noFill();
  }
  stroke(255, 255, 0);
  triangle(centerX + 150 + hexSize, centerY, centerX + 50+hexSize, centerY - 40, centerX + 50 + hexSize, centerY + 40);



  if ((L_IR==0&&R_IR==0)||distance<25||stopped) {
    movementStatusLabel.setText("Stopped").setColor(color(255, 0, 0));
    fill(255, 0, 0);
    stroke(255, 0, 0);
  } else {
    movementStatusLabel.setText("Moving").setColor(color(0, 255, 0));
    stroke(0, 255, 0);
    noFill();
  }

  drawHexagon(centerX, centerY, hexSize);
}

void drawHexagon(float x, float y, float radius) {
  beginShape();
  for (int i = 0; i < 8; i++) {
    float angle = TWO_PI / 8 * i - PI/8;
    float vx = x + cos(angle) * radius;
    float vy = y + sin(angle) * radius;
    vertex(vx, vy);
  }
  endShape(CLOSE);
}
