#include "encoder.h"
#include "motors.h"
volatile int leftCounter = 0;
int rightCounter = 0;

byte shiftIn(int myDataPin, int myClockPin);
void IncrementCounter();
void hallSensorsSetup()
{
    // -- LEFT SENSOR --
    // Pullup Input
    pinMode(LeftSensor, INPUT_PULLUP);

    // Attach Interrupt
    attachInterrupt(digitalPinToInterrupt(LeftSensor), IncrementCounter, RISING); // Rising as analogue only takes input on HIGH

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

void IncrementCounter()
{
    leftCounter += 1;
}

int getLCounter()
{
    return leftCounter;
}
int getRCounter()
{
    digitalWrite(LatchPin, HIGH);
    delayMicroseconds(40);
    digitalWrite(LatchPin, LOW);
    return (int)shiftIn(DataPin, ClockPin);
}

void hallSensorsLoop()
{
    Serial.print("Left Counter: ");
    Serial.print(getLCounter());
    Serial.print(" Right Counter: ");
    Serial.println(getRCounter(), DEC);
}

void turnDirection(int direction, int speed, int degrees)
{
    int initialLeftCounter = getLCounter();
    int initialRightCounter = getRCounter();
    int pulses = ceil(degrees / 45) + 1;
    if (direction = TURN_LEFT)
    {
        setMotorBackward(1);
        setMotors(speed, speed);
    }
    else if (direction = TURN_RIGHT)
    {
        setMotorBackward(0);
        setMotors(speed, speed);
    }
    while ((getLCounter() - initialLeftCounter) < pulses && (getRCounter() - initialRightCounter) < pulses)
    {
        continue;
    }
    Serial.print("Left Counter: ");
    Serial.print(getLCounter());
    Serial.print(" Right Counter: ");
    Serial.println(getRCounter(), DEC);
    setMotorForward(0);
    setMotorForward(1);
    stopMotors();
}

void moveDistance(int distance, int speed)
{
    int pulses = ceil(distance / DistancePerPulse);
    int initialLeftCounter = getLCounter();
    int initialRightCounter = getRCounter();
    setMotors(speed, speed);
    while (getLCounter() - initialLeftCounter < pulses && getRCounter() - initialRightCounter < pulses)
    {
        Serial.print("Left Counter: ");
        Serial.print(getLCounter());
        Serial.print(" Right Counter: ");
        Serial.println(getRCounter(), DEC);
        continue;
    }
    stopMotors();
}

double getAverageCounter()
{
    return (getLCounter() + getRCounter()) / 2.0;
}

double getTotalDistance()
{
    return DistancePerPulse * getAverageCounter();
}

double getSpeed()
{
    return 0;
}
// int milisec , int previousCounterVal (average previous count) ,
// wait for X mills, then calculate speed using int currentCounterVal(average current counter) / X mills

// int getSpeed

// From Given Example
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
