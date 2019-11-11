#ifndef SENSOR_GRABBER_H
#define SENSOR_GRABBER_H

#include "Arduino.h"

#define DELAY_TO_EASE_BOOT 5000
#define NUMBER_OF_FLOAT_VALUES 2

enum SensorGrabberSettings {
    SEND_WITH_EACH_SAMPLE_ASAP = 1,
    SEND_WITH_EACH_SAMPLE_ON_DEMAND = 2,  //CAN BE USED AS INTERNAL TRIGGER!!
    SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS = 3,
    SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_10_SAMPLE_AVERAGE = 4,   //DOESNT WORK
    SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_100_SAMPLE_AVERAGE = 5,  //DOESNT WORK
    PAUSE_THE_SENSOR = 6,
    STOP_THE_SENSOR = 7,
};

class SensorGrabber {
   public:
    SensorGrabber(SensorGrabberSettings input);

    void setCallbackSetup(void (*function)(void));

    void setCallbackLoop(void (*function)(bool));

    void setCallbackOutput(void (*function)(float[]));

    void changeSettings(SensorGrabberSettings newSettings);

    void setLoopSpeed(uint16_t millisInput);

    void grabData(float input, uint8_t whichArrayPosition);

    void demandData();

    void setIntervalSensorRead(uint32_t input);

    void sensorSetup();

    void loop();

   private:
    void (*callbackSetup)(void);
    void (*callbackLoop)(bool);
    void (*callbackOutput)(float[]);

    uint16_t loopRunSpeed = 0;
    uint32_t triggerTime = 0;
    uint32_t triggerTimeRead = 0;
    uint32_t interval = 0;
    uint16_t activeSetting = 0;
    bool setupDone = false;
    uint8_t howManyTimesWeStoredTheValue = 0;

    float storedValue[NUMBER_OF_FLOAT_VALUES] = {0, 0};
    bool dataDemanded = false;
};

#endif
