#define maxRecords 30

RTC_DATA_ATTR int loggerCounter = 0;

static char currDate [24];

void updateCurrDate() {
  timeClient.update();
  unsigned long timeNow = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&timeNow);
  sprintf(currDate, "%u_%u_%u.txt", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
}

typedef struct {
  float Temp;
  float Press;
  int BoilPw;
  unsigned long Time;
} sensorReadings;

RTC_DATA_ATTR sensorReadings Readings[maxRecords];

void updateLog() {
  timeClient.update();
  unsigned long timeNow = timeClient.getEpochTime();
  Readings[loggerCounter].Temp = currentTemperature;
  Readings[loggerCounter].Press = currentPressure;
  Readings[loggerCounter].BoilPw = heatPower;
  Readings[loggerCounter].Time = timeNow;
  loggerCounter++;

  if (loggerCounter >= maxRecords) {
    if (WiFi.status() == WL_CONNECTED) {
      ftp.OpenConnection();
      ftp.ChangeWorkDir("/ESD-USB/Gaggia");
      ftp.InitFile("Type A");
      ftp.AppendFile(currDate);
      char buffer [40 * maxRecords];
      int j = 0;
      for (int i = 0; i < maxRecords; i++) {
        j += sprintf(buffer + j, "%f,%f,%u,%u\n", Readings[i].Temp, Readings[i].Press, Readings[i].BoilPw, Readings[i].Time);
      }
      ftp.Write(buffer);
      ftp.CloseFile();
      ftp.CloseConnection();
    }
    loggerCounter = 0;
  }
}
