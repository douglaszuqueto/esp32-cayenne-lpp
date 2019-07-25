#include <Arduino.h>
#include <CayenneLPP.h>

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

  delay(5000);
}
