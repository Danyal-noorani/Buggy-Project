import processing.net.*;
import controlP5.*;
import java.awt.DisplayMode;
import java.awt.GraphicsEnvironment;
// Client
Client myClient;
boolean connected;

// CP5
ControlP5 cp5;
Textlabel distanceLabel, speedLabel, distanceCoveredLabel, slowDownFactorLabel, obstacleLabel, movementStatusLabel, wifiStatusLabel;
Button turnLeft, turnRight, sendNextDistance;
Textfield nextDistance;
String[] values ={"50", "1", "1"};
boolean stopped=false;


// Radius of circular buttons
int btnRadius = 50;

// Center positions
float leftX = 150, leftY = 600;
float rightX = 300, rightY = 600;

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
    .setPosition(200, 400)
    .setFont(btnFont)
    .setSize(200, 50)
    .setAutoClear(false)
    .setLabel("Distance (cm)");


  sendNextDistance = cp5.addButton("SendDistanceInput")
    .setPosition(215, 500)
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



  // --- Labels ---
  distanceLabel = cp5.addLabel("Distance").setText("Distance: N/A").setFont(labelFont).setColor(color(255));
  speedLabel = cp5.addLabel("LiveSpeed").setText("Speed : N/A").setFont(labelFont).setColor(color(255));
  distanceCoveredLabel =cp5.addLabel("DistanceCovered").setText("Total Distance Covered : N/A").setFont(labelFont).setColor(color(255));

  Textlabel[] labelList= {distanceLabel, speedLabel, distanceCoveredLabel};
  int labelPadding = 40;
  for (int i =0; i<3; i++) {
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
}


// Resets Button function and colors
void resetWidgets() {
  Textlabel[] textList ={distanceLabel, speedLabel, distanceCoveredLabel};

  for ( Textlabel i : textList) {
    i.setColor(color(255));
  }

  Controller[] controllerList= {obstacleLabel, movementStatusLabel, distanceLabel, distanceCoveredLabel, speedLabel};

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
    }
    lastRequestTime = millis() + commandCoolDown;
  }
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
          speedLabel.setText("Speed : "+values[1]);
          distanceCoveredLabel.setText("Total Distance Covered : "+values[2]);
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
