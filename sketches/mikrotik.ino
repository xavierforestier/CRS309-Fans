#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const int FAN0 = A1;
const int FAN1 = A3;
const char *ssid = "";
const char *password = "";

int fanSpeed0 = 255;
int fanSpeed1 = 255;
uint32_t PWN_Frequency = 10000;
int PWN_Resolution = 8;

WebServer server(80);

void handleRoot() {
  char rawhtml[1024];
  const int max = (1 << PWN_Resolution)-1;
  sprintf(rawhtml, "<html>"
                     "<head>"
                       "<title>Mikrotik fans</title>"
                      "</head>"
                      "<body onload=\"document.querySelectorAll('input').forEach(async input => { input.onchange = async event => { await fetch(`/api/${input.id}`,{method:'PUT',body:input.value})}});"
                                     "setInterval( () => { document.querySelectorAll('input').forEach( input => { fetch(`/api/${input.id}`).then(f => {f.text().then(t => { input.value = t; });}); }); }, 30000)\">"
                        "<label for=\"fans/0/speed\">Fan 1 speed&nbsp;:</label><input type=\"number\" id=\"fans/0/speed\" min=\"0\" max=\"%d\" /><br>"
                        "<label for=\"fans/1/speed\">Fan 2 speed&nbsp;:</label><input type=\"number\" id=\"fans/1/speed\" min=\"0\" max=\"%d\" /><br>"
                        "<label for=\"config/PWN_Frequency\">PWN frequency&nbsp;:</label><input type=\"number\" id=\"config/PWN_Frequency\"> Hz<br>"
                        "<label for=\"config/PWN_Resolution\">PWN resolution&nbsp;:</label><input type=\"number\" id=\"config/PWN_Resolution\" min=\"1\" max=\"20\"> bits"
                      "<body>"
                    "</html>", max, max);
  digitalWrite(LED_BUILTIN, LOW);
  server.send(200, "text/html", rawhtml);
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
  handleGetInteger(0);
}
void handleFanId1() {
  handleGetInteger(1);
}
void handleFanSpeed0() {
  handleGetInteger(fanSpeed0);
}
void handleFanSpeed1() {
  handleGetInteger(fanSpeed1);
}
void handleGetInteger(int id) {
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
  analogWrite(FAN0, fanSpeed0);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFanSpeedChange1() {
  digitalWrite(LED_BUILTIN, LOW);
  fanSpeed1 = server.arg("plain").toInt();
  server.send(204);
  analogWrite(FAN1, fanSpeed1);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleConfig() {
  char json[50];
  digitalWrite(LED_BUILTIN, LOW);
  sprintf(json, "{\"PWN_Frequency\":%d,\"PWN_Resolution\":%d}", PWN_Frequency, PWN_Resolution);
  server.send(200, "application/json", json);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleFrequency() {
  handleGetInteger(PWN_Frequency);
}
void handleFrequencyUpdate() {
  digitalWrite(LED_BUILTIN, LOW);
  PWN_Frequency = server.arg("plain").toInt();
  server.send(204);
  ledcChangeFrequency(FAN0, PWN_Frequency, PWN_Resolution);
  ledcChangeFrequency(FAN1, PWN_Frequency, PWN_Resolution);
  digitalWrite(LED_BUILTIN, HIGH);
}
void handleResolution() {
  handleGetInteger(PWN_Resolution);
}
void handleResolutionUpdate() {
  digitalWrite(LED_BUILTIN, LOW);
  PWN_Resolution = server.arg("plain").toInt();
  server.send(204);
  ledcChangeFrequency(FAN0, PWN_Frequency, PWN_Resolution);
  ledcChangeFrequency(FAN1, PWN_Frequency, PWN_Resolution);
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
  int led = HIGH;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FAN0, OUTPUT);
  pinMode(FAN1, OUTPUT);
  ledcAttach(FAN0, PWN_Frequency, PWN_Resolution);
  ledcAttach(FAN1, PWN_Frequency, PWN_Resolution);
  digitalWrite(LED_BUILTIN, LOW);  
  analogWrite(FAN0, fanSpeed0);
  analogWrite(FAN1, fanSpeed1);

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
  server.on("/api/fans", handleFans);
  server.on("/api/fans/0", handleFan0);
  server.on("/api/fans/1", handleFan1);
  server.on("/api/fans/0/id", handleFanId0);
  server.on("/api/fans/1/id", handleFanId1);
  server.on("/api/fans/0/speed", HTTP_GET, handleFanSpeed0);
  server.on("/api/fans/1/speed", HTTP_GET, handleFanSpeed1);
  server.on("/api/fans/0/speed", HTTP_PUT, handleFanSpeedChange0);
  server.on("/api/fans/1/speed", HTTP_PUT, handleFanSpeedChange1);
  server.on("/api/config", handleConfig);
  server.on("/api/config/PWN_Frequency", HTTP_GET, handleFrequency);
  server.on("/api/config/PWN_Frequency", HTTP_PUT, handleFrequencyUpdate);
  server.on("/api/config/PWN_Resolution", HTTP_GET, handleResolution);
  server.on("/api/config/PWN_Resolution", HTTP_PUT, handleResolutionUpdate);
  server.onNotFound(handleNotFound);
  server.begin();
}

// the loop routine runs over and over again forever:
void loop() {
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}
