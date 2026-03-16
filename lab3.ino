#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "TP_LINK2943";
const char* password = "pass123word";

WebServer server(80);

// ===== Basic Auth =====
const String AUTH_KEY = "Basic YWRtaW46MTIzNDU=";

void handleRoot() {
  if(server.hasHeader("Authorization")) {
    if(server.header("Authorization") != AUTH_KEY){
      server.sendHeader("WWW-Authenticate", "Basic realm=\"ESP32\"");
      server.send(401, "text/plain", "Не авторизовано!");
      return;
    }
  } else {
    server.sendHeader("WWW-Authenticate", "Basic realm=\"ESP32\"");
    server.send(401, "text/plain", "Не авторизовано!");
    return;
  }

  // Віддаємо index.html з LittleFS
  if(LittleFS.exists("/index.html")){
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "Файл не знайдено!");
  }
}

void setup() {
  Serial.begin(115200);

  // Ініціалізація LittleFS
  if(!LittleFS.begin(true)){
    Serial.println("Помилка LittleFS");
    return;
  }

  // Підключення до Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Підключення до Wi-Fi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nПідключено! IP: " + WiFi.localIP().toString());

  // Роут для головної сторінки
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Сервер запущено на порті 80");
}

void loop() {
  server.handleClient();
}