#include "motors.h"
#include "encoder.h"
#include "ultraSonic.h"

static byte shiftIn(int myDataPin, int myClockPin);
static void IncrementCounter();
static void updateSpeed();
static void updateTotalDistance();
static void updateCalibrateMotors();

static volatile int leftCounter = 0;
static volatile long sinceLastSpeedUpdate = 0;
static volatile double realSpeed = 0.0;
static volatile long sinceLastPulse = 0;

static long sinceLastCalibration = 0;
static double totalDistance = 0.0;

static int requiredPulses = 0;
static int initialRightCounter = 0;
static int initialLeftCounter = 0;

static MoveMode encoderMode = STOPMOVE;

void hallSensorsSetup()
{
    // Left sensor — interrupt on both edges (8 pulses/rev)
    pinMode(LeftSensor, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(LeftSensor), IncrementCounter, CHANGE);

    // Right sensor — shift-register IC (4 pulses/rev)
    pinMode(ClockPin, OUTPUT);
    pinMode(LatchPin, OUTPUT);
    pinMode(DataPin, INPUT);
    pinMode(ResetPin, OUTPUT);

    // Reset IC counter on startup
    digitalWrite(ResetPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(ResetPin, LOW);

    sinceLastCalibration = sinceLastPulse = sinceLastSpeedUpdate = millis();
}

void hallSensorsLoop()
{
    long currentMillis = millis();

    if (currentMillis - sinceLastCalibration > 100)
    {
        updateCalibrateMotors(); // update motor balance every second
        sinceLastCalibration = currentMillis;
    }

    // resets speed after 1 second
    if (currentMillis - sinceLastSpeedUpdate > 1000)
        realSpeed = 0;

    if (!getStop() || encoderMode == TURN)
    {
        switch (encoderMode)
        {
        case FORWARD:
        {
            // Both counters at 4 pulses/rev for direct comparison
            int lDiff = getLCounter() - initialLeftCounter;
            int rDiff = getRCounter() - initialRightCounter;

            // Serial.print(rDiff + initialRightCounter);
            // Serial.println(lDiff + initialLeftCounter);

            if (rDiff < requiredPulses && lDiff < requiredPulses)
            {
                if ((requiredPulses - rDiff) < 2 || (requiredPulses - lDiff) < 2)
                {

                    // Slow down in final 2 pulses to reduce overshoot
                    setMotors(40, 40);
                }
                else
                {
                    setMotors(55, 55);
                }
            }
            else
            {
                brake();
                stopMotors();
                encoderMode = STOPMOVE;
                initialLeftCounter = 0;
                initialRightCounter = 0;
            }
            break;
        }

        case TURN:
        {
            int lDiff = getLCounter(true) - initialLeftCounter;
            int rDiff = getRCounter() - initialRightCounter;
            int rDiffHighRes = rDiff * 2;            // scale right to 8 pulse/rev
            int leadDiff = max(rDiffHighRes, lDiff); // stop on whichever leads to prevent overshoot

            if (leadDiff >= requiredPulses)
            {
                stopMotors();
                encoderMode = STOPMOVE;
                initialLeftCounter = 0;
                initialRightCounter = 0;
                setMotorForward(0);
                setMotorForward(1);
            }
            else
            {
                if (requiredPulses - leadDiff <= 2)
                    setMotors(35, 35);
                else
                    setMotors(55, 55);
            }
            break;
        }

        case STOPMOVE:
        default:
            break;
        }
    }
    else
    {
        stopMotors();
    }
}

// fires on every CHANGE edge of the left sensor
static void IncrementCounter()
{
    leftCounter += 1;
    updateSpeed();
    sinceLastPulse = millis();
    if (encoderMode == FORWARD)
    {
        updateTotalDistance();
    }
}

// 90° turn
void turnDirection(int direction)
{
    initialLeftCounter = getLCounter(true);
    initialRightCounter = getRCounter();
    setMotorBackward(direction);
    requiredPulses = (int)ceil((PI * WHEELBASE_CM / 4.0f) / DistancePerPulseHighRes);
    encoderMode = TURN;
}
void adjustDirection(int direction)
{
    initialLeftCounter = getLCounter(true);
    initialRightCounter = getRCounter();
    setMotorBackward(direction);
    requiredPulses = 1;
    encoderMode = TURN;
}

void moveDistance(int distance)
{
    requiredPulses = ceil(distance / DistancePerPulse);
    encoderMode = FORWARD;
    initialLeftCounter = getLCounter();
    initialRightCounter = getRCounter();
}

static void updateSpeed()
{
    long currentMillis = millis();
    // Each ISR fires once per CHANGE edge = DistancePerPulseHighRes (2.55 cm) traveled
    realSpeed = DistancePerPulseHighRes * 1000.0f / (currentMillis - sinceLastPulse);
    sinceLastSpeedUpdate = currentMillis;
}

static void updateTotalDistance()
{
    totalDistance += DistancePerPulse / 2; // ISR fires twice per low-res pulse
}

double getAverageCounter() { return (getLCounter() + getRCounter()) / 2.0; }
double getTotalDistance() { return totalDistance; }
double getSpeed() { return realSpeed; }

// true → raw 8 pulse/rev; false → divided by 2 to match right encoder (4 pulse/rev)
int getLCounter(bool highRes)
{
    return highRes ? leftCounter : leftCounter / 2;
}

// Latch counter IC then clock out byte via shift register
int getRCounter()
{
    digitalWrite(LatchPin, HIGH);
    delayMicroseconds(40);
    digitalWrite(LatchPin, LOW);
    return (int)shiftIn(DataPin, ClockPin);
}

// Adjusts motor PWM balance to keep both wheels at equal speed (constrained to ±20%)
static void updateCalibrateMotors()
{
    int L = getLCounter();
    int R = getRCounter();
    if (L == 0 || R == 0)
        return;
    // L/R: if right is faster (R > L), ratio < 1, reducing right motor PWM to compensate
    float ratio = (float)L / (float)R;
    setMotorBalance(ratio);
}

// byte from the shift register
static byte shiftIn(int myDataPin, int myClockPin)
{
    byte myDataIn = 0;

    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(myClockPin, 0);
        delayMicroseconds(1);
        if (digitalRead(myDataPin))
            myDataIn |= (1 << i);
        digitalWrite(myClockPin, 1);
    }

    return myDataIn;
}
