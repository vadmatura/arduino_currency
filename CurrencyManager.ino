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

float CurrencyManager::getUSD() {
}

float CurrencyManager::getEUR() {
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
  
}

void CurrencyManager::saveEUR() {
  
}
