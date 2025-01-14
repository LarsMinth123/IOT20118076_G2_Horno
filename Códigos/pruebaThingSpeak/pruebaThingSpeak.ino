#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid = "GalaxyA205C7D";
const char* password = "xwcs2389";
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "NY0J6KEBYUIXI9GH";
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked / banned
unsigned long lastTime = 0;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 10000;
void setup() {
  Serial.begin(115200);
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
  // Random seed is a number used to initialize a pseudorandom number generator
  randomSeed(analogRead(33));
}

void loop() {
  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&field1=" + String(random(40));
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      /*
// If you need an HTTP request with a content type: application/json, use the
following:
http.addHeader("Content-Type", "application/json");
// JSON data to send with HTTP POST
String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" +
String(random(40)) + "\"}";
// Send HTTP POST request
int httpResponseCode = http.POST(httpRequestData);*/
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}