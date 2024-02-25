#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <vector>

#define UPDATE_VALUE_INTERVAL 50
#define UPDATE_DIAGRAM_INTERVAL 200

#define GRAPH_X 1
#define GRAPH_Y 1
#define GRAPH_H 50
#define GRAPH_L 238

#define TFT_BL   21
#define TFT_CS   15 
#define TFT_DC    2
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_RST  2
#define LDR_PIN 34

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);
//Data
std::vector<float> buffer;

int bufMin = 0;
int bufMax = 0;
long randomNumber = 0;

void addValuetoBuffer(float value)
{
  if (buffer.size() == (GRAPH_L-10))
    buffer.erase(buffer.begin());
  buffer.push_back(value);
  bufMin = *std::min_element(buffer.begin(), buffer.end());
  bufMax = *std::max_element(buffer.begin(), buffer.end());
}

float factor(float diff, int height)
{
  if (diff < 5 && height > 30)
    return 2.5;
  if (diff < height) {
    return ((float)height / diff);
  } else {
    return (diff / (float)height);
  }
}

int baseCorrection(float diff, int height)
{
  if (diff < 5 && height > 30)
    return 10;
  else if (diff < 5 && height <= 30)
    return 1;
  else
    return 0;
}

void drawDiagram(bool forceUpdate = false)
{
  int xpos = GRAPH_X+2;
  int tDiff = (bufMax - bufMin);
  float fact = factor(tDiff, GRAPH_H-4);
  int base = (GRAPH_Y+GRAPH_H-2) - baseCorrection(tDiff, GRAPH_H);

  //update all value
  if ( forceUpdate ) {
    tft.drawRect(GRAPH_X, GRAPH_Y, GRAPH_L, GRAPH_H, ILI9341_BLACK);
    tft.fillRect(GRAPH_X+1, GRAPH_Y+1, GRAPH_L-2, GRAPH_H-2, ILI9341_LIGHTGREY);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(GRAPH_L-15, GRAPH_Y+2);
    tft.print(bufMax);
    tft.setCursor(GRAPH_L-15, (GRAPH_Y+GRAPH_H-10));
    tft.print(bufMin);


    for (float n : buffer) {
      int ypos = base - ((n - bufMin)*fact);
      //Serial.println((String)"x(" + xpos + ")y(" + ypos + ")t(" + n + ")");
      tft.drawPixel(xpos, ypos, ILI9341_RED);
      xpos++;
    }
  } else {
    //todo we want to update just the latest pixel
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting application...");

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  analogSetAttenuation(ADC_0db);
  pinMode(LDR_PIN, INPUT);

  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(16000000);
  tft.begin();

  randomSeed(analogRead(0));
}

void loop() {
  static int updateInterval = 0;
  if (updateInterval % UPDATE_VALUE_INTERVAL == 0) {
    addValuetoBuffer((float)random(2, 6));
  }
  if (updateInterval % UPDATE_DIAGRAM_INTERVAL == 0) {
    drawDiagram(true);
  }
  
  updateInterval++;
  delay(10);
}
