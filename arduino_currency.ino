// arduino currency
// NodeMCU ESP8266
// TFT1.8 SPI
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// 7 days * 10 work hours
#define START_WORK_HOUR 9
#define END_WORK_HOUR 19
#define CURRENCY_DIFF_SIZE 7*(END_WORK_HOUR - START_WORK_HOUR)

class ConnectionManager {
  public:
  ConnectionManager();
  bool connect(const char *ssid, const char *password);
  bool disconnect();
};

class TimeManager {
  public:
  bool request(const char *server);
  uint8_t hour();
  
  private:
  char *m_json;
  uint8_t m_hour;
  bool parseJSON();
};

enum CurrencyCodes { usd = 840, eur = 978 };

class CurrencyManager {
  public:
  bool request(const char *server);
  float getUSD();
  float getEUR();
  int8_t getUSDdiff(uint8_t h);
  int8_t getEURdiff(uint8_t h);
  
  private:
  float m_usd;
  float m_eur;
  float m_lastUsd;
  float m_lastEur;
  int8_t m_usdDiff[CURRENCY_DIFF_SIZE];
  int8_t m_eurDiff[CURRENCY_DIFF_SIZE];
  void saveUSD();
  void saveEUR();
};

class ScreenManager {
  public:
  ScreenManager();
  void clear();
  void printUSD();
  void printEUR();
  
  private:
  void setPosColor(uint8_t x, uint8_t y, uint8_t color);
  void printUSDcurrency();
  void printEURcurrency();
  void printUSDdiff();
  void printEURdiff();
};

//================main================

ConnectionManager cm;

void setup() {
  cm = ConnectionManager();
}

void loop() {
  cm.connect(ssid, password);
  cm.disconnect();
}
