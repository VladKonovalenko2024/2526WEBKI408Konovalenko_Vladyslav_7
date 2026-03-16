#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>
#include <LittleFS.h>

// ====== Налаштування ======
WebServer webServer(80);
DNSServer dnsSrv;
Preferences prefs;

const byte DNS_LISTEN_PORT = 53;
IPAddress AP_ADDR(192, 168, 1, 1);
const char* AP_NAME = "ESP32_Setup_AP";

// ====== Обробка збереження Wi-Fi ======
void saveWiFiCredentials() {
    String ssidInput = webServer.arg("ssid");
    String passInput = webServer.arg("pass");

    if(ssidInput.length() > 0){
        prefs.begin("wifi_data", false);
        prefs.putString("ssid", ssidInput);
        prefs.putString("pass", passInput);
        prefs.end();

        webServer.send(200, "text/html", "<h2>Дані збережено! Перезавантаження ESP32...</h2>");
        delay(2000);
        ESP.restart();
    }
}

// ====== Налаштування AP та Captive Portal ======
void startAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_ADDR, AP_ADDR, IPAddress(255, 255, 255, 0));
    WiFi.softAP(AP_NAME);

    // DNS для Captive Portal
    dnsSrv.start(DNS_LISTEN_PORT, "*", AP_ADDR);

    // Головна сторінка
    webServer.on("/", HTTP_GET, []() {
        if(LittleFS.exists("/index.html")){
            File file = LittleFS.open("/index.html", "r");
            webServer.streamFile(file, "text/html");
            file.close();
        } else {
            webServer.send(404, "text/plain", "Error: index.html not found in LittleFS!");
        }
    });

    // Обробка POST
    webServer.on("/save", HTTP_POST, saveWiFiCredentials);

    // Редірект для “Not Found” (Captive Portal на мобільних)
    webServer.onNotFound([]() {
        webServer.sendHeader("Location", String("http://") + AP_ADDR.toString(), true);
        webServer.send(302, "text/plain", "");
    });

    webServer.begin();
    Serial.println("Captive Portal запущено на IP: " + AP_ADDR.toString());
}

void setup() {
    Serial.begin(115200);

    // Ініціалізація файлової системи
    if(!LittleFS.begin(true)){
        Serial.println("Помилка LittleFS!");
        return;
    }

    // Зчитування збережених Wi-Fi даних
    prefs.begin("wifi_data", true);
    String storedSSID = prefs.getString("ssid", "");
    String storedPASS = prefs.getString("pass", "");
    prefs.end();

    // Якщо дані є — намагаємося підключитись
    if(storedSSID != ""){
        WiFi.begin(storedSSID.c_str(), storedPASS.c_str());
        Serial.print("Підключення до мережі: "); Serial.println(storedSSID);

        int tries = 0;
        while(WiFi.status() != WL_CONNECTED && tries < 20){
            delay(500);
            Serial.print(".");
            tries++;
        }
    }

    // Якщо підключення не вдалося — запускаємо AP
    if(WiFi.status() != WL_CONNECTED){
        startAccessPoint();
    } else {
        Serial.println("Wi-Fi підключено! IP: " + WiFi.localIP().toString());
    }
}

void loop() {
    // Captive Portal працює тільки в режимі AP
    if(WiFi.getMode() == WIFI_AP){
        dnsSrv.processNextRequest();
    }
    webServer.handleClient();
}


