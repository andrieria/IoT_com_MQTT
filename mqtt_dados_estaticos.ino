#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Definições dos pinos para o módulo GPS
#define RXPin 16  // Pino RX do módulo GPS (no ESP32, RX2 é o pino 16)
#define TXPin 17  // Pino TX do módulo GPS (no ESP32, TX2 é o pino 17)

// Credenciais Wi-Fi
const char* ssid = "kk";
const char* password = "03172777";

// Configurações do MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883; // Porta padrão do MQTT
const char* mqtt_topic = "veiculo/localizacao";

TinyGPSPlus gps;  // Inicialização do objeto TinyGPS++
HardwareSerial gpsSerial(2); // Inicialização da comunicação serial para o módulo GPS (Serial2 no ESP32)

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando se conectar ao servidor MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);  // Inicialização da comunicação serial com o computador
  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);  // Inicialização da comunicação serial com o módulo GPS

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  

  /*while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());  // Lê os dados disponíveis na porta serial do GPS
  }*/


  

  // Verifica se há novos dados válidos do GPS
  //if (gps.location.isUpdated()) {
    char payload[100];
    /*snprintf(payload, sizeof(payload), "Latitude: %.6f | Longitude: %.6f | Altitude: %.2f meters", 
             gps.location.lat(), gps.location.lng(), gps.altitude.meters());*/
    float latitude = -5.938823;   // Substitua pelo valor inteiro desejado
    float longitude = -35.266754;  // Substitua pelo valor inteiro desejado
    float altitude = 70.70;      // Substitua pelo valor inteiro desejado

    snprintf(payload, sizeof(payload), "Latitude: %.6f | Longitude: %.6f | Altitude: %.2f meters", 
             latitude, longitude, altitude);


    Serial.println(payload);
    client.publish(mqtt_topic, payload);
  //}




  // Se nenhum dado válido foi recebido do GPS, exibe uma mensagem
  /*if (!gps.location.isValid()) {
    Serial.println("Aguardando dados válidos do GPS...");
  }*/





  delay(1000);  // Aguarda 1 segundo antes da próxima leitura
}
