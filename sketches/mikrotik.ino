#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
const int FAN0 = 1;
const int FAN1 = 3;
const char *ssid = "";
const char *password = "";

const int tryDelay = 500;
int numberOfTries = 60;

int wifi_status = 0;
int prevFanSpeed0 = 255;
int prevFanSpeed1 = 255;
int fanSpeed0 = 255;
int fanSpeed1 = 255;

WebServer server(80);

void handleRoot() {
  digitalWrite(LED_BUILTIN, LOW);
  server.send(200, "text/html", "<html><head><title>Mikrotik fans</title></head><body onload=\"document.querySelectorAll('input').forEach(async input => { input.onchange = async event => { await fetch(`/fans/${input.id.substr(3)}/speed`,{method:'PUT',body:input.value})}});setInterval( () => { ['0','1'].forEach( i => { fetch(`/fans/${i}/speed`).then(f => {f.text().then(t => { document.querySelector(`input#fan${i}`).value = t; });}); }); }, 30000)\"><label for=\"fan0\">Fan 1 speed&nbsp;:</label><input type=\"number\" id=\"fan0\" min=\"0\" max=\"255\" /><br><label for=\"fan1\">Fan 2 speed&nbsp;:</label><input type=\"number\" id=\"fan1\" min=\"0\" max=\"255\" /><body></html>");
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFavicon() { 
  server.send(204);
}
void handleFans() {
  char json[50];
  digitalWrite(LED_BUILTIN, LOW);
  sprintf(json, "[{\"id\":0,\"speed\":%d},{\"id\":1,\"speed\":%d}]", fanSpeed0, fanSpeed1);
  server.send(200, "application/json", json);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFan0() {
  handleFanNo(0,fanSpeed0);
}
void handleFan1() {
  handleFanNo(1,fanSpeed1);
}
void handleFanNo(int id,int speed) {
  char json[21];
  digitalWrite(LED_BUILTIN, LOW);
  sprintf(json, "{\"id\":%d,\"speed\":%d}", id, speed);
  server.send(200, "application/json", json);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFanId0() {
  handleFanIdNo(0);
}
void handleFanId1() {
  handleFanIdNo(1);
}
void handleFanSpeed0() {
  handleFanIdNo(fanSpeed0);
}
void handleFanSpeed1() {
  handleFanIdNo(fanSpeed1);
}
void handleFanIdNo(int id) {
  char json[3];
  digitalWrite(LED_BUILTIN, LOW);
  sprintf(json, "%d", id);
  server.send(200, "application/json", json);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFanSpeedChange0() {
  digitalWrite(LED_BUILTIN, LOW);
  fanSpeed0 = server.arg("plain").toInt();
  server.send(204);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFanSpeedChange1() {
  digitalWrite(LED_BUILTIN, LOW);
  fanSpeed1 = server.arg("plain").toInt();
  server.send(204);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleNotFound() {
  digitalWrite(LED_BUILTIN, LOW);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (int i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, HIGH);
}

// the setup routine runs once when you press reset:
void setup() {
  int led = LOW;
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FAN0, OUTPUT);
  pinMode(FAN1, OUTPUT);
  analogWrite(FAN0, fanSpeed0);
  analogWrite(FAN0, fanSpeed1);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    if( led == LOW ) {
      led = HIGH;
    }
    else {
      led = LOW;
    }
    digitalWrite(LED_BUILTIN, led);
    delay(250);
  }
  digitalWrite(LED_BUILTIN, LOW);

  server.on("/", handleRoot);
  server.on("/favicon.ico", handleFavicon);
  server.on("/fans", handleFans);
  server.on("/fans/0", handleFan0);
  server.on("/fans/1", handleFan1);
  server.on("/fans/0/id", handleFanId0);
  server.on("/fans/1/id", handleFanId1);
  server.on("/fans/0/speed", HTTP_GET, handleFanSpeed0);
  server.on("/fans/1/speed", HTTP_GET, handleFanSpeed1);
  server.on("/fans/0/speed", HTTP_PUT, handleFanSpeedChange0);
  server.on("/fans/1/speed", HTTP_PUT, handleFanSpeedChange1);
  server.onNotFound(handleNotFound);
  server.begin();
}

// the loop routine runs over and over again forever:
void loop() {
  server.handleClient();
  if(prevFanSpeed0 != fanSpeed0) {
    analogWrite(FAN0, fanSpeed0);
    prevFanSpeed0 = fanSpeed0;
  }
  if(prevFanSpeed1 != fanSpeed1) {
    analogWrite(FAN0, fanSpeed1);
    prevFanSpeed1 = fanSpeed1;
  }
  delay(2);  //allow the cpu to switch to other tasks
}
