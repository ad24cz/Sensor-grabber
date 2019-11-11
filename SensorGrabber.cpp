#include "SensorGrabber.h"
#include <Arduino.h>

SensorGrabber::SensorGrabber(SensorGrabberSettings input) {
    activeSetting = input;
}

void SensorGrabber::setCallbackSetup(void (*function)(void)) {
    callbackSetup = function;
    sensorSetup();
}

void SensorGrabber::setCallbackLoop(void (*function)(bool)) {
    callbackLoop = function;
}

void SensorGrabber::setCallbackOutput(void (*function)(float[])) {
    callbackOutput = function;
}

void SensorGrabber::changeSettings(SensorGrabberSettings newSettings) {
    activeSetting = newSettings;
    sensorSetup();
}

void SensorGrabber::setLoopSpeed(uint16_t millisInput) {
    loopRunSpeed = millisInput;
}

void SensorGrabber::grabData(float input, uint8_t whichArrayPosition) {
    if (activeSetting == SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_10_SAMPLE_AVERAGE
     || activeSetting == SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_100_SAMPLE_AVERAGE) {
        howManyTimesWeStoredTheValue++;
        storedValue[whichArrayPosition - 1] += input;
    } else {
        storedValue[whichArrayPosition - 1] = input;
    }
}

void SensorGrabber::demandData() {
    dataDemanded = true;
}

void SensorGrabber::setIntervalSensorRead(uint32_t input) {
    interval = input;
}

void SensorGrabber::sensorSetup() {
    uint32_t currentTime = millis();

    if (activeSetting == 0) {
#ifdef DEBUG
        Serial.println("no setting has been set for sensor Grabber, this instance will be terminated");
#endif
        return;
    }

    if (!setupDone) {
        callbackSetup();
        setupDone = true;
        triggerTime = currentTime + loopRunSpeed + DELAY_TO_EASE_BOOT;
        //triggerTime = currentTime + loopRunSpeed;
    } else {
        triggerTime = currentTime + loopRunSpeed;
        triggerTimeRead = 0;
        howManyTimesWeStoredTheValue = 0;
        for (int i = 0; i < NUMBER_OF_FLOAT_VALUES; i++) {
            storedValue[NUMBER_OF_FLOAT_VALUES] = 0;
        }
        dataDemanded = false;
    }
}

void SensorGrabber::loop() {
    uint32_t currentTime = millis();
    if (triggerTime <= currentTime) {
        triggerTime = (currentTime + loopRunSpeed);

        switch (activeSetting) {
            case SEND_WITH_EACH_SAMPLE_ASAP:
                callbackLoop(true);
                callbackOutput(storedValue);
                break;

            case SEND_WITH_EACH_SAMPLE_ON_DEMAND:
                if (dataDemanded) {
                    callbackLoop(true);
                    callbackOutput(storedValue);
                    dataDemanded = false;
                } else {
                    callbackLoop(false);
                }
                break;

            case SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS:
                if (triggerTimeRead <= currentTime) {
                    triggerTimeRead = currentTime + interval;
                    callbackLoop(true);
                    callbackOutput(storedValue);
                } else {
                    callbackLoop(false);
                }
                break;

            case SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_10_SAMPLE_AVERAGE:
                if (triggerTimeRead <= currentTime) {
                    triggerTimeRead = currentTime + interval;
                    callbackLoop(true);
                    if (howManyTimesWeStoredTheValue == 10) {
                        for (int i = 0; i < NUMBER_OF_FLOAT_VALUES; i++) {
                            storedValue[NUMBER_OF_FLOAT_VALUES] = storedValue[NUMBER_OF_FLOAT_VALUES] / 10;
                        }

                        callbackOutput(storedValue);
                        howManyTimesWeStoredTheValue = 0;
                    }
                } else {
                    callbackLoop(false);
                }
                break;

            case SEND_WITH_EACH_SAMPLE_INTERVAL_MILLIS_WITH_100_SAMPLE_AVERAGE:
                if (triggerTimeRead <= currentTime) {
                    triggerTimeRead = currentTime + interval;
                    callbackLoop(true);
                    if (howManyTimesWeStoredTheValue == 100) {
                        for (int i = 0; i < NUMBER_OF_FLOAT_VALUES; i++) {
                            storedValue[NUMBER_OF_FLOAT_VALUES] = storedValue[NUMBER_OF_FLOAT_VALUES] / 100;
                        }

                        callbackOutput(storedValue);
                        howManyTimesWeStoredTheValue = 0;
                    }
                } else {
                    callbackLoop(false);
                }
                break;

            case PAUSE_THE_SENSOR:
                callbackLoop(false);
                break;

            case STOP_THE_SENSOR:
                break;
        }
    }
}