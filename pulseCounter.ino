#include "driver/pcnt.h"

#define PCNT_UNIT PCNT_UNIT_0
#define LMT_TIMER_INTERVAL 5000

int16_t pulseCounter = 0;
volatile int oldPulseCount;

void initPulseCounter () {
  pcnt_config_t pcntConfig = { };
  pcntConfig.pulse_gpio_num = LMT_PIN;
  pcntConfig.neg_mode = PCNT_COUNT_INC;
  pcntConfig.unit = PCNT_UNIT;
  pcntConfig.channel = PCNT_CHANNEL_0;

  pcnt_unit_config(&pcntConfig);

  pcnt_set_filter_value(PCNT_UNIT, 100);
  pcnt_filter_enable(PCNT_UNIT);

  pcnt_counter_pause(PCNT_UNIT);
  pcnt_counter_clear(PCNT_UNIT);
  pcnt_counter_resume(PCNT_UNIT);
}

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onLmtTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  timeToCheckCounter = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void checkCounter () {
  pcnt_get_counter_value(PCNT_UNIT, &pulseCounter);
  if (pulseCounter > 0) {
    if (oldPulseCount == pulseCounter) {
      thermoLmt.setCurrentPulses(pulseCounter);
      pcnt_counter_clear(PCNT_UNIT);
      oldPulseCount = 0;
    } else {
      oldPulseCount = pulseCounter;
    }
  }
}

void initPulseCounterTimer () {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onLmtTimer, true);
  timerAlarmWrite(timer, LMT_TIMER_INTERVAL, true);
  timerAlarmEnable(timer);
}
