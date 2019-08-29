#ifndef SENSOR_GRABBER_H
#define SENSOR_GRABBER_H

#include "Arduino.h"

#define DELAY_TO_EASE_BOOT 5000
#define NUMBER_OF_FLOAT_VALUES 2

enum sensorGrabberSettings {
  SEND_WITH_EACH_SAMPLE_ASAP = 1,
  SEND_WITH_EACH_SAMPLE_ON_DEMAND = 2, //CAN BE USED AS INTERNAL TRIGGER!!
  SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS = 3,
  SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_10_SAMPLE_AVERAGE = 4,   //DOESNT WORK
  SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_100_SAMPLE_AVERAGE = 5,  //DOESNT WORK
  PAUSE_THE_SENSOR = 6,
  STOP_THE_SENSOR = 7,
};

class SensorGrabber
{

public:

  SensorGrabber(int16_t input){
    activeSetting = input;
  }

  void setCallbackSetup(void (*function)(void)){
    setCallbackSetupStorage = function;
    SensorSetup();
  }

  void setCallbackLoop(void (*function)(bool)){
    setCallbackLoopStorage = function;
  }

  void setCallbackOutput(void (*function)(float[])){
    setCallbackOutputStorage = function;
  }

  void changeSettings(uint8_t newSettings){
    activeSetting = newSettings;
    SensorSetup();
  }

  void setLoopSpeed(uint16_t millisInput){
    loopRunSpeed = millisInput;
  }

  void grabData(float input, uint8_t whichArrayPossition){
    if(activeSetting == SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_10_SAMPLE_AVERAGE
    || activeSetting == SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_100_SAMPLE_AVERAGE){
      howManyTimesWeStoredTheValue++;
      storedValue[whichArrayPossition - 1] += input;
    }
    else{
    storedValue[whichArrayPossition - 1] = input;
    }
  }

  void SensorGrabberDemandData() {
    demandData = true;
  }

  void setIntervalSensorRead(uint32_t input) {
    interval = input;
  }

  void SensorSetup(){

    uint32_t currentTime = millis();

      if(activeSetting == 0){
        #ifdef DEBUG
        Serial.println("no setting has been set for sensor Grabber, this instance will be terminated");
        #endif
        return;
      }

      if(!setupDone){
        setCallbackSetupStorage();
        setupDone = true;
        triggerTime = currentTime + loopRunSpeed + DELAY_TO_EASE_BOOT;
        //triggerTime = currentTime + loopRunSpeed;
      }
      else{
        triggerTime = currentTime + loopRunSpeed;
        triggerTimeRead = 0;
        howManyTimesWeStoredTheValue = 0;
        for(int x = 0; x < NUMBER_OF_FLOAT_VALUES; x++){
          storedValue[NUMBER_OF_FLOAT_VALUES] = 0;
        }
        demandData = false;
      }
  }

  void SensorGrabberloop(){

    uint32_t currentTime = millis();
    if(triggerTime <= currentTime) {
      triggerTime = (currentTime + loopRunSpeed);

      switch (activeSetting) {
        case SEND_WITH_EACH_SAMPLE_ASAP:
          setCallbackLoopStorage(true);
          setCallbackOutputStorage(storedValue);
        break;

        case SEND_WITH_EACH_SAMPLE_ON_DEMAND:
          if(demandData){
            setCallbackLoopStorage(true);
            setCallbackOutputStorage(storedValue);
            demandData = false;
          }
          else{
            setCallbackLoopStorage(false);
          }
        break;

        case SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS:
          if(triggerTimeRead <= currentTime){
            triggerTimeRead = currentTime + interval;
            setCallbackLoopStorage(true);
            setCallbackOutputStorage(storedValue);
          }
          else{
            setCallbackLoopStorage(false);
          }
        break;

        case SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_10_SAMPLE_AVERAGE:
          if(triggerTimeRead <= currentTime){
            triggerTimeRead = currentTime + interval;
            setCallbackLoopStorage(true);
            if(howManyTimesWeStoredTheValue == 10){
              for(int x = 0; x < NUMBER_OF_FLOAT_VALUES; x++){
                storedValue[NUMBER_OF_FLOAT_VALUES] = storedValue[NUMBER_OF_FLOAT_VALUES]/10;
              }

              setCallbackOutputStorage(storedValue);
              howManyTimesWeStoredTheValue = 0;

            }
          }
          else{
            setCallbackLoopStorage(false);
          }
        break;

        case SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_100_SAMPLE_AVERAGE:
          if(triggerTimeRead <= currentTime){
            triggerTimeRead = currentTime + interval;
            setCallbackLoopStorage(true);
            if(howManyTimesWeStoredTheValue == 100){
              for(int x = 0; x < NUMBER_OF_FLOAT_VALUES; x++){
                storedValue[NUMBER_OF_FLOAT_VALUES] = storedValue[NUMBER_OF_FLOAT_VALUES]/100;
              }

              setCallbackOutputStorage(storedValue);
              howManyTimesWeStoredTheValue = 0;

            }
          }
          else{
            setCallbackLoopStorage(false);
          }
        break;

        case PAUSE_THE_SENSOR:
          setCallbackLoopStorage(false);
        break;

        case STOP_THE_SENSOR:
        break;
      }
    }
  }

  private:

  void (*setCallbackSetupStorage)(void);
  void (*setCallbackLoopStorage)(bool);
  void (*setCallbackOutputStorage)(float[]);

  uint16_t loopRunSpeed = 0;
  uint32_t triggerTime = 0;
  uint32_t triggerTimeRead = 0;
  uint32_t interval = 0;
  uint16_t activeSetting = 0;
  bool setupDone = false;
  uint8_t howManyTimesWeStoredTheValue = 0;



  float storedValue[NUMBER_OF_FLOAT_VALUES] = {0, 0};
  bool demandData = false;

};

#endif
