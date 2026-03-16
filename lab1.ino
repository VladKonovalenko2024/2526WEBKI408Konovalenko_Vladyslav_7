#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "TP_LINK2943";
const char* password = "pass123word";


WebServer server(80);

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  
  if (!file) {
    server.send(500, "text/plain", "File not found");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

void setup() {

  Serial.begin(115200);

  // запуск файлової системи
  if(!LittleFS.begin()){
    Serial.println("LittleFS Mount Failed");
    return;
  }

  Serial.println("LittleFS mounted");

  // підключення до WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // обробник запиту /
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}





