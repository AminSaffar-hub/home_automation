#include <FS.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

char c;
byte R1state = 1;
byte R2state = 1;
byte R3state = 1;
int Switch1State = 0;
int Switch2State = 0;
int prev1State = 0;
int prev2State = 0;
WiFiServer wifiServer(2000);


void response(WiFiClient client)
{

  String st1 = "0";
  if(!R1state) {
    st1 = "1";
  }
  String st2 = "0";
  if(!R2state) {
    st2 = "1";
  }
  String st3 = "0";
  if(!R3state) {
    st3 = "1";
  }

  String str =  "{\"a\":" + st1 + ",\"b\":" + st2 + ",\"c\":" + st3 + "}";
  
  byte bytes[str.length()];
  str.getBytes(bytes, str.length()+1);
  client.write(bytes, str.length()+1);
  client.flush();
}


void sc()
{
  WiFiClient client = wifiServer.available();


 
  if (client) {
 
    while (client.connected()) {
 
      if (client.available()>0) {
      c = client.read();  
      delay(10);   
      }
      Switch1State = digitalRead(12);
      Switch2State = digitalRead(15);
      if ( c == 's') {
        response(client);
      }
      if ((c == 'a') || (Switch1State != prev1State) )
      {
      R1state = !R1state;
      digitalWrite(0,R1state);
      response(client);
      c='f';
      }
      if ((c == 'b') || (Switch2State != prev2State) )
      {
      R2state = !R2state;
      digitalWrite(2,R2state);
      c='f';
      response(client);
      }
      if ((c == 'c'))
      {
      R3state = !R3state;
      digitalWrite(4,R3state);
      c='f';
      response(client);
      }
      
      prev1State = Switch1State;
      prev2State = Switch2State;
      }
    client.stop();
    Serial.println("Client disconnected");
  }
}
void not_sc()
{
  Switch1State = digitalRead(12);
  Switch2State = digitalRead(15);
  
  if (Switch1State != prev1State)
      {
      R1state = !R1state;
      digitalWrite(0,R1state);
      }
  if (Switch2State != prev2State)
      {
      R2state = !R2state;
      digitalWrite(2,R2state);
      }
  prev1State = Switch1State;
      prev2State = Switch2State;
  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  pinMode(0,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(12,INPUT);
  pinMode(15,INPUT);  
//  /pinMode(6,INPUT);
  //pinMode(8,INPUT);
  digitalWrite(2,R1state);
  digitalWrite(0,R2state);
  digitalWrite(4,R3state);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);

   
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println("Trying to change IP to 200");
  IPAddress ip = WiFi.localIP();
  ip[3] = 200;
  Serial.println("Caculated IP");
  Serial.println(ip);
  Serial.println("gatewayIP IP");
  Serial.println(WiFi.gatewayIP());
  Serial.println("subnetMask");
  Serial.println(WiFi.subnetMask());
  wifiManager.setSTAStaticIPConfig(ip, WiFi.gatewayIP(), WiFi.subnetMask());

  Serial.println("New local ip");
  Serial.println(WiFi.localIP());
  wifiServer.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
not_sc();
sc();
}
