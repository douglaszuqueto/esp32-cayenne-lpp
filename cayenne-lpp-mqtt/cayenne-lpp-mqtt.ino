#include <WiFi.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <CayenneLPP.h>

WiFiClient espClient;
PubSubClient mqtt(espClient);
CayenneLPP lpp(51);


#define LED_BUILTIN 2

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.0.150";

unsigned long lastMsg = 0;

void setup_wifi() {

  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WiFi.SSID());

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  mqtt.setServer(mqtt_server, 1883);
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }

  // mqtt loop
  mqtt.loop();

  // send loop
  sendToBroker();
}

void sendToBroker() {
  unsigned long now = millis();

  if (now - lastMsg > 5000) {
    digitalWrite(LED_BUILTIN, 1);
    lastMsg = now;

    lpp.reset();
    lpp.addTemperature(1, 27.2);
    lpp.addTemperature(2, 25.5);
    lpp.addRelativeHumidity(1, 95);
    lpp.addRelativeHumidity(2, 87);

    Serial.println("Publishing message...");

    // Parameters:
    //    topic - the topic to publish to (const char[])
    //    payload - the message to publish (byte[])
    //    length - the length of the message (byte)
    mqtt.publish("device/01/metric", lpp.getBuffer(), lpp.getSize());
    digitalWrite(LED_BUILTIN, 0);
  }
}
