// LIBRERIAS
#include "DHTesp.h"
#include <AzureIoTHubMQTTClient.h>
#include <AzureIoTUtility.h>
#include <WiFiNINA.h>

// CONSTANTES
#define DHT 4   // Se define el pin del sensor de temperatura y humedad
#define MQ9 15  // Se define el pin del sensor de CO
#define POS 5   // Se define el pin del sensor de posición

// VARIABLES
float temperatura = 0;  // Se inicializa la variable de la temperatura
float humedad = 0;      // Se inicializa la variable de la humedad
bool cerrado = false;   // Se inicializa la variable de la posición de la puerta
int co = 0;             // Se inicializa la variable de la concentración de CO

// Se inicializa el dht
DHTesp dht;  // Se crea un objeto del tipo DHTesp para manejar el sensor DHT11

// Replace with your WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Replace with your IoT Hub connection string
const char* connectionString = "HostName=your_IoTHub.azure-devices.net;DeviceId=your_DeviceId;SharedAccessKey=your_SAS_key";

// Define the MQTT client
AzureIoTHubMQTTClient mqttClient;

void setup() {
  Serial.begin(115200);                                     // Se inicia la comunicación serial
  dht.setup(DHT, DHTesp::DHT11);                            // Se inicializa el sensor de temperatura y humedad
  pinMode(POS, INPUT);                                      // Se configura el pin del sensor de posición como entrada

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to Azure IoT Hub
  mqttClient.begin(connectionString, WiFi);

  // Start the MQTT client
  mqttClient.start();

  // Wait for the MQTT client to connect to Azure IoT Hub
  while (!mqttClient.connected()) {
    delay(1000);
    Serial.println("Connecting to Azure IoT Hub...");
  }
  Serial.println("Connected to Azure IoT Hub");
}

void loop() {
  delay(1000);  // Se hace una pausa de un segundo

  // Toma de lecturas
  TempAndHumidity data = dht.getTempAndHumidity();  // Se lee la temperatura y humedad del sensor DHT11
  temperatura = data.temperature;                   // Se actualiza la variable de la temperatura
  humedad = data.humidity;                          // Se actualiza la variable de la humedad
  cerrado = digitalRead(POS);                       // Se lee la posición de la puerta
  co = map(analogRead(MQ9), 0, 4095, 0, 100);      // Se lee la concentración de CO2 y se mapea a un rango de 0 a 100

  //Salida Serial
  Serial.print("Temperatura: ");  // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la temperatura
  Serial.println(temperatura);    // Se imprime la temperatura por la comunicación serial
  Serial.print("Humedad: ");      // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la humedad
  Serial.println(humedad);        // Se imprime la humedad por la comunicación serial
  Serial.print("Posicion: ");     // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la posición de la puerta
  Serial.println(cerrado);        // Se imprime la posición de la puerta por la comunicación serial
  Serial.print("CO: ");           // Se envía un mensaje por la comunicación serial indicando que se va a imprimir la concentración de CO2
  Serial.println(co);             // Se imprime la concentración de CO2 por la comunicación serial

  //Envío de datos a Azure
  if(WiFi.status() == WL_CONNECTED) {
    // Se crea un objeto JSON para los datos a enviar
    StaticJsonDocument<200> doc;
    doc["temperatura"] = temperatura;
    doc["humedad"] = humedad;
    doc["posicion"] = cerrado;
    doc["co2"] = co;
    
    // Se convierte el objeto JSON a una cadena
    String jsonString;
    serializeJson(doc, jsonString);

    // Se establece la conexión con Azure y se envían los datos
    HTTPClient http;
    http.begin(client, azureEndpoint);  // Se inicia la conexión con Azure
    http.addHeader("Content-Type", "application/json");  // Se establece el tipo de contenido a JSON
    http.addHeader("Authorization", "Bearer " + azureToken);  // Se agrega el token de autenticación
    int httpResponseCode = http.POST(jsonString);  // Se envían los datos
    http.end();  // Se cierra la conexión con Azure

    // Se imprime el código de respuesta de la solicitud HTTP
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  else {
    // Si no se ha establecido conexión Wi-Fi, se imprime un mensaje por la comunicación serial
    Serial.println("No se ha establecido conexion Wi-Fi");
  }
}
