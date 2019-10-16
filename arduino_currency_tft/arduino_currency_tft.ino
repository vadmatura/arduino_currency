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
$24.67 fc06fb02fafc000300ff02fafd0501f90205fefc06fdfc07f
eff020305f907fdf902fe04fdf9010504fafb030404fffefdf904f90
6ff040206fbfa000700f90404f9fcfcf9fcfdfd0404fefdfcfbfb06
@27.81 fb0701fbff0502fef9f901fffb07f90704fb00fe06fdfafcf
d0100fbffff01fcfcfd0701fc03fafffc07ff0200fcfd0506f90006f
b0306fefbfefefc0700f90300fd00f900fc04f9fa01fcfdfdfcfa06#
*/

//#define DEBUG_SERIAL
#define CURRENCY_DIFF_LENGTH 40
#define CURRENCY_INCREASE_LENGTH 4
#define CURRENCY_DIFF_CHAR_LENGTH (24 + CURRENCY_DIFF_LENGTH * 6)
#define CURRENCY_CODE_USD '$'
#define CURRENCY_CODE_EUR '@'
#define CURRENCY_CODE_END '#'
#define CURRENCY_CHANGE_MS 10000

struct CurrencySet {
  float val;
  int8_t diff[CURRENCY_DIFF_LENGTH];
  char name[4];
} usd, eur;
char currencyData[CURRENCY_DIFF_CHAR_LENGTH];
uint16_t dataPos = 0;
bool isUSD = true;
bool isReadComplete = false;
unsigned long timeForDelay = -1;

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
  #ifdef DEBUG_SERIAL
  Serial.println(currencyData);
  #endif
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
    currencyData[dataPos] = ch;
    dataPos++;
    yield();
    if (ch == CURRENCY_CODE_END) {
      currencyData[dataPos] = 0;
      return true;
    }
  }
  return false;
}

void decodeData(const char currencyCode, CurrencySet &currency) {
  char const *c = strchr(currencyData, currencyCode);
  char cDIFF[3];
  if (c) {
    c++;
    currency.val = atof(c);
    c = strchr(c, ' ');
    c++;
    int8_t diff = 0;
    for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
      cDIFF[0] = *c;
      c++;
      cDIFF[1] = *c;
      c++;
      cDIFF[2] = 0;
      diff += (int8_t)strtol(cDIFF, 0, HEX);
        currency.diff[i] = diff;
    }

    #ifdef DEBUG_SERIAL
    Serial.println(currency.val);
    for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
      Serial.print(currency.diff[i]);
      Serial.print(" ");
    }
    Serial.println();
    #endif
  }
}

void printCurrency(CurrencySet &mainCurrency, CurrencySet &secondCurrency) {
  int16_t minDiff = 32767;
  int16_t increase = 0;
  increase = mainCurrency.diff[CURRENCY_DIFF_LENGTH - 1] - mainCurrency.diff[CURRENCY_DIFF_LENGTH - CURRENCY_INCREASE_LENGTH - 1];
  #ifdef DEBUG_SERIAL
  Serial.println(increase);
  #endif
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ST7735_BLUE);
  tft.setCursor(8, 4);
  tft.print(mainCurrency.name);
  tft.setTextSize(5);
  if (increase >= 0) {
    tft.setTextColor(ST7735_GREEN);
  } else {
    tft.setTextColor(ST7735_RED);
  }
  tft.setCursor(8, 36);
  tft.print(mainCurrency.val);

  tft.setTextSize(1);
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(100, 4);
  tft.print(secondCurrency.name);
  tft.setCursor(120, 4);
  tft.print(secondCurrency.val);

  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH; i++) {
    if (minDiff > mainCurrency.diff[i])
    {
      minDiff = mainCurrency.diff[i];
    }
  }
  for(uint8_t i = 0; i < CURRENCY_DIFF_LENGTH - 1; i++) {
    const uint8_t x1 = i * 4;
    const uint8_t x2 = x1 + 4;
    const int8_t y1 = minDiff - mainCurrency.diff[i];
    const int8_t y2 = minDiff - mainCurrency.diff[i+1];
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
  #ifdef DEBUG_SERIAL
  Serial.println("TFT");
  Serial.println(CURRENCY_DIFF_CHAR_LENGTH);
  #endif
  tft.initR(INITR_BLACKTAB);
  tft.setTextWrap(false);
  tft.setTextColor(ST7735_WHITE);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  for(uint16_t i = 0; i < CURRENCY_DIFF_CHAR_LENGTH; i++) {
    currencyData[i] = 0;
  }
  strcpy(usd.name, "USD");
  strcpy(eur.name, "EUR");

  //fillData();
  //decodeData(CURRENCY_CODE_USD, usd);
  //decodeData(CURRENCY_CODE_EUR, eur);
}

void loop() {
  if (readData()) {
    #ifdef DEBUG_SERIAL
    Serial.println();
    for(uint16_t i = 0; i < CURRENCY_DIFF_CHAR_LENGTH; i++) {
      Serial.print(currencyData[i]);
    }
    Serial.println();
    #endif
    decodeData(CURRENCY_CODE_USD, usd);
    decodeData(CURRENCY_CODE_EUR, eur);
  }

  if (timeForDelay == -1 || millis() - timeForDelay > CURRENCY_CHANGE_MS) {
    if (isUSD) {
      printCurrency(usd, eur);
    } else {
      printCurrency(eur, usd);
    }
    isUSD = !isUSD;
    timeForDelay = millis();
  }
  if (millis() < timeForDelay) {
    timeForDelay = millis();
    #ifdef DEBUG_SERIAL
    Serial.println("New millis() cycle");
    #endif
  }
}
