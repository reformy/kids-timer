#include "SevSeg.h"

SevSeg sevseg;

const byte STATE_IDLE = 0;
const byte STATE_SET = 1;
const byte STATE_GO = 2;
const byte STATE_PAUSE = 3;
const byte STATE_TIMEOUT = 4;


byte state = STATE_IDLE;

const int PIN_UP = 3;
const int PIN_DOWN = 2;
const int PIN_SET = 13;

const long MIN_IN_MS = 60000;
const long MAX_TIME_MS = 100 * MIN_IN_MS - 1;

const long ACTION_TIME_1_MS = 100;
const long ACTION_TIME_2_MS = 2000;

void setup() {
  Serial.begin(9600);
  
  byte numDigits = 2;
  byte digitPins[] = {8, 5};
  byte segmentPins[] = {7, 9, 10, 12, 11, 6, 4, 0};
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_SET, INPUT_PULLUP);
}

// The button currently pressed.
int pressed = 0;
long pressedStartTime = 0;
bool action1fired = false;
bool action2fired = false;

long pressedTime = 0;

byte dispMins = -1;

long timeout = 0;
long timerLeftMs = 0;

int getFireAction1()
{
  if (pressed > 0 && !action1fired && pressedTime > ACTION_TIME_1_MS)
  {
    action1fired = true;
    return pressed;
  }
  return 0;
}

int getFireAction2()
{
  if (pressed > 0 && !action2fired && pressedTime > ACTION_TIME_2_MS)
  {
    action2fired = true;
    return pressed;
  }
  return 0;
}

int fireAction1;
int fireAction2;
long now;

void handleState()
{
  pressedTime = millis() - pressedStartTime;
  fireAction1 = getFireAction1();
  fireAction2 = getFireAction2();
  
  now = millis();
  
  switch (state) {
    
    case STATE_IDLE:
      // Reseted.
      if (fireAction1 == PIN_UP)
      {
        timerLeftMs = MIN_IN_MS;
        dispMins = 1;
        state = STATE_SET;
      }
      break;
      
    case STATE_SET:
      // Set
      if (fireAction1 == PIN_UP)
      {
        if (timerLeftMs + MIN_IN_MS > MAX_TIME_MS)
        {
          // Ignore.
        }
        else
        {
          timerLeftMs += MIN_IN_MS;
          dispMins++;
        }
      }
      else if (fireAction1 == PIN_DOWN)
      {
        timerLeftMs -= MIN_IN_MS;
        dispMins--;
        if (timerLeftMs < 0)
        {
          state = STATE_IDLE;
          timerLeftMs = 0;
          dispMins = -1;
        }
      }
      else if (fireAction1 == PIN_SET)
      {
        state = STATE_GO;
        timeout = now + timerLeftMs;
      }
      break;

    case STATE_GO:
      // Go!

      if (fireAction1 == PIN_SET)
      {
        // Pause.
        state = STATE_PAUSE;
        timerLeftMs = timeout - now;
      }
      else if (timeout < now)
      {
        // Timeout!
        state = STATE_TIMEOUT;
        dispMins = 0;
        timerLeftMs = 0;
        timeout = 0;
      }
      else
      {
        dispMins = (timeout - now) / MIN_IN_MS;
        if (dispMins == 0)
        {
          // Show seconds.
          dispMins = (timeout - now) / 1000;
        }
      }
      break;

    case STATE_PAUSE:
      if (fireAction1 == PIN_UP)
      {
        if (timerLeftMs + MIN_IN_MS > MAX_TIME_MS)
        {
          // Ignore.
        }
        else
        {
          timerLeftMs += MIN_IN_MS;
          dispMins++;
        }
      }
      else if (fireAction1 == PIN_DOWN)
      {
        timerLeftMs -= MIN_IN_MS;
        dispMins--;
        if (timerLeftMs < 0)
        {
          state = STATE_IDLE;
          timerLeftMs = 0;
          dispMins = -1;
          sevseg.setBrightness(100);
        }
      }
      else if (fireAction1 == PIN_SET)
      {
        state = STATE_GO;
        timeout = now + timerLeftMs;
        sevseg.setBrightness(100);
      }
      else
      {
        byte b = (now / 10) % 200;
        if (b > 100)
        {
          b = 200-b;
        }
        sevseg.setBrightness(b);
      }
      break;

    case STATE_TIMEOUT:
      // Timeout.
      dispMins = ((now % 1000) > 500) ? 0 : -1;
      break;
  }

  sevseg.setNumber(dispMins, 0);
}

void loop() {
  if (digitalRead(PIN_UP) == LOW && pressed == 0)
  {
    pressed = PIN_UP;
    pressedStartTime = millis();
  }
  else if (digitalRead(PIN_DOWN) == LOW && pressed == 0)
  {
    pressed = PIN_DOWN;
    pressedStartTime = millis();
  }
  else if (digitalRead(PIN_SET) == LOW && pressed == 0)
  {
    pressed = PIN_SET;
    pressedStartTime = millis();
  }
  else if (pressed != 0 && digitalRead(PIN_UP) == HIGH && digitalRead(PIN_DOWN) == HIGH && digitalRead(PIN_SET) == HIGH)
  {
    pressed = 0;
    pressedStartTime = 0;
    action1fired = false;
  }

  handleState();
  
  sevseg.refreshDisplay();
}

