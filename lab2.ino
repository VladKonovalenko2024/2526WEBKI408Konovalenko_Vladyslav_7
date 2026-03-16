#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "miron_shtyrm";
const char* password = "miron2019";

WebServer server(80);

const int ledPin = 2;
bool ledState = false;

void handleRoot() {

  File file = LittleFS.open("/index.html", "r");

  if(!file){
    server.send(500,"text/plain","File error");
    return;
  }

  server.streamFile(file,"text/html");
  file.close();
}

void handleStatus(){

  String json = "{\"led_on\":";

  if(ledState)
    json += "true}";
  else
    json += "false}";

  server.send(200,"application/json",json);
}

void handleControl(){

  ledState = !ledState;

  digitalWrite(ledPin, ledState);

  server.send(200,"application/json","{\"result\":\"ok\"}");
}

void setup(){

  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  if(!LittleFS.begin()){
    Serial.println("LittleFS error");
    return;
  }

  WiFi.begin(ssid,password);

  Serial.print("Connecting");

  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/control", HTTP_POST, handleControl);

  server.begin();
}

void loop(){
  server.handleClient();
}