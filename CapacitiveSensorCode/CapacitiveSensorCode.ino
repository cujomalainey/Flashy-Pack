#include <CapacitiveSensor.h>
#include <Math.h>
#include "Adafruit_NeoPixel.h"
#include <Wire.h>
#include "TSL2561.h"
#include <Adafruit_LSM303.h>

#define VISIBLE_UPPER_THRESHOLD 500.0
#define VISIBLE_LOWER_THRESHOLD 100.0

#define CAPACITANCE_ACTIVATION_THRESHOLD 30
#define CAPACITANCE_INTERFERENCE_THRESHOLD 100

class AnimationManager
{
  public:
    AnimationManager()
    {
      this->front = Adafruit_NeoPixel(24, 1, NEO_GRB + NEO_KHZ800);
      this->back = Adafruit_NeoPixel(40, 12, NEO_GRB + NEO_KHZ800);
      this->front.begin();
      this->back.begin();
      for (int i = 0; i < 40; i++){
        back.setPixelColor(i, front.Color(0,0,0)); 
      }
      for (int i = 0; i < 24; ++i)
      {
        front.setPixelColor(i, front.Color(0,0,0)); 
      }
      this->front.show();
      this->back.show();
      this->SignalLED = 0;
      this->leftSignalDelay = 0;
      this->rightSignalDelay = 0;
    };
    float brightness;
    void update()
    {
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
    };
    void resetSignals()
    {
      for (int i = 0; i < 16; i++){
        this->back.setPixelColor(i, this->front.Color(0,0,0)); 
      }
      int white = (int)(255.0*this->brightness);
      for (int i = 0; i < 24; i++){
        this->front.setPixelColor(i, this->front.Color(white,white,white)); 
      }
  
      this->back.show();
      this->front.show();
    };

  private:
    Adafruit_NeoPixel front;
    Adafruit_NeoPixel back;
    uint8_t SignalLED; 
    long leftSignalDelay;
    long rightSignalDelay;
};

class StateManager
{
  public:
    StateManager(AnimationManager* am)
    {
      this->am = am;
    };
    void update()
    {
      if (this->ready != 0)
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
    };
    void flipReady()
    {
      if (this->ready == 0)
      {
        this->am->
      }
      else
      {
        this->am->resetSignals();
      }
    };

  private:
    AnimationManager* am;
    uint8_t ready = 0;
};

class SensorManager
{
  public:
    SensorManager(StateManager* stm, AnimationManager* am)
    {
      this->stm = stm;
      this->am = am;
      //cs_9_10.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
      //cs_9_2.set_CS_AutocaL_Millis(0xFFFFFFFF); 
      Serial.println("Starting LSM");
      if (!lsm.begin())
      {
        Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
        while (1);
      }
      Serial.println("Starting TSL");
      if (tsl.begin()) {
        Serial.println("Found sensor");
      } else {
        Serial.println("No sensor?");
        while (1);
      }
        
      // You can change the gain on the fly, to adapt to brighter/dimmer light situations
      //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
      this->tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
      
      // Changing the integration time gives you a longer time over which to sense light
      // longer timelines are slower, but are good in very low light situtations!
      this->tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
      //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
      //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
      
      // Now we're ready to get readings!
    };
    void update()
    {
      uint16_t vis = tsl.getLuminosity(TSL2561_VISIBLE);
      Serial.print("Visible: "); Serial.println(vis, DEC);
      float factor = ((float)vis - VISIBLE_LOWER_THRESHOLD)/(VISIBLE_UPPER_THRESHOLD - VISIBLE_LOWER_THRESHOLD);
      factor = min(1, max(0, factor)) - brightness;

      if (factor > 0.01)
      {
        brightness = brightness + 0.01;
      }
      if (factor < -0.01)
      {
        brightness = brightness - 0.01;
      }
      int red = (int)(100.0*brightness);
      for (int i = 16; i < 40; i++){
        back.setPixelColor(i, front.Color(red,0,0)); 
      }
      back.show();
      Serial.print("Brightness: "); Serial.println(brightness);
      lsm.read();
      Serial.print("Accel X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
      Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
      Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
      Serial.print("Mag X: "); Serial.print((int)lsm.magData.x);     Serial.print(" ");
      Serial.print("Y: "); Serial.print((int)lsm.magData.y);         Serial.print(" ");
      Serial.print("Z: "); Serial.println((int)lsm.magData.z);       Serial.print(" ");
      delay(100);
    };

  private:
    StateManager* stm;
    AnimationManager* am;
    Adafruit_LSM303 lsm;
    TSL2561 tsl(TSL2561_ADDR_FLOAT);
    CapacitiveSensor cs_9_10 = CapacitiveSensor(9,6);
    CapacitiveSensor cs_9_2 = CapacitiveSensor(9,10);
    uint8_t leftSignal = 0;
    uint8_t rightSignal = 0; 
};

class TouchManager
{
  public:
    TouchManager(StateManager* stm)
    {
      this->stm = stm;
    };
    void update()
    {
      long leftTotal  =  cs_9_10.capacitiveSensor(30);
      long rightTotal =  cs_9_2.capacitiveSensor(30);
      if (leftTotal > CAPACITANCE_INTERFERENCE_THRESHOLD)
      {
        Serial.print("Left: ");
        Serial.println(leftTotal);
        leftTotal = 2;
      }
      if (rightTotal > CAPACITANCE_INTERFERENCE_THRESHOLD)
      {
        Serial.print("Right: ");
        Serial.println(rightTotal);
        rightTotal = 2;
      }
      if ((leftTotal > CAPACITANCE_ACTIVATION_THRESHOLD && rightTotal > CAPACITANCE_ACTIVATION_THRESHOLD) || bothPadsHit) {
        if (!bothPadsHit) {
          padHoldTime = millis();
          bothPadsHit = 1;
        }
        if (leftTotal < CAPACITANCE_ACTIVATION_THRESHOLD || rightTotal < CAPACITANCE_ACTIVATION_THRESHOLD) {
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
        if (leftTotal > CAPACITANCE_ACTIVATION_THRESHOLD || leftPadHit) {
          if (!leftPadHit) {
            padHoldTime = millis();
            leftPadHit = 1;
          }
          if (leftTotal < CAPACITANCE_ACTIVATION_THRESHOLD) {
            leftPadStatus = ceil(((double)(millis() - padHoldTime))/1000.0);
            leftPadHit = 0;
            // Serial.print("Left: ");
            // Serial.println(leftPadStatus);
          }
        }
        else {
          leftPadStatus = 0;
        }
        if (rightTotal > CAPACITANCE_ACTIVATION_THRESHOLD || rightPadHit) {
          if (!rightPadHit) {
            padHoldTime = millis();
            rightPadHit = 1;
          }
          if (rightTotal < CAPACITANCE_ACTIVATION_THRESHOLD) {
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
    };

  private:
    StateManager* stm;
    uint8_t leftPadStatus = 0;
    uint8_t rightPadStatus = 0;
    uint8_t bothPadStatus = 0;
    long padHoldTime = 0;
    uint8_t bothPadsHit = 0;
    uint8_t rightPadHit = 0;
    uint8_t leftPadHit = 0;
};

AnimationManager anim();
StateManager statem(anim);
SensorManager sensorm(statem, anim);
TouchManager touchm(statem);

void setup()                    
{
  Serial.begin(9600);
}

void loop()                    
{
  anim.update();
  statem.update();
  sensorm.update();
  touchm.update();
  delay(10);                             // arbitrary delay to limit data to serial port 
}
