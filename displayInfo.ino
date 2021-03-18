static const byte topGraphX = 104;
static const byte bottomGraphX = 184;
static const byte graphStart = 20;
static const int graphLength = 279;
static byte tempGraphOffset = (bottomGraphX - topGraphX) / 2;
static byte middleGraphX = tempGraphOffset + topGraphX;
static byte resolution = 4;
static byte lowTemp;
static byte highTemp;
static int z = graphLength - 1;
static int tempGraph [graphLength];
static byte middleTemp = 0;
static boolean middleTempWasChanged = true;

void drawInitScreen () {
  int h = 48, w = 34, row, col, buffidx = 0;
  int offsetX = 0;
  int offsetY = 15;
  for (row = offsetX ; row < h + offsetX; row++) {
    for (col = offsetY; col < w + offsetY; col++) {
      tft.drawPixel(col, row, pgm_read_word(therm_big + buffidx));
      buffidx++;
    }
  }

  h = 48;
  w = 48;
  buffidx = 0;
  offsetX = 50;
  offsetY = 8;
  for (row = offsetX ; row < h + offsetX; row++) {
    for (col = offsetY; col < w + offsetY; col++) {
      tft.drawPixel(col, row, pgm_read_word(press_big + buffidx));
      buffidx++;
    }
  }

  h = 48;
  w = 46;
  buffidx = 0;
  offsetX = 192;
  offsetY = 9;
  for (row = offsetX ; row < h + offsetX; row++) {
    for (col = offsetY; col < w + offsetY; col++) {
      tft.drawPixel(col, row, pgm_read_word(timer_big + buffidx));
      buffidx++;
    }
  }

  tft.drawCircle(296, 35, 3, TFT_SKYBLUE);
  tft.setTextColor(TFT_SKYBLUE);
  tft.setTextDatum(BR_DATUM);
  tft.drawString("C", 310, 48, 2);
  tft.drawString("BAR", 320, 100, 2);
  tft.setTextDatum(BL_DATUM);
  tft.drawString("SET:", 60, 24, 2);
  tft.drawString("TEMP", 60, 48, 2);
  tft.drawString("PRESSURE", 60, 100, 2);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft.drawString("TIMER", 60, 224, 2);
  tft.setTextDatum(TR_DATUM);
  tft.drawString("S", 304, 224, 2);
  tft.setTextDatum(TL_DATUM);

  int graphEnd = graphStart + graphLength + 1;
  tft.drawLine(graphStart, topGraphX, graphStart, bottomGraphX, TFT_WHITE);
  tft.drawLine((graphStart - 4), (middleGraphX - 40), graphStart, (middleGraphX - 40), TFT_WHITE);
  tft.drawLine((graphStart - 2), (middleGraphX - 20), graphStart, (middleGraphX - 20), TFT_WHITE);
  tft.drawLine((graphStart - 4), middleGraphX, graphStart, middleGraphX, TFT_WHITE);
  tft.drawLine((graphStart - 2), (middleGraphX + 20), graphStart, (middleGraphX + 20), TFT_WHITE);
  tft.drawLine((graphStart - 4), (middleGraphX + 40), graphStart, (middleGraphX + 40), TFT_WHITE);
 // tft.drawLine(graphStart, (bottomGraphX + 1), graphEnd, (bottomGraphX + 1), TFT_WHITE);
  tft.drawLine(graphEnd, topGraphX, graphEnd, bottomGraphX, TFT_WHITE);
  tft.drawLine((graphEnd + 4), (middleGraphX - 40), graphEnd, (middleGraphX - 40), TFT_WHITE);
  tft.drawLine((graphEnd + 2), (middleGraphX - 20), graphEnd, (middleGraphX - 20), TFT_WHITE);
  tft.drawLine((graphEnd + 4), middleGraphX, graphEnd, middleGraphX, TFT_WHITE);
  tft.drawLine((graphEnd + 2), (middleGraphX + 20), graphEnd, (middleGraphX + 20), TFT_WHITE);
  tft.drawLine((graphEnd + 4), (middleGraphX + 40), graphEnd, (middleGraphX + 40), TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
}

void redrawDisplay () {
  drawTempGraph ();
  drawTemperature();
  drawPressure();
}

void drawTemperature () {
  if (targetTempWasChanged) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(BL_DATUM);
    if (TARGET_TEMP < 100) {
      byte tempX = 93 + tft.drawNumber(TARGET_TEMP, 93, 24, 2);
      tft.fillRect(tempX, 0, 12, 24, TFT_BLACK);
    } else {
      tft.drawNumber(TARGET_TEMP, 93, 24, 2);
    }
    tft.setTextDatum(TL_DATUM);
    targetTempWasChanged = false;
  }
  byte posX = 114;
  byte posY = 0;
  byte fontSize = 7;
  if (currentTemperature < 0) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("ERROR", posX + 35, posY + 20, 4);
  } else {
    if (currentTemperature < 100) {
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      byte tempPos = posX + tft.drawNumber(8, posX, posY, fontSize);
      tft.setTextColor(TFT_GREY, TFT_BLACK);
      tft.drawFloat(currentTemperature, 2 , tempPos, posY, fontSize);
    } else {
      tft.setTextColor(TFT_GREY, TFT_BLACK);
      tft.drawFloat(currentTemperature, 2 , posX, posY, fontSize);
    }
  }
}

