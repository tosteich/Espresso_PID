void setup(void) {

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LMT_PIN, INPUT);

  Serial.begin(115200);
  preferences.begin("gaggia-prefs", false);
  getPreferenses();
  sensorsTimer.setPeriod(SENSORS_PERIOD);
  heater.setPeriod(MAX_BOILER_PW);
  regulator.setupPid( preferences.getFloat("pidKp", 145.0),
                      preferences.getFloat("pidKi", 500.0),
                      preferences.getFloat("pidKd", 10.),
                      TARGET_TEMP, SENSORS_PERIOD, MAX_BOILER_PW);
  preferences.end();

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);

  drawInitScreen();
  
  Timer wifiTimer(500);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.setTimeOffset(7200);
  updateCurrDate();

  initWebSocket();
  initWebServerControllers();
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();

  initPulseCounter ();
  initPulseCounterTimer ();
  initFlowCounterTimer ();
  initButtonCheckTimer();
}

void getPreferenses () {
  TARGET_TEMP = preferences.getInt("targetTemp", 24);
  SENSORS_PERIOD = preferences.getInt("sensorsPeriod", 200);
  MAX_BOILER_PW = preferences.getInt("boilerMaxPower", 1000);
}
