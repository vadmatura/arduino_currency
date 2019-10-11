ConnectionManager::ConnectionManager() {
  // init WiFi
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_OFF);
  //WiFi.mode(WIFI_STA);
}

bool ConnectionManager::connectWiFi(const char *ssid, const char *password) {
  // connect to Wifi
  WiFi.begin(ssid, password);
  #ifdef DEBUG_SERIAL
  Serial.println(ssid);
  #endif
  int8_t maxConnectQuantity = 10;
  while (WiFi.status() != WL_CONNECTED && maxConnectQuantity) {
    delay(500);
    #ifdef DEBUG_SERIAL
    Serial.print(".");
    #endif
  }
  #ifdef DEBUG_SERIAL
  Serial.println("");
  #endif
  if (WiFi.status() == WL_CONNECTED) {
    #ifdef DEBUG_SERIAL
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    #endif
    return true;
  }
  return false;
}

void ConnectionManager::disconnectWiFi() {
  // disconnect to Wifi
  WiFi.disconnect();
  delay(500);
  #ifdef DEBUG_SERIAL
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status());
  Serial.println("WiFi disconnected");
  #endif
}

bool ConnectionManager::connectServer(const char *server, uint16_t port) {
  // request time from server
  if (!clientWiFi.connect(server, port)) {
    #ifdef DEBUG_SERIAL
    Serial.println("Connection failed");
    Serial.println(server);
    Serial.println(port);
    #endif
    return false;
  }
  if (clientWiFi.connected()) {
    #ifdef DEBUG_SERIAL
    Serial.println("Connected");
    Serial.println(server);
    Serial.println(port);
    Serial.println("");
    #endif
    return true;
  }
  return false;
}

void ConnectionManager::disconnectServer() {
  clientWiFi.stop();
  delay(500);
  #ifdef DEBUG_SERIAL
  Serial.println("Server disconnected");
  #endif
}

bool ConnectionManager::request(const char *requestData) {
  data[0] = 0;
  clientWiFi.println(requestData);
  #ifdef DEBUG_SERIAL
  Serial.println(requestData);
  #endif
  // wait for data to be available
  unsigned long timeout = millis();
  while (clientWiFi.available() == 0) {
    if (millis() - timeout > 5000) {
      #ifdef DEBUG_SERIAL
      Serial.println(">>> Client Timeout !");
      #endif
      clientWiFi.stop();
      delay(60000);
      return false;
    }
    yield();
  }

  uint16_t dataPos = 0;
  uint8_t maxPackageQuantity = 8; //Data from server gets with packages. Check 8 time if the new pakcage exists.
  while (maxPackageQuantity) {
    // Read all the lines of the reply from server and print them to Serial
    // not testing 'clientWiFi.connected()' since we do not need to send data here
    while (clientWiFi.available() && dataPos < DATA_BUFFER_SIZE) {
      char ch = static_cast<char>(clientWiFi.read());
      data[dataPos] = ch;
      dataPos++;
      yield();
    }
    delay(500);
    maxPackageQuantity--;
  }
  data[dataPos] = 0; //Add zero symbol to the end of the data for next manipulation.
  
  #ifdef DEBUG_SERIAL
  dataPos = 0;
  while (data[dataPos] != 0 && dataPos < DATA_BUFFER_SIZE) {
    Serial.print((char)data[dataPos]);
    dataPos++;
  }
  Serial.println("");
  #endif
}
