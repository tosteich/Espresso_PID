#define FLOW_TIMER_INTERVAL 1000
#define FLOW_BUTTONCHECK_INTERVAL 1000
#define AC_DELAY 50

volatile int flowTimerCounter;

hw_timer_t * flowTimer = NULL;
portMUX_TYPE flowTimerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * buttonCheckTimer = NULL;
portMUX_TYPE buttonCheckTimerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onFlowTimer() {
  portENTER_CRITICAL_ISR(&flowTimerMux);
  flowTimerCounter++;
  portEXIT_CRITICAL_ISR(&flowTimerMux);
}

void IRAM_ATTR onbuttonCheckTimer() {
  portENTER_CRITICAL_ISR(&buttonCheckTimerMux);
  if (!digitalRead (BUTTON_PIN)) {
    restartFlowCounterTimer();
    flowButtonIsPressed = true;
  } else if ( flowTimerCounter > AC_DELAY ) {
    stopFlowCounterTimer();
    flowButtonIsPressed = false;
  }
  portEXIT_CRITICAL_ISR(&buttonCheckTimerMux);
}

void initFlowCounterTimer () {
  flowTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(flowTimer, &onFlowTimer, true);
  timerAlarmWrite(flowTimer, FLOW_TIMER_INTERVAL, true);
}

void initButtonCheckTimer () {
  buttonCheckTimer = timerBegin(2, 80, true);
  timerAttachInterrupt(buttonCheckTimer, &onbuttonCheckTimer, true);
  timerAlarmWrite(buttonCheckTimer, FLOW_BUTTONCHECK_INTERVAL, true);
  timerAlarmEnable(buttonCheckTimer);
}

void startFlowCounterTimer () {
  timerAlarmWrite(flowTimer, FLOW_TIMER_INTERVAL, true);
  timerAlarmEnable(flowTimer);
}

void stopFlowCounterTimer () {
  timerAlarmDisable(flowTimer);
  flowTimerCounter = 0;
}

void restartFlowCounterTimer () {
  stopFlowCounterTimer ();
  startFlowCounterTimer();
}

void checkMachineButtonStatus () {
  if (flowButtonIsPressed) {
    if (!stopWatch.getIsFlow()) {
      stopWatch.setIsFlow(true);
      stopWatch.setFlowJustPressed(true);
      stopWatch.run();
    }
  } else {
    if (stopWatch.getIsFlow()) {
      stopWatch.setIsFlow(false);
      stopWatch.setFlowJustPressed(true);
      stopWatch.run();
    }
  }
}
