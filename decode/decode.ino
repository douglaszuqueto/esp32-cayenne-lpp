#include <Arduino.h>
#include <CayenneLPP.h>
#include <CayenneLPPDec.h>

CayenneLPP lpp(51);

void setup() {
  Serial.begin(115200);
}

void loop() {
  lpp.reset();
  lpp.addTemperature(1, 25);
  lpp.addTemperature(2, 22.5);
  lpp.addRelativeHumidity(1, 90);
  lpp.addRelativeHumidity(2, 87);

  uint8_t *payload = lpp.getBuffer();

  char buffer[7];
  String payloadString;

  for (int i = 0; i < lpp.getSize(); i++) {
    sprintf(buffer, "%02x", payload[i]);
    payloadString += buffer;
  }

  Serial.println();
  Serial.println("############### LOOP ###############");

  Serial.print("HEX: ");
  Serial.print(payloadString);
  Serial.print(" | SIZE: ");
  Serial.println(payloadString.length());


  StaticJsonDocument<512> jsonBuffer;
  JsonArray root = jsonBuffer.createNestedArray();

  CayenneLPPDec::ParseLPP (payload, lpp.getSize(), root);
  serializeJsonPretty (root, Serial);

  JsonObject root_0 = root[0];
  int root_0_channel = root_0["channel"]; // 1
  const char* root_0_type = root_0["type"]; // "temp"
  int root_0_value = root_0["value"]; // 25

  Serial.println();
  Serial.print("channel: ");
  Serial.print(root_0_channel);
  Serial.print(" | type: ");
  Serial.print(root_0_type);
  Serial.print(" | value: ");
  Serial.println(root_0_value);

  delay(5000);
}
