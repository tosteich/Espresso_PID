class GaggiaPid {
  private:
    float iState = 0;
    float lastTemp = 0;
    float pTerm, dTerm;
    float Kp, Ki, Kd;
    float setpoint;
    float periodSec;
    int maxPower;

  public:

    GaggiaPid() {
    }

    GaggiaPid(float Kp, float Ki, float Kd, float setpoint) {
      this->Kp = Kp;
      this->Ki = Ki;
      this->Kd = Kd;
      this->setpoint = setpoint;
    }

    void setupPid (float Kp, float Ki, float Kd, float setpoint, int period, int maxPower) {
      this->Kp = Kp;
      this->Ki = Ki;
      this->Kd = Kd;
      this->setpoint = setpoint;
      periodSec = period / 1000.0;
      this->maxPower = maxPower / 10;
    }

    void setKp (float Kp) {
      this->Kp = Kp;
    }

    void setKi (float Ki) {
      this->Ki = Ki;
    }

    void setKd (float Kd) {
      this->Kd = Kd;
    }

    float getKp () {
      return Kp;
    }

    float getKi () {
      return Ki;
    }

    float getKd () {
      return Kd;
    }

    void setSetpoint (float setpoint) {
      this->setpoint = setpoint;
    }

    int getResult(float input)
    {
      // determine how badly we are doing
      float error = setpoint - input;

      pTerm = Kp * error;

      dTerm = Kd * (input - lastTemp) / periodSec;

      lastTemp = input;

      iState += error * Ki * periodSec;

      float result = constrain ((pTerm + dTerm), 0, maxPower);
      
      if (Ki != 0) {
        iState = constrain(iState, (0 - result) / (Ki * periodSec), (maxPower - result) / (Ki * periodSec));
      }
      
      iState = constrain (iState, 0, maxPower);
      result = (int) constrain ((result + iState), 0, maxPower);

      //Serial.println(result);
      return  result;
    }
};
