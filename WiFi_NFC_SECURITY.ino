#include <ESP8266WiFi.h>

const char* ssid = "BILMARK";
const char* password = "18900533";

int relayPin = D1;
WiFiServer server(80);
IPAddress ip(192, 168, 0, 99);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

void setup() {
  Serial.begin(9600);
  delay(10);
 

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
 
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  server.begin();
  Serial.println("Server started");
 
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/"); 
  Serial.println("NDEF Reader");
  nfc.begin();
}


void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  int value = LOW;
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    tag.print();
    digitalWrite(relayPin, HIGH);
    value = HIGH;
  } 
  else {
    digitalWrite(relayPin, LOW);
    value = LOW;
  }
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Relay is now: ");
 
  if(value == HIGH) {
    client.print("Engaged (ON)");  
    Serial.println("Client working");
  } else {
    client.print("Disengaged (OFF)");
  }
  client.println("<br><br><br>");
  client.println("<a href=\"/relay=ON\">Click here to engage (Turn ON) the relay.</a> <br><br><br>");
  client.println("<a href=\"/relay=OFF\">Click here to disengage (Turn OFF) the relay.</a><br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client waiting...");
  Serial.println("");
 
}
