/**
 * Tom Cooper 2021
 */

#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "WEMOS_Motor.h"

#include "index.h"

ESP8266WebServer server(80);          //  Web Server Port 80  | 192.168.1.1:80 <-
WebSocketsServer webSocket(81);       //  WebSocket Port 81   | 192.168.4.1:81 <-

Motor M1(0x30,_MOTOR_A, 1000);  //  Motor A
Motor M2(0x30,_MOTOR_B, 1000);  //  Motor B

void setup() {

  Serial.begin(115200);
  Serial.println(); //clear junk line


  Serial.println(WiFi.softAP("BattleBot_0") ? "Ready" : "Failed!"); //  Start of the Soft Access Point

  //  Redirect for the main page 
  server.on("/", handleRoot);

  //  Redirect for the nippleJS used by the main page
  server.on("/nipplejs.min.js", handleNippleJS);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  //  Debug Prompt
  //Serial.println("WebSocket server started.");

  server.begin();
}

void loop() {

  //  Listen for server events
  webSocket.loop();
  server.handleClient();
}


/** 
  *   WebsocketEvent
  *   WStype_DISCONNECTED - On disconect motor stop with Serial Prompt
  *   WStype_CONNECTED - Serial display connected IP with connected signal. Motor Stoped as well.
  *   WStype_TEXT - Payload converted into String sorted into; S - Stop | R = Right Motor | L = Left Motor
  *                 Dpeending on input either Stop or Motor movement
**/

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {  
    switch(type) {
        case WStype_DISCONNECTED: {
            Serial.printf("[%u] Disconnected!\n", num);
            handleStop();
            break;
        }
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");

            handleStop();
            break;
        }
        case WStype_TEXT: {
            char* str = (char*)payload;
            int left = atoi(str);
            char* fromSpace = strchr(str, ' ');
            if(!fromSpace) break;
            int right = atoi(fromSpace+1);

            Serial.print(left);
            Serial.print(" ");
            Serial.println(right);
            
            M1.setmotor(right>0 ? _CW : _CCW, abs(right));
            M2.setmotor(left >0 ? _CW : _CCW, abs(left));
            break;
        }
    }
}

/**
 * Helper Functions
 */

//  
void handleRoot(){
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

//  
void handleNippleJS(){
  String s = nipplejs;
  server.send(200, "text/html", s);
}

//  Stop all motor movement
void handleStop(){
    M1.setmotor( _STOP );
    M2.setmotor( _STOP );
}
