/*
HTTP/1.1 200 OK
Server: nginx
Date: Wed, 22 May 2019 09:14:24 GMT
Content-Type: application/json
Content-Length: 2146
Connection: keep-alive
Vary: Accept-Encoding
Last-Modified: Wed, 22 May 2019 06:59:02 GMT
ETag: "862-58974801afb44"
Accept-Ranges: bytes
Vary: Accept-Encoding,User-Agent

[{"date":"2019-05-22","bankName":"\u041e\u0422\u041f \u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/otpbank\/","codeNumeric":"978","codeAlpha":"EUR","rateBuy":"29.0000","rateBuyDelta":0,"rateSale":"29.4500","rateSaleDelta":0},{"date":"2019-05-22","bankName":"\u041e\u0422\u041f \u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/otpbank\/","codeNumeric":"840","codeAlpha":"USD","rateBuy":"26.1000","rateBuyDelta":0,"rateSale":"26.3000","rateSaleDelta":0},{"date":"2019-05-21","bankName":"\u041f\u0456\u0440\u0435\u0443\u0441 \u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/piraeusbank\/","codeNumeric":"978","codeAlpha":"EUR","rateBuy":"29.0000","rateBuyDelta":-0.050000000000001,"rateSale":"29.4000","rateSaleDelta":-0.050000000000001},{"date":"2019-05-21","bankName":"\u041f\u0456\u0440\u0435\u0443\u0441 \u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/piraeusbank\/","codeNumeric":"840","codeAlpha":"USD","rateBuy":"26.0700","rateBuyDelta":-0.030000000000001,"rateSale":"26.2700","rateSaleDelta":-0.02},{"date":"2019-05-21","bankName":"\u041f\u0456\u0440\u0435\u0443\u0441 \u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/piraeusbank\/","codeNumeric":"643","codeAlpha":"RUB","rateBuy":"0.3900","rateBuyDelta":0,"rateSale":"0.4080","rateSaleDelta":0},{"date":"2019-05-22","bankName":"\u041f\u0440\u0438\u0432\u0430\u0442\u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/privatbank\/","codeNumeric":"978","codeAlpha":"EUR","rateBuy":"28.8000","rateBuyDelta":0,"rateSale":"29.4000","rateSaleDelta":0},{"date":"2019-05-22","bankName":"\u041f\u0440\u0438\u0432\u0430\u0442\u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/privatbank\/","codeNumeric":"840","codeAlpha":"USD","rateBuy":"26.0000","rateBuyDelta":0,"rateSale":"26.3500","rateSaleDelta":0},{"date":"2019-05-22","bankName":"\u041f\u0440\u0438\u0432\u0430\u0442\u0411\u0430\u043d\u043a","sourceUrl":"http:\/\/bank-ua.com\/banks\/privatbank\/","codeNumeric":"643","codeAlpha":"RUB","rateBuy":"0.3700","rateBuyDelta":0,"rateSale":"0.4100","rateSaleDelta":0}]
*/

CurrencyManager::CurrencyManager() {
  for (int i = 0; i < CURRENCY_DIFF_SIZE; i++) {
    m_usdDiff[i] = 0;
    m_eurDiff[i] = 0;
  }
  m_lastUsd = 0;
  m_lastEur = 0;
  m_usd = 0;
  m_eur = 0;
}

bool CurrencyManager::parseJSON(char *json) {
  StaticJsonDocument<512> doc;

  m_lastUsd = m_usd;
  m_lastEur = m_eur;

  char *subJsonStart = strstr(json, "{");
  char *subJsonEnd = strstr(json, "}");
  char c = 0;
  while (subJsonStart != NULL) {
    subJsonEnd = strstr(subJsonStart, "}");
    if (subJsonEnd == NULL) {
      break;
    }
    subJsonEnd++; 
    c = *subJsonEnd;
    *subJsonEnd = 0;
    
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, subJsonStart);
  
    // Test if parsing succeeds.
    if (error) {
      #ifdef DEBUG_SERIAL
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      #endif
      break;
    }
    const char* codeAlpha = doc["codeAlpha"];
    const char* rateSale = doc["rateSale"];
    #ifdef DEBUG_SERIAL
    Serial.print("rateSale: ");
    Serial.print(codeAlpha);
    Serial.println(rateSale);
    #endif
    if (strcmp(codeAlpha, STR_USD) == 0) {
      m_usd = atof(rateSale);
    }
    if (strcmp(codeAlpha, STR_EUR) == 0) {
      m_eur = atof(rateSale);
    }
    *subJsonEnd = c;
    subJsonStart = strstr(subJsonEnd, "{");
  }
  m_usd = m_usd + random(0,63) / 100;
  m_eur = m_eur + random(0,63) / 100;
  if (m_lastUsd == 0) {
    m_lastUsd = m_usd;
  }
  if (m_lastEur == 0) {
    m_lastEur = m_eur;
  }
  saveUSD();
  saveEUR();
  return true;
}

float CurrencyManager::getUSD() {
  return m_usd;
}

float CurrencyManager::getEUR() {
  return m_eur;
}

int8_t CurrencyManager::getUSDdiff(uint8_t h) {
  if (h < CURRENCY_DIFF_SIZE) {
    return m_usdDiff[h];
  }
}

int8_t CurrencyManager::getEURdiff(uint8_t h) {
  if (h < CURRENCY_DIFF_SIZE) {
    return m_eurDiff[h];
  }
}

void CurrencyManager::saveUSD() {
  for (uint8_t i = 0; i < CURRENCY_DIFF_SIZE - 1; i++) {
    m_usdDiff[i] = m_usdDiff[i + 1];
  }
  m_usdDiff[CURRENCY_DIFF_SIZE - 1] = (uint8_t)((m_lastUsd - m_usd) * 100);
  #ifdef DEBUG_SERIAL
  Serial.print("m_usdDiff: ");
  for (int i = 0; i < CURRENCY_DIFF_SIZE; i++) {
    Serial.print(m_usdDiff[i]);
    Serial.print(" ");
  }
  Serial.println();
  #endif
}

void CurrencyManager::saveEUR() {
  for (uint8_t i = 0; i < CURRENCY_DIFF_SIZE - 1; i++) {
    m_eurDiff[i] = m_eurDiff[i + 1];
  }
  m_eurDiff[CURRENCY_DIFF_SIZE - 1] = (uint8_t)((m_lastEur - m_eur) * 100);
  #ifdef DEBUG_SERIAL
  Serial.print("m_usdDiff: ");
  for (uint8_t i = 0; i < CURRENCY_DIFF_SIZE; i++) {
    Serial.print(m_eurDiff[i]);
    Serial.print(" ");
  }
  Serial.println();
  #endif
}
