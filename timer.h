#pragma once
#include <Arduino.h>
class Timer {
  public:
    Timer () {};
    Timer (int period) {
      _period = period;
    }
    bool ready() {
      if (millis() - _tmr >= _period) {
        _tmr = millis();
        return true;
      }
      return false;
    }

    void setPeriod (int period) {
      _period = period;
    }
  private:
    uint32_t _tmr;
    int _period;
};
