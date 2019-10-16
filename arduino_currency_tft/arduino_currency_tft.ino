/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

/*
 * ESP8266-12        HY-1.8 SPI
 * GPIO5             Pin 06 (RESET)
 * GPIO2             Pin 07 (A0)
 * GPIO13 (HSPID)    Pin 08 (SDA)
 * GPIO14 (HSPICLK)  Pin 09 (SCK)
 * GPIO15 (HSPICS)   Pin 10 (CS)
 */
#define TFT_PIN_CS   10
#define TFT_PIN_DC   9
#define TFT_PIN_RST  8

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 (including HalloWing) use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);

// For 1.54" TFT with ST7789:
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
/*
$24.67 06f906fffc0707f9fb03ff00ff010701fc04fcfd030403f9fb03fd07ff03fc00fc050307070005fffefd05fafe0404fc020302fffe04fc070201fefd0604fb00fc040700fc040606fe07fffa0404fefe@27.81 06f9fa00fafa020604050507fafc0307fd070403070206fcff0200f90103fa04fe050303fd01f900f906fbf9fef903fcfa0100fafafefc01fa01fb01000001fafa05fe02ff0205040703fafd00fffdff#
$24.67 01fffbfdfffcfe040002fcfefc0701fe0602fa07fbff05fcfbfe0706030703fe07fd03f905020103fdf9fb02fcf903fc060502fd02fa0404ff05fd0100fdfd0305fdfff906fbfdfb00fa00fe05fcfd01@27.81 00fd07fd070204fdfcfefffcfb0004ffff0004ff05fd050604fd00fbff00fcfc01fb020601ff030501fafd02020101ff0104fbfa02060003fffa02fafa04fd070401070505fefe03f9fc07000007fafa#
$24.67 fffbfdfdfdfaff0606fafbfefe0001fd07f904fc01fdfa06fe01ff04040201fdfbfc02040102fcfcfc07fc04fffefc00fefbfefa0307fdfa06000303fb00fbfdfbfe06ff0003fefd03000407fdfe03fa@27.81 fcfd00fdf900f9fa0000ff000305fcfc01fb040704fbfbf9fafbfe03fb0604fffffcfcf903000000fffc0700fffffb0503fb02f9fe03fefef901fdfe02050302fd0407fb0505000403050605fafbfe00#
$24.67 fc06fb02fafc000300ff02fafd0501f90205fefc06fdfc07f
eff020305f907fdf902fe04fdf9010504fafb030404fffefdf904f90
6ff040206fbfa000700f90404f9fcfcf9fcfdfd0404fefdfcfbfb06
@27.81 fb0701fbff0502fef9f901fffb07f90704fb00fe06fdfafcf
d0100fbffff01fcfcfd0701fc03fafffc07ff0200fcfd0506f90006f
b0306fefbfefefc0700f90300fd00f900fc04f9fa01fcfdfdfcfa06#
*/
#define CURRENCY_DIFF_LENGTH 80
#define CURRENCY_INCREASE_LENGTH 4
#define CURRENCY_DIFF_CHAR_LENGTH (20 + CURRENCY_DIFF_LENGTH * 8)
#define CURRENCY_CODE_USD '$'
#define CURRENCY_CODE_EUR '@'
#define CURRENCY_CODE_END '#'
float currencyUSD = 0.0;
float currencyEUR = 0.0;
int8_t currencyDiffArrayUSD[CURRENCY_DIFF_LENGTH];
int8_t currencyDiffArrayEUR[CURRENCY_DIFF_LENGTH];
char currencyData[CURRENCY_DIFF_CHAR_LENGTH];
uint16_t dataPos = 0;

void fillData() {
  strcpy(currencyData, "$24.67 ");
  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
    uint8_t currency = (uint8_t)(random(1,16) - 8);
    String hex = String(currency, HEX);
    if (hex.length() == 1) {
      strcat(currencyData, "0");
    }
    strcat(currencyData, hex.c_str());
  }
  strcat(currencyData, "@27.81 ");
  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
    uint8_t currency = (uint8_t)(random(1,16) - 8);
    String hex = String(currency, HEX);
    if (hex.length() == 1) {
      strcat(currencyData, "0");
    }
    strcat(currencyData, hex.c_str());
  }
  strcat(currencyData, "#");
  Serial.println(currencyData);
}

bool readData() {
  while (Serial.available() && (dataPos < CURRENCY_DIFF_CHAR_LENGTH)) {
    char ch = static_cast<char>(Serial.read());
    if (ch == CURRENCY_CODE_USD) {
      dataPos = 0;
    }
    if (ch == '\r' || ch == '\n') {
      continue;
    }
    Serial.print(ch);
    Serial.print(" ");
    Serial.print(ch, HEX);
    Serial.print(" ");
    Serial.println(dataPos);
    currencyData[dataPos] = ch;
    dataPos++;
    yield();
    if (ch == CURRENCY_CODE_END) {
      currencyData[dataPos] = 0;
      Serial.println();
      for(uint16_t i = 0; i < CURRENCY_DIFF_CHAR_LENGTH; i++) {
        Serial.print(currencyData[i]);
      }
      Serial.println();
      return true;
    }
  }
  return false;
}

