#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "AlanTAS"
#define WLAN_PASS       "85585885"
int status = WL_IDLE_STATUS;
int led_red = 9;
int led_green = 10;
int led_blue = 11;

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "192.168.0.4"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiEspClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
SoftwareSerial soft(2,3); // RX, TX

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe rgbLED = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "test");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(9600); 
  soft.begin(9600);
  delay(10);
  WiFi.init(&soft);


  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

 // WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (status != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    status = WiFi.begin(WLAN_SSID, WLAN_PASS);
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&rgbLED);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &rgbLED) {
      Serial.print(F("Got: "));
      Serial.println((char *)rgbLED.lastread);
      String oi = (char *)rgbLED.lastread;
      Serial.println(oi);


          int str_len = oi.length() + 1; 
          char payload[str_len];
          oi.toCharArray(payload, str_len);

          if(payload[0] == '#'){
  
        int um = 0;
        int dois = 0;
        int tres = 0;

        for(int i = 0; i < (sizeof(payload)); i++){

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
  Serial.println(um);  
  Serial.println(dois);
  Serial.println(tres);


  

     RGB(ver, azu, verd);


      
    }
  }
  /*
  // Now we can publish stuff!
  Serial.print(F("\nSending photocell val "));
  Serial.print(x);
  Serial.print("...");
  if (! photocell.publish(x++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  */

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void RGB(int r, int g, int b) {

  analogWrite(led_red, r);
  analogWrite(led_green, g);
  analogWrite(led_blue, b);

}
