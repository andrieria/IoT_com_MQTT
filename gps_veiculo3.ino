#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define RXPin 16         // Pino RX do módulo GPS (no ESP32, RX2 é o pino 16)
#define TXPin 17        // Pino TX do módulo GPS (no ESP32, TX2 é o pino 17)

TinyGPSPlus gps;             // Inicialização do objeto TinyGPS++
HardwareSerial gpsSerial(2); // Inicialização da comunicação serial para o módulo GPS (Serial2 no ESP32)

void setup() {
  Serial.begin(9600);         // Inicialização da comunicação serial com o computador
  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);  // Inicialização da comunicação serial com o módulo GPS
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());  // Lê os dados disponíveis na porta serial do GPS
  }

  // Verifica se há novos dados válidos do GPS
  if (gps.location.isUpdated()) {
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);  // Imprime a latitude com 6 casas decimais
    Serial.print(" | Longitude: ");
    Serial.print(gps.location.lng(), 6);  // Imprime a longitude com 6 casas decimais
    Serial.print(" | Altitude: ");
    Serial.print(gps.altitude.meters());  // Imprime a altitude em metros
    Serial.println(" meters");
  }

  // Se nenhum dado válido foi recebido do GPS, exibe uma mensagem
  if (!gps.location.isValid()) {
    Serial.println("Aguardando dados válidos do GPS...");
  }

  delay(1000);  // Aguarda 1 segundo antes da próxima leitura
}
