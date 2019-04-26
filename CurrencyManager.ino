bool CurrencyManager::request(const char *server) {
  // request time from server
}

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
