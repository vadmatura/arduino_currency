int8_t getHourFromJSON(char *json) {
  StaticJsonDocument<512> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    #ifdef DEBUG_SERIAL
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    #endif
    return -1;
  }
  const char* currentDateTime = doc["currentDateTime"];
  uint8_t hour = -1;
  #ifdef DEBUG_SERIAL
  Serial.println(currentDateTime);
  #endif
  sscanf(currentDateTime, "%*d-%*d-%*dT%d:%*d", &hour);
  return hour;
}
