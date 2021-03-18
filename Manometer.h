#define PRESSURE_PIN 34


class Manometer {
  private:
    float analogLowVolts = 0.33; //analog readings with 0 bar and 0,33v
    float analogRangeVolts = 2.64; //analog readings minus analogLow with 12 bar and 2.97v
    float maxPressure = 12.0;
    int currentSensorValue;

    float calcInputVolts () {
      if (currentSensorValue > 3000) {
        return 0.0005 * (float)currentSensorValue + 1.0874;
      } else {
        return 0.0008 * (float)currentSensorValue + 0.1372;
      }
    }

  public:
    void update() {
      int sensorVal = 0;
      for ( int i = 0; i < 32; i++ ) {
        sensorVal = sensorVal + analogRead(PRESSURE_PIN);
      }
      currentSensorValue = sensorVal / 32;
    }

    float getPressure () {
      this->update();
      float result = (calcInputVolts() - analogLowVolts) * maxPressure / analogRangeVolts;
      return result > 0 ? result : 0.0;
    }
};
