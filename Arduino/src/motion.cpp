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
static float previous_error = 0.0f;
const float ALPHA = 0.3f;
static long sinceLastPid = 0;

// Command list
#define CMD_LIST_SIZE 8
struct MotionCmd
{
    int speed;
    int seconds;
};

static MotionCmd cmdList[CMD_LIST_SIZE] = {{10, 10}, {20, 20}, {30, 30}};
static int index = 3;

void pushMotionCmd(int speed, int seconds)
{
    if (index == 8)
        return;
    cmdList[index].speed = speed;
    cmdList[index].seconds = seconds;
    index++;
}

void popCommand(MotionCmd &cmd)
{
    if (index == 0)
        return;
    index--;
    cmd = cmdList[index];
    Serial.print(cmd.speed);
    Serial.print(" seconds at speed ");
    Serial.println(cmd.seconds);
}

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
    Serial.println("moveAtSpeed");
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
void motionLoop()
{

    MotionCmd cmd;

    if (index > 0 && !moving)
    {
        popCommand(cmd);
        moveAtSpeed(cmd.speed, cmd.seconds);
    }

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

    if (moving)
    {
        if (secondsRequired < (millis() - initialMillis) / 1000)
        {

            brake();
            stopMotors();
            moving = false;
            targetSpeed = 0.0f;
            motorPower = 0.0f;
            integral = 0.0f;
            initialMillis = 0;
            secondsRequired = 0;

            // Start next queued command if available

            return;
        }
    }

    if ((now - sinceLastPid) > 100) // Updates PID every 100 MS
    {
        float error = targetSpeed - getSpeed();
        if (abs(error) > 1.5f) // error deadzone to prevent jitter
        {
            integral = constrain(integral + error * dt, -30.0f, 30.0f);
            derivative = (error - previous_error) / dt;
            derivative = constrain(derivative, -150.0f, 150.0f);
            motorPower = constrain(MOTION_KP * error + MOTION_KI * integral + MOTION_KD * derivative, -60.0f, 60.0f);
            previous_error = error;
        }
        editMotorsSpeed((int)motorPower, (int)motorPower);
        sinceLastPid = now;
    }
}