void decodeData(const char currencyCode) {
  char const *cUSD = strchr(currencyData, currencyCode);
  char cDIFF[3];
  if (cUSD) {
    cUSD++;
    if (currencyCode == CURRENCY_CODE_USD) {
      currencyUSD = atof(cUSD);
    }
    if (currencyCode == CURRENCY_CODE_EUR) {
      currencyEUR = atof(cUSD);
    }
    cUSD = strchr(cUSD, ' ');
    cUSD++;
    int8_t diff = 0;
    for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
      cDIFF[0] = *cUSD;
      cUSD++;
      cDIFF[1] = *cUSD;
      cUSD++;
      cDIFF[2] = 0;
      diff += (int8_t)strtol(cDIFF, 0, HEX);
      if (currencyCode == CURRENCY_CODE_USD) {
        currencyDiffArrayUSD[i] = diff;
      }
      if (currencyCode == CURRENCY_CODE_EUR) {
        currencyDiffArrayEUR[i] = diff;
      }
    }

    if (currencyCode == CURRENCY_CODE_USD) {
      Serial.println(currencyUSD);
      for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
        Serial.print(currencyDiffArrayUSD[i]);
        Serial.print(" ");
      }
    }
    if (currencyCode == CURRENCY_CODE_EUR) {
      Serial.println(currencyEUR);
      for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
        Serial.print(currencyDiffArrayEUR[i]);
        Serial.print(" ");
      }
    }
    Serial.println();
  }
}

void printUSD() {
  int16_t minDiff = 32767;
  int16_t increase = 0;
  increase = currencyDiffArrayUSD[CURRENCY_DIFF_LENGTH - 1] - currencyDiffArrayUSD[CURRENCY_DIFF_LENGTH - CURRENCY_INCREASE_LENGTH - 1];
  Serial.println(increase);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ST7735_BLUE);
  tft.setCursor(8, 4);
  tft.print("USD");
  tft.setTextSize(5);
  if (increase >= 0) {
    tft.setTextColor(ST7735_GREEN);
  } else {
    tft.setTextColor(ST7735_RED);
  }
  tft.setCursor(8, 36);
  tft.print(currencyUSD);

  tft.setTextSize(1);
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(100, 4);
  tft.print("EUR");
  tft.setCursor(120, 4);
  tft.print(currencyEUR);

  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
    if (minDiff > currencyDiffArrayUSD[i])
    {
      minDiff = currencyDiffArrayUSD[i];
    }
  }
  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH - 1; i++) {
    const uint8_t x1 = i * 2;
    const uint8_t x2 = x1 + 2;
    const int8_t y1 = minDiff - currencyDiffArrayUSD[i];
    const int8_t y2 = minDiff - currencyDiffArrayUSD[i+1];
    tft.drawLine(x1, 124 + y1, x2, 124 + y2, ST7735_WHITE);
    tft.drawLine(x1, 125 + y1, x2, 125 + y2, ST7735_WHITE);
    tft.drawLine(x1, 126 + y1, x2, 126 + y2, ST7735_WHITE);
    tft.drawLine(x1, 127 + y1, x2, 127 + y2, ST7735_WHITE);
  }
}

void printEUR() {
  int16_t minDiff = 32767;
  int16_t increase = 0;
  increase = currencyDiffArrayEUR[CURRENCY_DIFF_LENGTH - 1] - currencyDiffArrayEUR[CURRENCY_DIFF_LENGTH - CURRENCY_INCREASE_LENGTH - 1];
  Serial.println(increase);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ST7735_BLUE);
  tft.setCursor(8, 4);
  tft.print("EUR");
  tft.setTextSize(5);
  if (increase >= 0) {
    tft.setTextColor(ST7735_GREEN);
  } else {
    tft.setTextColor(ST7735_RED);
  }
  tft.setCursor(8, 36);
  tft.print(currencyEUR);

  tft.setTextSize(1);
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(100, 4);
  tft.print("USD");
  tft.setCursor(120, 4);
  tft.print(currencyUSD);

  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
    if (minDiff > currencyDiffArrayEUR[i])
    {
      minDiff = currencyDiffArrayEUR[i];
    }
  }
  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH - 1; i++) {
    const uint8_t x1 = i * 2;
    const uint8_t x2 = x1 + 2;
    const int8_t y1 = minDiff - currencyDiffArrayEUR[i];
    const int8_t y2 = minDiff - currencyDiffArrayEUR[i+1];
    tft.drawLine(x1, 124 + y1, x2, 124 + y2, ST7735_WHITE);
    tft.drawLine(x1, 125 + y1, x2, 125 + y2, ST7735_WHITE);
    tft.drawLine(x1, 126 + y1, x2, 126 + y2, ST7735_WHITE);
    tft.drawLine(x1, 127 + y1, x2, 127 + y2, ST7735_WHITE);
  }
}

void setup(void) {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  delay(2000); // wait for uart to settle and print Espressif blurb..
  Serial.println("Hello!!!");
  Serial.println(CURRENCY_DIFF_CHAR_LENGTH);
  tft.initR(INITR_BLACKTAB);
  tft.setTextWrap(false);
  tft.setTextColor(ST7735_WHITE);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  for(uint16_t i = 0; i < CURRENCY_DIFF_CHAR_LENGTH; i++) {
    currencyData[i] = 0;
  }
}

bool isUSD = true;

void loop() {
  if (readData()) {
    decodeData(CURRENCY_CODE_USD);
    decodeData(CURRENCY_CODE_EUR);
    if (isUSD) {
      printUSD();
    } else {
      printEUR();
    }
    isUSD = !isUSD;
  }
}
