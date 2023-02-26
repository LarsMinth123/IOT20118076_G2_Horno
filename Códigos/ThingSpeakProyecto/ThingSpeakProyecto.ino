// LIBRERIAS
#include "DHTesp.h"
#include <WiFi.h>
#include <HTTPClient.h>

// CONSTANTES
#define DHT 4   // Se define el pin del sensor de temperatura y humedad
#define MQ9 32  // Se define el pin del sensor de CO
#define POS 5   // Se define el pin del sensor de posición
const char* ssid = "GalaxyA205C7D";
const char* password = "xwcs2389";
const char* serverName = "http://api.thingspeak.com/update";
unsigned long previousMillis = 0;
const long timerDelay = 3000;

// VARIABLES
float temperatura = 0;  // Se inicializa la variable de la temperatura
float humedad = 0;      // Se inicializa la variable de la humedad
bool cerrado = false;   // Se inicializa la variable de la posición de la puerta
int co = 0;             // Se inicializa la variable de la concentración de CO2
// Service API Key
String apiKey = "NHDV0Q2LGNN2YB5T";

// Se inicializa el dht
DHTesp dht;  // Se crea un objeto del tipo DHTesp para manejar el sensor DHT11

void setup() {
  Serial.begin(115200);                                     // Se inicia la comunicación serial
  dht.setup(DHT, DHTesp::DHT11);                            // Se inicializa el sensor de temperatura y humedad
  pinMode(POS, INPUT);                                      // Se configura el pin del sensor de posición como entrada
  Serial.println("Configuracion Inicial Realizada .....");  // Se envía un mensaje por la comunicación serial indicando que se ha terminado la configuración inicial
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  delay(1000);  // Se hace una pausa de un segundo

  // Toma de lecturas
  TempAndHumidity data = dht.getTempAndHumidity();  // Se lee la temperatura y humedad del sensor DHT11
  temperatura = data.temperature;                   // Se actualiza la variable de la temperatura
  humedad = data.humidity;                          // Se actualiza la variable de la humedad
  cerrado = digitalRead(POS);                       // Se lee la posición de la puerta
  co = map(analogRead(MQ9), 0, 4095, 0, 100);       // Se lee la concentración de CO2 y se mapea a un rango de 0 a 100

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timerDelay) {
    previousMillis = currentMillis;
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&field1=" + String(temperatura) + "&field2=" + String(humedad) + "&field3=" + String(cerrado) + "&field4=" + String(co);
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
  }


  //Salida Serial
  Serial.print("Temperatura: ");  // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la temperatura
  Serial.println(temperatura);    // Se imprime la temperatura por la comunicación serial
  Serial.print("Humedad: ");      // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la humedad
  Serial.println(humedad);        // Se imprime la humedad por la comunicación serial
  Serial.print("Posicion: ");     // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la posición de la puerta
  Serial.println(cerrado);        // Se imprime la posición de la puerta por la comunicación serial
  Serial.print("CO: ");           // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la concentración de CO2
  Serial.println(co);             // Se imprime la concentración de CO2 por la comunicación serial
}
