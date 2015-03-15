#include <CapacitiveSensor.h>
#include <Math.h>
#include "Adafruit_NeoPixel.h"

CapacitiveSensor   cs_9_10 = CapacitiveSensor(9,6);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_9_2  = CapacitiveSensor(9,10);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil

Adafruit_NeoPixel front = Adafruit_NeoPixel(24, 1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel back = Adafruit_NeoPixel(40, 12, NEO_GRB + NEO_KHZ800);

uint8_t leftSignal = 0;
uint8_t rightSignal = 0;

uint8_t SignalLED = 0; 
long leftSignalDelay = 0;
long rightSignalDelay = 0;

uint8_t leftPadStatus = 0;
uint8_t rightPadStatus = 0;
uint8_t bothPadStatus = 0;
long padHoldTime = 0;
uint8_t bothPadsHit = 0;
uint8_t rightPadHit = 0;
uint8_t leftPadHit = 0;

void resetSignals(){
  for (int i = 0; i < 16; i++){
    back.setPixelColor(i, front.Color(0,0,0)); 
  }
  for (int i = 0; i < 24; i++){
    front.setPixelColor(i, front.Color(255,255,255)); 
  }
  back.show();
  front.show();
}

void checkPads() {
  long leftTotal  =  cs_9_10.capacitiveSensor(30);
  long rightTotal =  cs_9_2.capacitiveSensor(30);
  if ((leftTotal > 30 && rightTotal > 30) || bothPadsHit) {
    if (!bothPadsHit) {
      padHoldTime = millis();
      bothPadsHit = 1;
    }
    if (leftTotal < 30 || rightTotal < 30) {
      bothPadStatus = ceil(((double)(millis() - padHoldTime))/1000.0);
      bothPadsHit = 0;
      leftPadHit = 0;
      rightPadHit = 0;
      // Serial.print("Both: ");
      // Serial.println(bothPadStatus);
    }
    else {
      bothPadStatus = 0;
    }
  }
  else {
    if (leftTotal > 30 || leftPadHit) {
      if (!leftPadHit) {
        padHoldTime = millis();
        leftPadHit = 1;
      }
      if (leftTotal < 30) {
        leftPadStatus = ceil(((double)(millis() - padHoldTime))/1000.0);
        leftPadHit = 0;
        // Serial.print("Left: ");
        // Serial.println(leftPadStatus);
      }
    }
    else {
      leftPadStatus = 0;
    }
    if (rightTotal > 30 || rightPadHit) {
      if (!rightPadHit) {
        padHoldTime = millis();
        rightPadHit = 1;
      }
      if (rightTotal < 30) {
        rightPadStatus = ceil(((double)(millis() - padHoldTime))/1000.0);
        rightPadHit = 0;
        // Serial.print("Right: ");
        // Serial.println(rightPadStatus);
      }
    }
    else {
      rightPadStatus = 0;
    }
  }
}

void updateState()
{
    // left signal control
    if (leftPadStatus == 1){
      if (leftSignal == 1){
        resetSignals();
        back.show();
        leftSignal = 0;
        SignalLED = 0;
      }
      else {
        resetSignals();
        leftSignal = 1;
        rightSignal = 0;
      }
      front.show();
    }
    // right signal control
    if (rightPadStatus == 1){
      if (rightSignal == 1){
        resetSignals();
        rightSignal = 0;
        SignalLED = 0;
      }
      else {
        resetSignals();
        rightSignal = 1;
        leftSignal = 0;
      }
      front.show();
    }
}

void updateLedAnimations() {
  // left signal animations
  if (leftSignal && millis() - leftSignalDelay > 50 && !rightSignal) {
    if (SignalLED == 0) {
      for (int i = 0; i < 12; i++){
        front.setPixelColor(i, front.Color(255,90,0)); 
      }  
    }
    if (SignalLED == 4) {
      for (int i = 0; i < 12; i++){
        front.setPixelColor(i, front.Color(0,0,0)); 
      }  
    }
    front.show();
    if (SignalLED == 8) {
      SignalLED = 0;
      for (uint8_t i = 0; i < 8; i++) {
        back.setPixelColor(i, back.Color(0,0,0));
      }
    }
    else {
      back.setPixelColor(7 - SignalLED, back.Color(255,90,0));
      SignalLED++;
    }
    back.show();
    leftSignalDelay = millis();
  }
  // right signal animations
  if (rightSignal && millis() - rightSignalDelay > 50 && !leftSignal) {
    if (SignalLED == 0) {
      for (int i = 12; i < 24; i++){
        front.setPixelColor(i, front.Color(255,90,0)); 
      }  
    }
    if (SignalLED == 4) {
      for (int i = 12; i < 24; i++){
        front.setPixelColor(i, front.Color(0,0,0)); 
      }  
    }
    front.show();
    if (SignalLED == 8) {
      SignalLED = 0;
      for (uint8_t i = 8; i < 16; i++) {
        back.setPixelColor(i, back.Color(0,0,0));
      }
    }
    else {
      back.setPixelColor(SignalLED + 8, back.Color(255,90,0));
      SignalLED++;
    }
    back.show();
    rightSignalDelay = millis();
  }
}

void setup()                    
{
  //cs_9_10.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  //cs_9_2.set_CS_AutocaL_Millis(0xFFFFFFFF); 
  // Serial.begin(9600);
  front.begin();
  back.begin();
  front.show();
  uint8_t ready = 0;
  for (int i = 0; i < 40; i++){
    back.setPixelColor(i, front.Color(0,0,0)); 
  }
  for (int i = 0; i < 24; ++i)
  {
    front.setPixelColor(i, front.Color(0,0,0)); 
  }
  while(!ready) {
    checkPads();
    if (bothPadStatus >= 3) {
      ready = 1;
    }
  }
  for (int i = 16; i < 40; i++){
    back.setPixelColor(i, front.Color(100,0,0)); 
  }
  back.show();
  resetSignals();
}
int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
void loop()                    
{
  Serial.println(millis());
  checkPads();
  updateState();
  updateLedAnimations();
  delay(10);                             // arbitrary delay to limit data to serial port 
}
