#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ===== Wi-Fi =====
const char* WIFI_SSID = "miron_shtyrm";
const char* WIFI_PASS = "miron2019";

// ===== Сервер =====
WebServer server(443); 

// ===== Basic Auth =====
// admin:12345 -> YWRtaW46MTIzNDU=
const String AUTH_KEY = "Basic YWRtaW46MTIzNDU=";

// ===== HTML =====
String getHtmlPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
<meta charset="UTF-8">
<title>Адмін-панель - Коноваленко В.Р.</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family: 'Segoe UI', sans-serif; background: #e6f9e6; color: #033d03; text-align: center; padding: 50px; }
.panel { background: #d4f0d4; max-width: 550px; margin: auto; padding: 35px; border-radius: 15px; border: 2px solid #4caf50; box-shadow: 0 0 25px rgba(76, 175, 80, 0.25); }
h1 { color: #2e8b2e; }
.status { display: inline-block; padding: 6px 18px; background: #4caf50; color: #fff; border-radius: 20px; margin-bottom: 20px; }
p { line-height: 1.6; }
.note { color: #1a5d1a; font-size: 0.9em; margin-top: 20px; border-top: 1px solid #8bc34a; padding-top: 10px; }
</style>
</head>
<body>
<div class='panel'>
<div class='status'>З’єднання активне (HTTPS на порті 443)</div>
<h1>Адмін-панель</h1>
<p>Вітаємо, <strong>Коноваленко В.Р.</strong>!</p>
<p>Це демонстраційний сервер, HTTPS порт 443.</p>
</div>
</body>
</html>
)rawliteral";
}

// ===== Обробник головної сторінки =====
void handleRoot() {
  if (server.hasHeader("Authorization")) {
    String auth = server.header("Authorization");
    if (auth == AUTH_KEY) {
      server.send(200, "text/html", getHtmlPage());
      return;
    }
  }
  server.sendHeader("WWW-Authenticate", "Basic realm=\"ESP32\"");
  server.send(401, "text/plain", "Авторизація не пройдена!");
}

void setup() {
  Serial.begin(115200);

  // ===== Підключення до Wi-Fi =====
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Підключення до Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  // ===== Налаштування маршруту =====
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTPS сервер запущено на порті 443");
}

void loop() {
  server.handleClient();
}