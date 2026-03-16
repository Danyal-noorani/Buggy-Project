#include "motors.h"
#include "encoder.h"

byte shiftIn(int myDataPin, int myClockPin);
void IncrementCounter();
void updateSpeed();
void updateTotalDistance();

volatile int leftCounter = 0;
int rightCounter = 0;

long sinceLastSpeedUpdate = millis();
double realSpeed = 0.0;

long sinceLastPulse = sinceLastSpeedUpdate;

double totalDistance = 0.0;

int requiredPulses = 0;
int initialRightCounter = 0;
int initialLeftCounter = 0;

MoveMode encoderMode = STOPMOVE;

void hallSensorsSetup()
{
    // -- LEFT SENSOR --
    // Pullup Input
    pinMode(LeftSensor, INPUT_PULLUP);

    // Attach Interrupt
    attachInterrupt(digitalPinToInterrupt(LeftSensor), IncrementCounter, CHANGE); // Rising as analogue only takes input on HIGH

    // -- RIGHT SENSOR --
    pinMode(ClockPin, OUTPUT);
    pinMode(LatchPin, OUTPUT);
    pinMode(DataPin, INPUT);
    pinMode(ResetPin, OUTPUT);

    // Resets encoder everytime Arduino is turned on
    digitalWrite(ResetPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(ResetPin, LOW);
}

void hallSensorsLoop()
{

    long currentMillis = millis();
    if (sinceLastSpeedUpdate + 1000 > currentMillis)
    {
        realSpeed = 0;
    }

    switch (encoderMode)
    {

    case FORWARD:
    {
        int lDiff = getLCounter() - initialLeftCounter;
        int rDiff = getRCounter() - initialRightCounter;
        Serial.println(lDiff);
        Serial.println(rDiff);
        Serial.println(requiredPulses);
        if (rDiff < requiredPulses && lDiff < requiredPulses)
        {
            if (rDiff - requiredPulses <= 2 || lDiff - requiredPulses <= 2)
            {
                setMotors(35, 35);
            }
            else
            {

                setMotors(35, 35);
            }
        }
        else
        {
            stopMotors();
            encoderMode = STOPMOVE;
            initialLeftCounter = 0;
            initialRightCounter = 0;
        }
        break;
    }

    case TURN:
    {
        // Serial.println("TURNING");
        Serial.println(getLCounter(true));

        if (getLCounter(true) - initialLeftCounter > requiredPulses)
        {
            stopMotors();
            encoderMode = STOPMOVE;
            initialLeftCounter = 0;
            setMotorForward(0);
            setMotorForward(1);
        }
        break;
    }
    case STOPMOVE:
        break;

    default:
        break;
    }
}

void IncrementCounter()
{
    leftCounter += 1;
    if (encoderMode == FORWARD) // Updates Speed and distance only if moving forward
    {
        updateSpeed();
        updateTotalDistance();
    }
}

void turnDirection(int direction)
{
    initialLeftCounter = getLCounter();
    Serial.println(initialLeftCounter);

    requiredPulses = 4;

    if (direction == 0) // Turn Left
    {
        setMotorBackward(1);
    }
    else if (direction == 1) // Turn Right
    {
        setMotorBackward(0);
    }
    setMotors(35, 35);
    encoderMode = TURN;
}

void moveDistance(int distance)
{
    requiredPulses = ceil(distance / DistancePerPulse);
    Serial.println(requiredPulses);
    encoderMode = FORWARD;
    initialLeftCounter = getLCounter();
    initialRightCounter = getRCounter();
}

void updateSpeed()
{
    long currentMillis = millis();
    realSpeed = DistancePerPulse / (sinceLastPulse - currentMillis);
    sinceLastPulse = currentMillis;
}

void updateTotalDistance()
{
    totalDistance += DistancePerPulse;
}

double getAverageCounter()
{
    return (getLCounter() + getRCounter()) / 2.0;
}

double getTotalDistance()
{
    return totalDistance;
}
double getSpeed()
{
    return realSpeed;
}

int getLCounter(bool highRes)
{
    if (highRes)
        return leftCounter;
    return leftCounter / 2;
}
int getRCounter()
{
    digitalWrite(LatchPin, HIGH);
    delayMicroseconds(40);
    digitalWrite(LatchPin, LOW);
    return (int)shiftIn(DataPin, ClockPin);
}
// ------------------------------------------------------------------------------------------------------------------------
byte shiftIn(int myDataPin, int myClockPin)
{

    int i;

    int temp = 0;

    int pinState;

    byte myDataIn = 0;

    pinMode(myClockPin, OUTPUT);

    pinMode(myDataPin, INPUT);
    // we will be holding the clock pin high 8 times (0,..,7) at the
    // end of each time through the for loop

    // at the beginning of each loop when we set the clock low, it will
    // be doing the necessary low to high drop to cause the shift
    // register's DataPin to change state based on the value
    // of the next bit in its serial information flow.
    // The register transmits the information about the pins from pin 7 to pin 0
    // so that is why our function counts down

    for (i = 7; i >= 0; i--)

    {

        digitalWrite(myClockPin, 0);

        delayMicroseconds(0.2);

        temp = digitalRead(myDataPin);

        if (temp)
        {

            pinState = 1;

            // set the bit to 0 no matter what

            myDataIn = myDataIn | (1 << i);
        }

        else
        {

            // turn it off -- only necessary for debugging

            // print statement since myDataIn starts as 0

            pinState = 0;
        }

        // Debugging print statements

        // Serial.print(pinState);

        // Serial.print("     ");

        // Serial.println (dataIn, BIN);

        digitalWrite(myClockPin, 1);
    }

    // debugging print statements whitespace

    // Serial.println();

    // Serial.println(myDataIn, BIN);

    return myDataIn;
}
