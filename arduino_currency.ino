// arduino currency
// NodeMCU ESP8266
// TFT1.8 SPI
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <ArduinoJson.h>

#define DEBUG_SERIAL
// 7 days * 10 work hours
#define START_WORK_HOUR 9
#define END_WORK_HOUR 19
#define CURRENCY_DIFF_SIZE 7*(END_WORK_HOUR - START_WORK_HOUR)
#define DATA_BUFFER_SIZE 8192

class ConnectionManager {
  public:
  ConnectionManager();
  bool connectWiFi(const char *ssid, const char *password);
  bool connectServer(const char *server, uint16_t port);
  bool request(const char *requestData);
  void disconnectServer();
  void disconnectWiFi();
  char data[DATA_BUFFER_SIZE];
};

int8_t getHourFromJSON(char *json);

enum CurrencyCodes { usd = 840, eur = 978 };

class CurrencyManager {
  public:
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

ConnectionManager connectionManager;
WiFiClient clientWiFi;

void setup() {
  #ifdef DEBUG_SERIAL
  Serial.begin(9600);
  #endif
  if (connectionManager.connectWiFi(ssid, password)) {
    if (connectionManager.connectServer("52.165.220.33", 80)) {
      connectionManager.request("GET /api/json/utc/now HTTP/1.1\r\nHost: worldclockapi.com\r\n");
      char *sjson = strstr(connectionManager.data, "\r\n\r\n"); //Get HTTP body begin.
      if (sjson) {
          sjson += 4;
          #ifdef DEBUG_SERIAL
          Serial.println(getHourFromJSON(sjson));
          #endif
      }
      connectionManager.disconnectServer();
    }
    /*if (connectionManager.connectServer("194.28.174.234", 80)) {
      connectionManager.request("GET /export/exchange_rate_cash.json HTTP/1.1\r\nHost: bank-ua.com\r\n");
      connectionManager.disconnectServer();
    }*/
    connectionManager.disconnectWiFi();
  }
}

void loop() {
}
