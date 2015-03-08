#include <CapacitiveSensor.h>
#include <Math.h>
#include "Adafruit_NeoPixel.h"

CapacitiveSensor   cs_9_10 = CapacitiveSensor(9,6);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_9_2  = CapacitiveSensor(9,10);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil

Adafruit_NeoPixel front = Adafruit_NeoPixel(24, 1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel back = Adafruit_NeoPixel(40, 12, NEO_GRB + NEO_KHZ800);

// signal state trackers
int leftSignal = 0;
int rightSignal = 0;
int lastLeftSignal = 0;
int lastRightSignal = 0;

// touch pad trackers
long lastLeftState = 0;
long lastRightState = 0;

uint8_t leftSignalLED = 0; 
long leftSignalDelay = 0;

uint8_t rightSignalLED = 0; 
long rightSignalDelay = 0;

uint8_t leftPadStatus = 0;
uint8_t rightPadStatus = 0;
uint8_t bothPadStatus = 0;
long padHoldTime = 0;
uint8_t bothPadsHit = 0;
uint8_t rightPadHit = 0;
uint8_t leftPadHit = 0;

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
      Serial.print("Both: ");
      Serial.println(bothPadStatus);
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
        Serial.print("Left: ");
        Serial.println(leftPadStatus);
      }
    }
    if (rightTotal > 30 || rightPadHit) {
      if (!rightPadHit) {
        padHoldTime = millis();
        rightPadHit = 1;
      }
      if (rightTotal < 30) {
        rightPadStatus = ceil(((double)(millis() - padHoldTime))/1000.0);
        rightPadHit = 0;
        Serial.print("Right: ");
        Serial.println(rightPadStatus);
      }
    }
    // left signal control
    // if (leftTotal > 30 && lastLeftState < 30){
    //   if (leftSignal == 1){
    //     for (int i = 0; i < 12; i++){
    //       front.setPixelColor(i, front.Color(255,255,255)); 
    //     }
    //     for (int i = 0; i < 8; i++){
    //       back.setPixelColor(i, front.Color(0,0,0)); 
    //     }
    //     back.show();
    //     leftSignal = 0;
    //     leftSignalLED = 0;
    //   }
    //   else {
    //     leftSignal = 1;
    //   }
    //   front.show();
    //   Serial.print("left pressed");
    // }
    // // right signal control
    // if (total2 > 30 && lastRightState < 30){
    //   if (rightSignal == 1){
    //     for (int i = 12; i < 24; i++){
    //       front.setPixelColor(i, front.Color(255,255,255)); 
    //     }
    //     rightSignal = 0;
    //   }
    //   else {
    //     for (int i = 12; i < 24; i++){
    //       front.setPixelColor(i, front.Color(255,90,0)); 
    //     }  
    //     rightSignal = 1;
    //   }
    //   front.show();
    //   Serial.print("right pressed");
    // }
  }
}

void setup()                    
{
  cs_9_10.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  cs_9_2.set_CS_AutocaL_Millis(0xFFFFFFFF); 
  Serial.begin(9600);
  front.begin();
  back.begin();
  front.show();
  for (int i = 16; i < 40; i++){
    back.setPixelColor(i, front.Color(100,0,0)); 
  }
  back.show();
}

void loop()                    
{
  checkPads();
  long start = millis();
  
  // left signal animations
  if (leftSignal && millis() - leftSignalDelay > 50 && !rightSignal) {
    if (leftSignalLED == 0) {
      for (int i = 0; i < 12; i++){
        front.setPixelColor(i, front.Color(255,90,0)); 
      }  
    }
    if (leftSignalLED == 4) {
      for (int i = 0; i < 12; i++){
        front.setPixelColor(i, front.Color(0,0,0)); 
      }  
    }
    front.show();
    if (leftSignalLED == 8) {
      leftSignalLED = 0;
      for (uint8_t i = 0; i < 8; i++) {
        back.setPixelColor(i, back.Color(0,0,0));
      }
    }
    else {
      back.setPixelColor(7 - leftSignalLED, back.Color(255,90,0));
      leftSignalLED++;
    }
    back.show();
    leftSignalDelay = millis();
  }
  // right signal animations
  if (rightSignal && millis() - rightSignalDelay > 50 && !leftSignal) {
    if (rightSignalLED == 0) {
      for (int i = 12; i < 24; i++){
        front.setPixelColor(i, front.Color(255,90,0)); 
      }  
    }
    if (rightSignalLED == 4) {
      for (int i = 12; i < 24; i++){
        front.setPixelColor(i, front.Color(0,0,0)); 
      }  
    }
    front.show();
    if (rightSignalLED == 8) {
      rightSignalLED = 0;
      for (uint8_t i = 8; i < 16; i++) {
        back.setPixelColor(i, back.Color(0,0,0));
      }
    }
    else {
      back.setPixelColor(rightSignalLED + 8, back.Color(255,90,0));
      rightSignalLED++;
    }
    back.show();
    rightSignalDelay = millis();
  }
  delay(10);                             // arbitrary delay to limit data to serial port 
}
