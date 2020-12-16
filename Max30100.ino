#include <stdint.h>
#include <Wire.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "MAX30100_PulseOximeter.h"
#include <WiFi.h>

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "leuz1n"
#define AIO_KEY  "aio_afPR192Exnwju3AZgrDttFcHW4BB"
WiFiClient client;

#define I2C_SDA   21
#define I2C_SCL   22

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

boolean MQTT_connect();

boolean MQTT_connect() {  int8_t ret; if (mqtt.connected()) {    return true; }  uint8_t retries = 3;  while ((ret = mqtt.connect()) != 0) { mqtt.disconnect(); delay(2000);  retries--;if (retries == 0) { return false; }} return true;}

// aqui altera o feed e a variavel do ada
Adafruit_MQTT_Publish   mydata = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ mydata");
Adafruit_MQTT_Publish   batimentos = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ Batimentos");
Adafruit_MQTT_Publish   oxigenacao = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ Oxigenacao");
Adafruit_MQTT_Publish   batEox = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ Bat e Ox");
Adafruit_MQTT_Publish   MaxBat = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ BatMax");
Adafruit_MQTT_Publish   MinBat = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ BatMin");
Adafruit_MQTT_Publish   MinOx = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ OxMin");


PulseOximeter pox;

int batVar, oxiVar, BatMax = 120, BatMin = 60, OxMin = 90;


void onBeatDetected
{
  Serial.println("Beat!");
}

void dadosMAX30100
{
  while(1){
      {
    // sinais injetados para testar o alerta    
    if(cont == 3)
    {
       batVar = 60;
       oxiVar = 88;
    }
    else if(cont == 4)
    {
       batVar = 100;
       oxiVar = 95;
    }
    else if (cont == 5)
    {
      batVar = 129;
      oxiVar = 100;
    }  
    else
    {
       pox.update();                             // update no sensor
      batVar = pox.getHeartRate();              // recebe os batimentos
      oxiVar = pox.getSpO2();
    }
      batimentos.publish(batVar);               // manda pro adafruit batimentos
      Serial.println("Batimentos: ");
      Serial.println(batVar);
      oxigenacao.publish(oxiVar);               // manda pro adafruit oxigenação
      Serial.println("Oxigenacao: ");
      Serial.println(pox.getSpO2());
      batEox.publish(batVar);                   // printa no display do inicio os batimentos
      batEox.publish(oxiVar);                  // printa no display do inicio a oxigenação
       }
               
  }

void setup()
{
                Serial.begin(115200);
                Wire.begin(I2C_SDA, I2C_SCL);
                Serial.println("START");
                //WiFi.begin("fluipress","fluipress@2017!");
                WiFi.begin("NET_AP801_2g","meneghatti");
                while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.println("Tentando conectar");
                }
                Serial.println();
                Serial.println("WiFi connected");
                Serial.println("IP address: "); Serial.println(WiFi.localIP());
                mqtt.subscribe(&sw_sub);
                
                MaxBat.publish(BatMax);
                MinBat.publish(BatMin);
                MinOx.publish(OxMin);
                
                Serial.print("Initializing pulse oximeter..");
                if (!pox.begin()) {
                Serial.println("FAILED");
                for(;;);
                } else {
                Serial.println("SUCCESS");
                }
                
                // Register a callback for the beat detection
                pox.setOnBeatDetectedCallback(onBeatDetected);        // libera pra começar a receber dados
                stopReadPOX();
}
void loop() {
  MQTT_connect();

  if (MQTT_connect()) {
      
      dadosMAX30100;
    } else {
      Serial.println("Problem to connect");

    }
    delay(3000); 
 
        Serial.println(millis());
    }

}
