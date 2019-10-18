// arduino currency
// NodeMCU ESP8266
// TFT1.8 SPI
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//#define DEBUG_SERIAL
#define START_WORK_HOUR 9
#define END_WORK_HOUR 19
#define CURRENCY_DIFF_SIZE 40
#define DATA_BUFFER_SIZE 8192
#define MILLISECONDS_PER_HOUR 30000
#define MON "Monday"
#define TUE "Tuesday"
#define WED "Wednesday"
#define THU "Thursday"
#define FRI "Friday"
#define SAT "Saturday"
#define SUN "Sunday"
#define WEEKEND 5
#define STR_USD "USD"
#define STR_EUR "EUR"

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

class TimeManager {
  public:
  bool parseJSON(char *json);
  int8_t getHour();
  int8_t getDayOfWeek();  //0 - monday, 6 - sunday
  private:
  int8_t m_hour;
  int8_t m_dayOfWeek;
};

enum CurrencyCodes { usd = 840, eur = 978 };

class CurrencyManager {
  public:
  CurrencyManager();
  bool parseJSON(char *json);
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

//================main================

ConnectionManager connectionManager;
CurrencyManager currencyManager;
TimeManager timeManager;
WiFiClient clientWiFi;
const char *cTime = "\r\n\r\n{\"currentDateTime\":\"2019-10-07T14:10Z\",\"dayOfTheWeek\":\"Monday\"}";
const char *cCurrency = "\r\n\r\n[{\"codeNumeric\":\"978\",\"codeAlpha\":\"EUR\",\"rateBuy\":\"27.1500\",\"rateBuyDelta\":0.25,\"rateSale\":\"27.6500\",\"rateSaleDelta\":0.25},{\"codeNumeric\":\"840\",\"codeAlpha\":\"USD\",\"rateBuy\":\"24.8500\",\"rateBuyDelta\":0.3,\"rateSale\":\"25.2000\",\"rateSaleDelta\":0.25},{\"codeNumeric\":\"978\",\"codeAlpha\":\"EUR\",\"rateBuy\":\"27.1500\",\"rateBuyDelta\":0.15,\"rateSale\":\"27.5900\",\"rateSaleDelta\":0.09},{\"codeNumeric\":\"840\",\"codeAlpha\":\"USD\",\"rateBuy\":\"24.7500\",\"rateBuyDelta\":0.039999999999999,\"rateSale\":\"25.0500\",\"rateSaleDelta\":0.060000000000002},{\"codeNumeric\":\"643\",\"codeAlpha\":\"RUB\",\"rateBuy\":\"0.3600\",\"rateBuyDelta\":0,\"rateSale\":\"0.3860\",\"rateSaleDelta\":0.001},{\"codeNumeric\":\"978\",\"codeAlpha\":\"EUR\",\"rateBuy\":\"26.9000\",\"rateBuyDelta\":0,\"rateSale\":\"27.7000\",\"rateSaleDelta\":0},{\"codeNumeric\":\"840\",\"codeAlpha\":\"USD\",\"rateBuy\":\"24.7500\",\"rateBuyDelta\":0,\"rateSale\":\"25.2500\",\"rateSaleDelta\":0},{\"codeNumeric\":\"643\",\"codeAlpha\":\"RUB\",\"rateBuy\":\"0.3550\",\"rateBuyDelta\":0,\"rateSale\":\"0.3900\",\"rateSaleDelta\":0}]";

void UpdateTime() {
  if (true) {//connectionManager.connectServer("52.165.220.33", 80)) {
    //connectionManager.request("GET /api/json/utc/now HTTP/1.1\r\nHost: worldclockapi.com\r\n");
    //connectionManager.disconnectServer();
    for (int i = 0; i <= strlen(cTime); i++) { connectionManager.data[i] = cTime[i]; }
    char *sjson = strstr(connectionManager.data, "\r\n\r\n"); //Get HTTP body begin.
    if (sjson) {
        sjson += 4;
        timeManager.parseJSON(sjson);
        #ifdef DEBUG_SERIAL
        Serial.println(timeManager.getHour());
        Serial.println(timeManager.getDayOfWeek());
        #endif
    }
  }
}

void UpdateCurrency() {
  if (true) {//connectionManager.connectServer("194.28.174.234", 80)) {
    //connectionManager.request("GET /export/exchange_rate_cash.json HTTP/1.1\r\nHost: bank-ua.com\r\n");
    //connectionManager.disconnectServer();
    //strcpy(connectionManager.data, "\r\n\r\n[{\"date\":\"2019-10-07\",\"bankName\":\"\u041e\u0422\u041f \u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/otpbank\/\",\"codeNumeric\":\"978\",\"codeAlpha\":\"EUR\",\"rateBuy\":\"27.1500\",\"rateBuyDelta\":0.25,\"rateSale\":\"27.6500\",\"rateSaleDelta\":0.25},{\"date\":\"2019-10-07\",\"bankName\":\"\u041e\u0422\u041f \u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/otpbank\/\",\"codeNumeric\":\"840\",\"codeAlpha\":\"USD\",\"rateBuy\":\"24.8500\",\"rateBuyDelta\":0.3,\"rateSale\":\"25.2000\",\"rateSaleDelta\":0.25},{\"date\":\"2019-10-07\",\"bankName\":\"\u041f\u0456\u0440\u0435\u0443\u0441 \u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/piraeusbank\/\",\"codeNumeric\":\"978\",\"codeAlpha\":\"EUR\",\"rateBuy\":\"27.1500\",\"rateBuyDelta\":0.15,\"rateSale\":\"27.5900\",\"rateSaleDelta\":0.09},{\"date\":\"2019-10-07\",\"bankName\":\"\u041f\u0456\u0440\u0435\u0443\u0441 \u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/piraeusbank\/\",\"codeNumeric\":\"840\",\"codeAlpha\":\"USD\",\"rateBuy\":\"24.7500\",\"rateBuyDelta\":0.039999999999999,\"rateSale\":\"25.0500\",\"rateSaleDelta\":0.060000000000002},{\"date\":\"2019-10-07\",\"bankName\":\"\u041f\u0456\u0440\u0435\u0443\u0441 \u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/piraeusbank\/\",\"codeNumeric\":\"643\",\"codeAlpha\":\"RUB\",\"rateBuy\":\"0.3600\",\"rateBuyDelta\":0,\"rateSale\":\"0.3860\",\"rateSaleDelta\":0.001},{\"date\":\"2019-10-07\",\"bankName\":\"\u041f\u0440\u0438\u0432\u0430\u0442\u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/privatbank\/\",\"codeNumeric\":\"978\",\"codeAlpha\":\"EUR\",\"rateBuy\":\"26.9000\",\"rateBuyDelta\":0,\"rateSale\":\"27.7000\",\"rateSaleDelta\":0},{\"date\":\"2019-10-07\",\"bankName\":\"\u041f\u0440\u0438\u0432\u0430\u0442\u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/privatbank\/\",\"codeNumeric\":\"840\",\"codeAlpha\":\"USD\",\"rateBuy\":\"24.7500\",\"rateBuyDelta\":0,\"rateSale\":\"25.2500\",\"rateSaleDelta\":0},{\"date\":\"2019-10-07\",\"bankName\":\"\u041f\u0440\u0438\u0432\u0430\u0442\u0411\u0430\u043d\u043a\",\"sourceUrl\":\"http:\/\/bank-ua.com\/banks\/privatbank\/\",\"codeNumeric\":\"643\",\"codeAlpha\":\"RUB\",\"rateBuy\":\"0.3550\",\"rateBuyDelta\":0,\"rateSale\":\"0.3900\",\"rateSaleDelta\":0}]");
    for (int i = 0; i <= strlen(cCurrency); i++) { connectionManager.data[i] = cCurrency[i]; }
    char *sjson = strstr(connectionManager.data, "\r\n\r\n"); //Get HTTP body begin.
    if (sjson) {
        sjson += 4;
        currencyManager.parseJSON(sjson);
        #ifdef DEBUG_SERIAL
        Serial.println(currencyManager.getUSD());
        Serial.println(currencyManager.getEUR());
        #endif
    }
  }
}

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  delay(1000);
}

