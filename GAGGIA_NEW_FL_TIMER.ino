#include <TFT_eSPI.h> 
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <ESP32_FTPClient.h>
#include <AsyncTCP.h>
#include "GaggiaPid.h"
#include "icons.h"
#include "webpage.h"
#include "Thermo_LMT01.h"
#include "Manometer.h"
#include "Heater.h"
#include "timer.h"


#define BUTTON_PIN 32 //button
#define LMT_PIN 25

#define TFT_GREY 0xC618

int TARGET_TEMP;
int SENSORS_PERIOD;
int MAX_BOILER_PW;

char ftp_server[] = "10.0.0.1";
char ftp_user[]   = "gaggia";
char ftp_pass[]   = "123";
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";
boolean targetTempWasChanged = true;
volatile byte wsClients = 0;
float currentTemperature = 1;
float currentPressure = 0;
int heatPower;
boolean timeToCheckCounter = false;
boolean flowButtonIsPressed = false;


TFT_eSPI tft = TFT_eSPI();
Preferences preferences;
GaggiaPid regulator;
LMT01 thermoLmt;
Manometer manometer;
Heater heater;

Timer sensorsTimer;
Timer displayTimer(1000);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

ESP32_FTPClient ftp (ftp_server, ftp_user, ftp_pass, 5000, 2);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

class StopWatch {
  private:
    long startTime = 0;
    byte timerSecs = 0;
    byte timerMils = 0;
    byte memoryTimer = 40;
    int flowTickCounter = 0;
    boolean isFlow = false;
    boolean flowJustPressed = false;
    int timerTime;

    void printTime (byte posX, byte posY, byte fontSize) {
      tft.setTextColor(TFT_GREY, TFT_BLACK);
      if (timerSecs < 100) {
        if (timerSecs < 10) {
          posX += tft.drawChar('0', posX, posY, fontSize);
        }
        posX += tft.drawNumber(timerSecs, posX, posY, fontSize);
        posX += tft.drawChar('.', posX, posY, fontSize);
        if (timerMils < 10) {
          posX += tft.drawChar('0', posX, posY, fontSize);
        }
        tft.drawNumber(timerMils, posX, posY, fontSize);
      } else {
        posX += tft.drawNumber(timerSecs, posX, posY, fontSize);
        posX += tft.drawChar('.', posX, posY, fontSize);
        tft.drawNumber(timerMils / 10, posX, posY, fontSize);
      }
    }

    void sendWsMessage () {
      if (wsClients > 0) {
        char buffer[100];
        sprintf(buffer, "{\"name\":\"time\",\"state\":\"%s\",\"value\":\"%f\"}", isFlow ? "FLOW" : "Timer", timerTime / 1000.0);
        ws.textAll(buffer);
      }
    }

  public:
    void run() {
      byte posX = 146, posY = 192, fontSize = 7;
      if (isFlow) {
        flowTickCounter++;
        tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
        tft.drawString("FLOW ", 60, 224, 2);
        long currTime = millis();
        if (startTime == 0) {
          startTime = currTime;
          memoryTimer = 0;
        }
        timerTime = currTime - startTime;
        timerSecs =  timerTime / 1000;
        timerMils = (timerTime % 1000) / 10;
        printTime(posX, posY, fontSize);
        if (flowTickCounter % 2 == 0) {
          sendWsMessage ();
        } else if ( flowJustPressed ) {
          sendWsMessage ();
          flowJustPressed = false;
        }
      } else {
        if (memoryTimer == 0) {
          tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
          tft.drawString("TIMER", 60, 224, 2);
          flowTickCounter = 0;
          startTime = 0;
        }
        if (memoryTimer < 40 && timerTime > 0) {
          printTime(posX, posY, fontSize);
          if (displayTimer.ready()) {
            sendWsMessage ();
          } else if ( flowJustPressed ) {
            sendWsMessage ();
            flowJustPressed = false;
          }
          memoryTimer++;
        } else if (memoryTimer == 40) {
          timerTime = 0;
          tft.setTextColor(TFT_GREY, TFT_BLACK);
          posX += tft.drawString("00.00", posX, posY, fontSize);
          sendWsMessage ();
          memoryTimer++;
        }
      }
    }
    void setIsFlow (boolean isFlow) {
      this->isFlow = isFlow;
    }

    boolean getIsFlow () {
      return isFlow;
    }

    void setFlowJustPressed (boolean flowJustPressed) {
      this->flowJustPressed = flowJustPressed;
    }

    boolean getFlowJustPressed () {
      return flowJustPressed;
    }

};

StopWatch stopWatch;

void loop() {
  ws.cleanupClients();
  checkMachineButtonStatus();
  if (timeToCheckCounter) {
    checkCounter();
    timeToCheckCounter = false;
  }
  if (sensorsTimer.ready()) {
    currentTemperature = thermoLmt.getTemperature();
    currentPressure = manometer.getPressure();
    heatPower = 0;
    if (currentTemperature > 0 && currentTemperature < 105) {
      heatPower = regulator.getResult(currentTemperature);
    }
    heater.setHeatPower(heatPower);
    stopWatch.run();
  }
  heater.updateHeater();
  if (displayTimer.ready()) {
   //Feature below writing logs on local ftp, disabled
   // updateLog();
    updateInfoToClients();
    redrawDisplay();
  }
}
