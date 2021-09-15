#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "WEMOS_Motor.h"

#include "index.h"

  ESP8266WebServer server(80);          //Web Server Port 80

  WebSocketsServer webSocket(81);       //WebSocket Port 81

  Motor M1(0x30,_MOTOR_A, 1000);  //Motor A
  Motor M2(0x30,_MOTOR_B, 1000);  //Motor B
  
void setup() {
  
  Serial.begin(115200);
  
  //WiFi Access Point Open to all users
  Serial.println(WiFi.softAP("BattleBot_0") ? "Ready" : "Failed!");

  //IP Address Output
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/nipplejs.min.js", handleNippleJS); 
  server.on("/move", handleMove);
  server.on("/move", handleStop);
  
  server.begin();

  Serial.println("Left Right");
}

void handleRoot(){
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void handleNippleJS(){
  String s = nipplejs;
  server.send(200, "text/html", s);
}

void handleMove(){
  int left = server.arg("l").toInt();
  int right = server.arg("r").toInt();

 Serial.print(left);
 Serial.print(" ");
 Serial.println(right);

 M1.setmotor( left>0 ? _CW : _CCW, abs(left));
 M2.setmotor(right>0 ? _CW : _CCW, abs(right));
}

void handleStop(){
    M1.setmotor( _STOP );
    M2.setmotor( _STOP );
  }

void loop() {
  webSocket.loop();
  
  server.handleClient();
}
