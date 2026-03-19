#include "motion.h"
#include "encoder.h"
#include "motors.h"
#include "ultraSonic.h"

static float targetSpeed = 0.0f; // cm/s
static float motorPower = 0.0f;  // 0–100 PWM
static float integral = 0.0f;
static bool moving = false; // true during a moveAtSpeed call
static int targetPulses = 0;
static int startLeftCounter = 0;
static int startRightCounter = 0;
static long lastLoopMs = 0;

// Set a continuous target speed (cm/s). Pass 0 to stop.
void setTargetSpeed(float cmPerSec)
{
    targetSpeed = cmPerSec;
    if (cmPerSec == 0.0f)
    {
        integral = 0.0f;
        motorPower = 0.0f;
    }
}

// Drive distanceCm at cmPerSec, then stop automatically.
void moveAtSpeed(float cmPerSec, int distanceCm)
{
    targetSpeed = cmPerSec;
    targetPulses = (int)ceil(distanceCm / DistancePerPulse);
    startLeftCounter = getLCounter();
    startRightCounter = getRCounter();
    motorPower = 0.0f;
    integral = 0.0f;
    lastLoopMs = millis();
    moving = true;
}

bool motionBusy() { return moving; }

// Call this from loop(). Runs the PI controller and handles distance stopping.
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
    if (dt < 0.005f)
        return; // skip if called faster than 5 ms to avoid integral noise
    lastLoopMs = now;

    // Distance check — stop when either wheel reaches the target
    if (moving)
    {
        int lDiff = getLCounter() - startLeftCounter;
        int rDiff = getRCounter() - startRightCounter;
        if (max(lDiff, rDiff) >= targetPulses)
        {
            brake();
            stopMotors();
            moving = false;
            targetSpeed = 0.0f;
            motorPower = 0.0f;
            integral = 0.0f;
            return;
        }
    }

    // PI controller: error between desired and measured speed
    float error = targetSpeed - getSpeed();
    integral = constrain(integral + error * dt, -30.0f, 30.0f); // clamp to prevent windup
    motorPower = constrain(MOTION_KP * error + MOTION_KI * integral, 0.0f, 100.0f);

    setMotors((int)motorPower, (int)motorPower);
}