unsigned long timeForDelay = -1;

void loop() {
  if (timeForDelay == -1 || millis() - timeForDelay > MILLISECONDS_PER_HOUR) {
    timeForDelay = millis();
    #ifdef DEBUG_SERIAL
    Serial.print("Time: ");
    Serial.println(timeForDelay / MILLISECONDS_PER_HOUR);
    #endif
    if (true) {//connectionManager.connectWiFi(ssid, password)) {
      UpdateTime();
      if ((START_WORK_HOUR <= timeManager.getHour()) && (timeManager.getHour() <= END_WORK_HOUR) && (timeManager.getDayOfWeek() < WEEKEND)) {
        UpdateCurrency();
        Serial.print("$");
        Serial.print(currencyManager.getUSD());
        Serial.print(" ");
        for (uint8_t i = 0; i < CURRENCY_DIFF_SIZE; i++) {
          if ((uint8_t)currencyManager.getUSDdiff(i) < 0x10) {
            Serial.print("0");
          }
          Serial.print((uint8_t)currencyManager.getUSDdiff(i), HEX);
          delay(10);
        }
        Serial.println();
        Serial.print("@");
        Serial.print(currencyManager.getEUR());
        Serial.print(" ");
        for (uint8_t i = 0; i < CURRENCY_DIFF_SIZE; i++) {
          if ((uint8_t)currencyManager.getEURdiff(i) < 0x10) {
            Serial.print("0");
          }
          Serial.print((uint8_t)currencyManager.getEURdiff(i), HEX);
          delay(10);
        }
        Serial.println("#");
      }
      //connectionManager.disconnectWiFi();
    }
  }
  if (millis() < timeForDelay) {
    timeForDelay = millis();
    #ifdef DEBUG_SERIAL
    Serial.println("New millis() cycle");
    #endif
  }
}
