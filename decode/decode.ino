#include <Arduino.h>
#include <CayenneLPP.h>
#include <CayenneLPPDec.h>

CayenneLPP lpp(51);

void setup() {
  Serial.begin(115200);
}

void loop() {
  lpp.reset();
  lpp.addTemperature(1, 27.2);
  lpp.addTemperature(2, 25.5);
  lpp.addRelativeHumidity(1, 95);
  lpp.addRelativeHumidity(2, 87);

  uint8_t *payload = lpp.getBuffer();

  char buffer[7];
  String payloadString;

  for (int i = 0; i < lpp.getSize(); i++) {
    sprintf(buffer, "%02x", payload[i]);
    payloadString += buffer;
  }

  Serial.println();
  Serial.println("################### LOOP ###################");

  Serial.print("HEX: ");
  Serial.print(payloadString);
  Serial.print(" | SIZE: ");
  Serial.println(payloadString.length());


  // DECODE

  StaticJsonDocument<512> jsonBuffer;
  JsonArray root = jsonBuffer.createNestedArray();

  CayenneLPPDec::ParseLPP (payload, lpp.getSize(), root);

  // Imprime o decode no monitor serial
  serializeJsonPretty (root, Serial);

  JsonObject temp_1 = root[0];
  JsonObject temp_2 = root[1];
  JsonObject umid_1 = root[2];
  JsonObject umid_2 = root[3];

  int temp_1_channel = temp_1["channel"];
  int temp_2_channel = temp_2["channel"];
  int umid_1_channel = umid_1["channel"];
  int umid_2_channel = umid_2["channel"];

  const char* temp_1_type = temp_1["type"];
  const char* temp_2_type = temp_2["type"];
  const char* umid_1_type = umid_1["type"];
  const char* umid_2_type = umid_2["type"];

  float temp_1_value = temp_1["value"];
  float temp_2_value = temp_2["value"];
  float umid_1_value = umid_1["value"];
  float umid_2_value = umid_2["value"];

  // Mostra os valores na monitor serial
  Serial.println();

  debug(temp_1_channel, temp_1_type, temp_1_value);
  debug(temp_2_channel, temp_2_type, temp_2_value);
  debug(umid_1_channel, umid_1_type, umid_1_value);
  debug(umid_2_channel, umid_2_type, umid_2_value);

  Serial.println();
  delay(5000);
}

void debug(int channel, const char* type, float value) {
  Serial.print("channel: ");
  Serial.print(channel);
  Serial.print(" | type: ");
  Serial.print(type);
  Serial.print(" | value: ");
  Serial.println(value);
}

