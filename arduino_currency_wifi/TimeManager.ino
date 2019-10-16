//"{\"$id\":\"1\",\"currentDateTime\":\"2019-10-07T14:10Z\",\"utcOffset\":\"00:00:00\",\"isDayLightSavingsTime\":false,\"dayOfTheWeek\":\"Monday\",\"timeZoneName\":\"UTC\",\"currentFileTime\":132149310448939833,\"ordinalDate\":\"2019-280\",\"serviceResponse\":null}"

bool TimeManager::parseJSON(char *json) {
  m_hour = -1;
  m_dayOfWeek = -1;

  StaticJsonDocument<512> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    #ifdef DEBUG_SERIAL
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    #endif
    return false;
  }
  const char* currentDateTime = doc["currentDateTime"];
  #ifdef DEBUG_SERIAL
  Serial.println(currentDateTime);
  #endif
  sscanf(currentDateTime, "%*d-%*d-%*dT%d:%*d", &m_hour);

  const char* dayOfWeek = doc["dayOfTheWeek"];
  #ifdef DEBUG_SERIAL
  Serial.println(dayOfWeek);
  #endif
  if (strcmp(dayOfWeek, MON) == 0) {
    m_dayOfWeek = 0;
  } 
  else if (strcmp(dayOfWeek, TUE) == 0) {
    m_dayOfWeek = 1;
  }
  else if (strcmp(dayOfWeek, WED) == 0) {
    m_dayOfWeek = 2;
  }
  else if (strcmp(dayOfWeek, THU) == 0) {
    m_dayOfWeek = 3;
  }
  else if (strcmp(dayOfWeek, FRI) == 0) {
    m_dayOfWeek = 4;
  }
  else if (strcmp(dayOfWeek, SAT) == 0) {
    m_dayOfWeek = 5;
  }
  else if (strcmp(dayOfWeek, SUN) == 0) {
    m_dayOfWeek = 6;
  }
  return true;
}

int8_t TimeManager::getHour() {
  return m_hour;
}

int8_t TimeManager::getDayOfWeek() { //0 - monday, 6 - sunday
  return m_dayOfWeek;
}
