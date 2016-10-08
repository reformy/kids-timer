#include "SevSeg.h"

SevSeg sevseg;

byte state = 0; // 0 reseted, 1 set, 2 going, 3 ding.

int PIN_UP = 3;
int PIN_DOWN = 2;
int PIN_SET = 13;

void setup() {
  Serial.begin(9600);
  
  byte numDigits = 2;
  byte digitPins[] = {8, 7};
  byte segmentPins[] = {5, 9, 10, 12, 11, 6, 4, 14};
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

long ACTION_TIME_1_MS = 100;
long ACTION_TIME_2_MS = 2000;

byte dispMins = 0;

int getFireAction1()
{
  Serial.println("getFireAction1");
  Serial.println(pressed);
  Serial.println(action1fired);
  Serial.println(pressedTime);
  Serial.println();
  if (pressed > 0 && !action1fired && pressedTime > ACTION_TIME_1_MS)
  {
    Serial.println("Pressed 1: ");
    Serial.println(pressed);
    action1fired = true;
    return pressed;
  }
  return 0;
}

int getFireAction2()
{
  if (pressed > 0 && !action2fired && pressedTime > ACTION_TIME_2_MS)
  {
    Serial.println("Pressed 2: ");
    Serial.println(pressed);
    action2fired = true;
    return pressed;
  }
  return 0;
}

void handleState()
{
  pressedTime = millis() - pressedStartTime;
  //Serial.println(pressedTime);
  int fireAction1 = getFireAction1();
  int fireAction2 = getFireAction2();
  
  switch (state) {
    
    case 0:
      // Reseted.
      if (fireAction1 == PIN_UP)
      {
        dispMins = 1;
        state = 1;
      }
      break;
      
    case 1:
      // Set
      if (fireAction1 == PIN_UP)
      {
        dispMins++;
      }
      else if (fireAction1 == PIN_DOWN)
      {
        dispMins--;
        if (dispMins == 0)
        {
          state = 0;
          dispMins = -1;
        }
      }
      break;
  }

  sevseg.setNumber(dispMins, 0);
}

void loop() {
  if (digitalRead(PIN_UP) == LOW && pressed == 0)
  {
    Serial.println("PIN_UP low");
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
  else if (pressed != 0)
  {
    pressed = 0;
    pressedStartTime = 0;
    action1fired = false;
  }

  handleState();
  
  sevseg.refreshDisplay();

  delay(100);
}

