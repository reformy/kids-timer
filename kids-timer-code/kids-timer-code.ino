#include "SevSeg.h"

SevSeg sevseg;

void setup() {
  byte numDigits = 2;
  byte digitPins[] = {8, 7};
  byte segmentPins[] = {5, 9, 10, 12, 11, 6, 4, 14};
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
}

void loop() {
  sevseg.setNumber(millis()/1000,0);
  sevseg.refreshDisplay();
}

