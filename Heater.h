#define HEAT_RELAY_PIN 16


class Heater {
  private:
    int heatCycles;
    boolean flag = false;
    int maxPower = 100;
    unsigned long activePeriod, tmr;
    int period;

  public:
    Heater() {
      pinMode(HEAT_RELAY_PIN, OUTPUT);
    }

    void setPeriod (int period) {
      this->period = period;
    }

    void setHeatPower(int power) {
      heatCycles = power;
      if (heatCycles == 0) {
        toggleHeatElementOnOff(false);
      }
      if (heatCycles == maxPower) {
        toggleHeatElementOnOff(true);
      }
      activePeriod = heatCycles * period / maxPower;
    }

    void updateHeater() {
      if (heatCycles != 0 && heatCycles != maxPower) {
        if (millis() - tmr >= (flag ? activePeriod : (period - activePeriod))) {
          tmr = millis();
          flag = !flag;
          toggleHeatElementOnOff (flag);
        }
      }
    }

    void toggleHeatElementOnOff(boolean toggle) {
      digitalWrite(HEAT_RELAY_PIN, toggle);  
    }

};
