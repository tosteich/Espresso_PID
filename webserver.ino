String processor(const String& var) {
  if (var == "TEMPERATUREC") {
    return getTemperature();
  }
  if (var == "PRESSURE") {
    return getPressure();
  }
  if (var == "TARGETTEMP") {
    return String(TARGET_TEMP);
  }
  if (var == "PID_KP") {
    return String(regulator.getKp());
  }
  if (var == "PID_KI") {
    return String(regulator.getKi());
  }
  if (var == "PID_KD") {
    return String(regulator.getKd());
  }
  return String();
}

String getTemperature() {
  return String(thermoLmt.getTemperature());
}

String getPressure() {
  if (currentPressure < 0) {
    return String(0.0);
  }
  return String(currentPressure);
}


String changeTargetTemp (byte temp) {
  preferences.begin("gaggia-prefs", false);
  preferences.putInt("targetTemp", temp);
  preferences.end();
  TARGET_TEMP = temp;
  targetTempWasChanged = true;
  sendWsTargetTemp ();
  regulator.setSetpoint (TARGET_TEMP);
}


void sendWsTargetTemp () {
  if (wsClients > 0) {
    char buffer[50];
    sprintf(buffer, "{\"name\":\"targetTemp\",\"value\":\"%u\"}", TARGET_TEMP);
    ws.textAll(buffer);
  }
}

void changePidKp(float newKp) {
  regulator.setKp(newKp);
  preferences.begin("gaggia-prefs", false);
  preferences.putFloat("pidKp", newKp);
  preferences.end();
  if (wsClients > 0) {
    char buffer[50];
    sprintf(buffer, "{\"name\":\"pidKp\",\"value\":\"%f\"}", newKp);
    ws.textAll(buffer);
  }
}

void changePidKi(float newKi) {
  regulator.setKi(newKi);
  preferences.begin("gaggia-prefs", false);
  preferences.putFloat("pidKi", newKi);
  preferences.end();
  if (wsClients > 0) {
    char buffer[50];
    sprintf(buffer, "{\"name\":\"pidKi\",\"value\":\"%f\"}", newKi);
    ws.textAll(buffer);
  }
}

void changePidKd(float newKd) {
  regulator.setKd (newKd);
  preferences.begin("gaggia-prefs", false);
  preferences.putFloat("pidKd", newKd);
  preferences.end();
  if (wsClients > 0) {
    char buffer[50];
    sprintf(buffer, "{\"name\":\"pidKd\",\"value\":\"%f\"}", newKd);
    ws.textAll(buffer);
  }
}

void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "reboot") == 0) {
      ESP.restart();
    }
    if (strcmp((char*)data, "initInfo") == 0) {
      giveInitInfoToClient(client);
    }
  }
}

void giveInitInfoToClient (AsyncWebSocketClient *client) {
  char buffer[200];
  sprintf(buffer, "{\"name\":\"initInfo\",\"currentTemp\":\"%f\",\"currentPress\":\"%f\",\"targetTemp\":\"%u\",\"pidKp\":\"%f\",\"pidKi\":\"%f\",\"pidKd\":\"%f\"}", currentTemperature, currentPressure, TARGET_TEMP, regulator.getKp(), regulator.getKi(), regulator.getKd());
  ws.text(client->id(), buffer);
}

void updateInfoToClients () {
  if (wsClients > 0) {
    char buffer[100];
    sprintf(buffer, "{\"name\":\"updateInfo\",\"currentTemp\":\"%f\",\"currentPress\":\"%f\",\"currentPower\":\"%u\"}", currentTemperature, currentPressure, heatPower);
    ws.textAll(buffer);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      wsClients++;
      Serial.printf("Total connected clients - %u\n", wsClients);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      wsClients--;
      Serial.printf("Total connected clients - %u\n", wsClients);
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(client, arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebServerControllers () {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/targetTemp", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("temp")) {
      String str = request->getParam("temp")->value();
      int newTemp = str.toInt();
      changeTargetTemp(newTemp);
      Serial.printf("New target temp - %u\n", newTemp);
    }
    request->send_P(200, "text/plain", "OK");
  });
  server.on("/changePidKp", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("Kp")) {
      String str = request->getParam("Kp")->value();
      float newKp = str.toFloat();
      changePidKp(newKp);
      Serial.printf("New pid KP - - %f\n", newKp);
    }
    request->send_P(200, "text/plain", "OK");
  });
  server.on("/changePidKi", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("Ki")) {
      String str = request->getParam("Ki")->value();
      float newKi = str.toFloat();
      changePidKi(newKi);
    }
    request->send_P(200, "text/plain", "OK");
  });
  server.on("/changePidKd", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("Kd")) {
      String str = request->getParam("Kd")->value();
      float newKd = str.toFloat();
      changePidKd(newKd);
    }
    request->send_P(200, "text/plain", "OK");
  });
}


void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