void drawTempGraph () {
  if (abs(currentTemperature - middleTemp) > 3) {
    middleTemp = currentTemperature;
    middleTempWasChanged = true;
  }
  if (middleTempWasChanged) {
    lowTemp = middleTemp - tempGraphOffset / resolution;
    highTemp = middleTemp + tempGraphOffset / resolution;
    tft.setCursor(0, (topGraphX - 3));
    if (highTemp < 100) {
      tft.print(" ");
      tft.print(highTemp);
    } else {
      tft.print(highTemp);
    }
    tft.setCursor(0, (bottomGraphX - 3));
    if (lowTemp < 100) {
      tft.print(" ");
      tft.print(lowTemp);
    } else {
      tft.print(lowTemp);
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, (middleGraphX - 3));
    if (middleTemp < 100) {
      tft.print(" ");
      tft.print(middleTemp);
    } else {
      tft.print(middleTemp);
    }
    middleTempWasChanged = false;
  }
  int highLimit = highTemp * resolution;
  int lowLimit = lowTemp * resolution;
  int targetLimit = middleTemp * resolution;
  for (int i = 0; i <= z; i++) {
    if (i == z) {
      tempGraph[i] = round(currentTemperature * resolution);
    } else {
      tempGraph[i] = tempGraph [i + 1];
    }
    int offsetX = graphStart + 1 + i;
    tft.drawLine(offsetX, topGraphX, offsetX, bottomGraphX, TFT_BLACK);
    if (isWithinLimits(tempGraph[i], highLimit, lowLimit)) {
      if (isWithinLimits(tempGraph[i - 1], highLimit, lowLimit)) {
        tft.drawLine((offsetX - 1), (middleGraphX - (tempGraph[i - 1] - targetLimit)), offsetX, (middleGraphX - (tempGraph[i] - targetLimit)), TFT_RED);
      } else {
        tft.drawPixel(offsetX, middleGraphX - (tempGraph[i] - targetLimit), TFT_RED);
      }
    }
  }
}

boolean isWithinLimits (int temperature, int highLimit, int lowLimit) {
  return temperature < highLimit && temperature > lowLimit;
}

void drawPressure () {
  byte posX = 146;
  byte posY = 52;
  byte fontSize = 7;
  tft.setTextColor(TFT_GREY, TFT_BLACK);
  if (currentPressure > 0) {
    if (currentPressure < 10) {
      byte nextPos = posX + tft.drawChar('0', posX, posY, fontSize);
      tft.drawFloat(currentPressure, 2, nextPos, posY, fontSize);
    } else {
      tft.drawFloat(currentPressure, 2, posX, posY, fontSize);
    }
  } else {
    tft.drawString("00.00", posX, posY, fontSize);
  }
}
