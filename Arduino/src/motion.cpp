#include "motion.h"
#include "encoder.h"
#include "motors.h"
#include "ultraSonic.h"

static float targetSpeed = 0.0f; // cm/s
static float motorPower = 0.0f;  // 0–100 PWM
static float integral = 0.0f;
static float derivative = 0.0f;
static bool moving = false; // true during a moveAtSpeed call
static int targetPulses = 0;
static int startLeftCounter = 0;
static int startRightCounter = 0;
static long lastLoopMs = 0;
static int secondsRequired = 0;
static long initialMillis = 0;
static float previous_error = 0;
static float previous_speed = 0.0f;
static float filtered_error = 0.0f;
const float ALPHA = 0.3f;

// Set a continuous target speed (cm/s). Pass 0 to stop.
void setTargetSpeed(float cmPerSec)
{
    targetSpeed = cmPerSec;
    if (cmPerSec == 0.0f)
    {
        integral = 0.0f;
        derivative = 0.0f;
        motorPower = 0.0f;
    }
}

// Drive distanceCm at cmPerSec, then stop automatically.
void moveAtSpeed(float cmPerSec, int seconds)
{
    targetSpeed = cmPerSec;
    motorPower = 0.0f;
    integral = 0.0f;
    derivative = 0.0f;
    lastLoopMs = millis();
    moving = true;
    secondsRequired = seconds;
    initialMillis = millis();
}

bool motionBusy() { return moving; }

// Runs the PI controller and handles distance stopping.
// NOTE: do not call moveDistance() / hallSensorsLoop() motor control at the same
// time — encoderMode must stay STOPMOVE while motionLoop is active.
void motionLoop()
{

    if (targetSpeed == 0.0f && !moving)
        return;

    // Ultrasonic obstacle override
    if (getStop())
    {
        stopMotors();
        return;
    }

    long now = millis();
    float dt = (now - lastLoopMs) / 1000.0f;
    if (dt < 0.01f)
        return; // skip if called faster than 5 ms to avoid integral noise
    lastLoopMs = now;

    // Distance check — stop when either wheel reaches the target
    if (moving)
    {
        if (secondsRequired < (millis() - initialMillis) / 1000)
        {
            Serial.println(secondsRequired);
            Serial.println((millis() - initialMillis) / 1000);
            brake();
            stopMotors();
            moving = false;
            targetSpeed = 0.0f;
            motorPower = 0.0f;
            integral = 0.0f;
            initialMillis = 0;
            secondsRequired = 0;
            return;
        }
    }

    // PI controller: error between desired and measured speed
    float error = targetSpeed - getSpeed();
    // Serial.println(getSpeed());
    integral = constrain(integral + error * dt, -30.0f, 30.0f); // clamp to prevent windup
    // Calculate derivative on measurement (prevents derivative kick)
    // filtered_error = ALPHA * error + (1.0f - ALPHA) * filtered_error;
    // derivative = (filtered_error - previous_error) / dt;
    // derivative = constrain(derivative, -150.0f, 150.0f);
    derivative = (getSpeed() - previous_speed) / dt;
    derivative = constrain(derivative, -150000.0f, 150000.0f);
    motorPower = constrain(MOTION_KP * error + MOTION_KI * integral + MOTION_KD * derivative, 0.0f, 100.0f) * 1.5;
    Serial.print(integral);
    Serial.print(" ");
    Serial.print(derivative);
    Serial.print(" ");
    Serial.print(error);
    Serial.print(" ");
    Serial.println(motorPower);
    previous_error = error;
    setMotors((int)motorPower, (int)motorPower);
}
