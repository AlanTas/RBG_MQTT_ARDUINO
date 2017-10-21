#include <PubSubClient.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
//#include <WiFiEspServer.h>
#include "SoftwareSerial.h"
#include <WiFiEspUdp.h>

char ssid[] = "AlanTAS";            
char pass[] = "85585885";       
int status = WL_IDLE_STATUS;
int led_red = 9;
int led_green = 10;
int led_blue = 11;


IPAddress server(192, 168, 0, 4);


// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(2,3); // RX, TX
void setup() {

  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  //Serial.println("You're connected to the network");

  //connect to MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
 
  //String strPayload = String((char*)payload);

    if(payload[0] == '#'){
  
        int um = 0;
        int dois = 0;
        int tres = 0;

        for(int i = 0; i < (length); i++){

         if(payload[i] == '*'){
             if(um == 0){

              um = i;
             }

             else if(dois == 0){
              dois = i;
             }

             else if(tres == 0){
              tres = i;
             }    
         }
          
        }

         String red;
         for(int i = 3; i<um; i++){
          red += String((char)payload[i]);
         }
         int ver = red.toInt();
         Serial.print("Vermelho: ");
         Serial.println(ver);

         String blu;
         for(int j = (um + 3); j<dois; j++){
          blu += String((char)payload[j]);
         }
         int azu = blu.toInt();
         Serial.print("Azul: ");
         Serial.println(azu);

         String gre;
         for(int k = (dois + 3); k<tres; k++){
          gre += String((char)payload[k]);
         }
         int verd = gre.toInt();
         Serial.print("Verde: ");
         Serial.println(verd);          
  // Serial.println(um);  
  //Serial.println(dois);
  //Serial.println(tres);


  

 RGB(ver, azu, verd);
      
    }
     
     
     
     /*
     
     if(payload[0] == '1'){
       digitalWrite(led, HIGH);
       }

     else if (payload[0] == '0'){
       digitalWrite(led, LOW);
       } */

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
  delay(1000);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("command","hello world");
      // ... and resubscribe
      client.subscribe("test");
    } else {
        Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void RGB(int r, int g, int b) {

  analogWrite(led_red, r);
  analogWrite(led_green, g);
  analogWrite(led_blue, b);

}
