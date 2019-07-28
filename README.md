# ESP32 Cayenne LPP

## Introdução

Dentre os projetos atuais, eu estou desenvolvendo uma plataforma IoT de um projeto de sensoriamento sobre o ecossistema LoRaWan, então foi através dele que eu cheguei a esse magnífico formato de payload :)

Portanto, este repositório mostrará de forma bem objetiva um pouco sobre o Cayenne LPP e também mostrar um exemplo de encode/decode utilizando o ESP32.

Caso tenha interesse em saber mais sobre Cayenne LPP a referência estará em anexo.

## Materiais utilizados

* ESP32 Dev module

## Bibliotecas utilizadas

* Encode - CayenneLPP library
* Decode - CayenneLPPdec library

**Obs¹:** O link das bibliotecas citadas encontram-se no final deste documento.
**Obs²:** A biblioteca para decode, tem como dependência a biblioteca [ArduinoJSON](https://arduinojson.org/) v6, então a mesma precisa estar instalada em sua IDE Arduino.

## Cayenne Low Power Payload(LPP)

*"The Cayenne Low Power Payload (LPP) provides a convenient and easy way to send data over LPWAN networks such as LoRaWAN. The Cayenne LPP is compliant with the payload size restriction, which can be lowered down to 11 bytes, and allows the device to send multiple sensor data at a time or splitted into several frames."*

### Estrutura

Basicamente a estrutura da payload é dividida em: canal, tipo e o valor da mensagem em si.

| 1 Byte | 1 Byte | N Bytes | 1 Byte | 1 Byte | M Bytes | ...
|---|---|---|---|---|---|---|
|Data1 Ch.|Data1 Type|Data1|Data2 Ch.|Data2 Type|Data2 | ...

*[fonte](https://mydevices.com/cayenne/docs/lora/#lora-cayenne-low-power-payload)*

### Tipos suportados

| Type | IPSO | LPP | Hex | Data size | Data Resolution per bit
:-----:|:-----:|:-----:|:-----:|:-----:|:-----:
Digital Input|3200|0|0|1|1
Digital Output|3201|1|1|1|1
Analog Input|3202|2|2|2|0.01 Signed
Analog Output|3203|3|3|2|0.01 Signed
Illuminance Sensor|3301|101|65|2|1 Lux Unsigned MSB
Presence Sensor|3302|102|66|1|1
Temperature Sensor|3303|103|67|2|0.1 °C Signed MSB
Humidity Sensor|3304|104|68|1|0.5 % Unsigned
Accelerometer|3313|113|71|6|0.001 G Signed MSB per axis
Barometer|3315|115|73|2|0.1 hPa Unsigned MSB
Gyrometer|3334|134|86|6|0.01 °/s Signed MSB per axis
GPS Location|3336|136|88|9|Latitude : 0.0001 ° Signed MSB

*[fonte](https://mydevices.com/cayenne/docs/lora/#lora-cayenne-low-power-payload)*

### Exemplos

Supondo que o projeto X tenha um sensor de temperatura e outro de umidade, teríamos a seguinte payload:

| Byte 1 | Byte 2 | Byte 3/4 | Byte 5 | Byte 6 | Byte 7 |
|---|---|---|---|---|---|
| canal 1 | temperatura | 27.2°C | canal 1 | umidade | 95%
|01|67|01 10|01|68|BE|

Payload final: 016701100168BE

## ESP32 - exemplos 

Para rodar os exemplos abaixo, certifique-se que todas as bibliotecas estão corretamente instaladas!

O contexto(payload) utilizada nos exemplos, conterá 2 sensores de temperatura e 2 de umidade.

| parte | canal | tipo | valor | payload |tamanho
|---|---|---|---|---|---|
| 1 | 1 | temp - 67 | 27.2 - 0110 | 01670110 | 4 bytes
| 2 | 2 | temp - 67 | 25.5 - 00FF | 026700FF | 4 bytes
| 3 | 1 | umid - 68 | 95 - BE | 0168BE | 3 bytes
| 4 | 2 | umid - 68 | 87 - AE | 0268AE | 3 bytes

Payload final de 14 bytes: 01670110026700FF0168BE0268AE

Caso o resultado final(depois de chegar na API e fazer o decode) seja um JSON, ele ficará neste formato abaixo:
```json
{
  "relative_humidity_1": 95,
  "relative_humidity_2": 87,
  "temperature_1": 27.2,
  "temperature_2": 25.5
}
```

### Encode

* [arquivo .ino](https://raw.githubusercontent.com/douglaszuqueto/esp32-cayenne-lpp/master/encode/encode.ino)

```c
#include <Arduino.h>
#include <CayenneLPP.h>

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

  delay(5000);
}
```

### Decode

* [arquivo .ino](https://raw.githubusercontent.com/douglaszuqueto/esp32-cayenne-lpp/master/decode/decode.ino)

```c
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
```

### Utilizando WiFi e MQTT para envio

```c
#include <WiFi.h>
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
```

## Considerações finais

Dependendo do projeto o uso desse formato pode se tornar meio limitado devido aos tipos disponiveis, mas caso não tenha essa restrição, o uso é realmente fantástico e certamente positivo.

Atualmente eu acredito que seja muito utilizado em projetos envolvendo LoRaWan, já que a plataforma da TTN(The Things Network) possui esse suporte nativo - ou faz o uso do cayenne ou você irá precisar fazer o encode/decode manualmente - tanto no firmware como também na TTN através do uso de um script JS.

O mais legal dessa história toda, é que você pode utilizar essa técnica em qualquer outro projeto, indiferente do meio de comunicação e meio de transporte.

Você pode pegar estes exemplos e adaptar para utilizar WiFi do ESP32 e efetuar o envio através do protocolo MQTT sem problemas algum - é claro que do outro lado(onde receber - API por exemplo) você vai precisar efetuar o decode da mensagem recebida, para isso dependendo da linguagem utilizada, já se tem bibliotecas prontas para fazer o encode/decode assim como temos pra o ecossistema Arduino.

Por hora era isso, espero que tenham gostado!

**Um grande abraço,**
**Douglas Zuqueto**

## Referências

* [Cayenne Low Power Payload](https://mydevices.com/cayenne/docs/lora/#lora-cayenne-low-power-payload)
* [CayenneLPP library](https://github.com/sabas1080/CayenneLPP)
* [CayenneLPPdec library](https://github.com/gmag11/CayenneLPPdec)