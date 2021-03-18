#pragma once
#include <Arduino.h>

class LMT01 {
  private:
    volatile int currentPulses = 0;
  public:
    LMT01() {}

    void setCurrentPulses (int currentPulses) {
      this->currentPulses = currentPulses;
    }

    float getTemperature() {
      return currentPulses / 16.0 - 50;
    }
};
