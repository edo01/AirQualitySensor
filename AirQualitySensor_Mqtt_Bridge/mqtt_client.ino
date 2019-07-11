/*********************************************************
*                                                        *
*         ARCES::GROVE AIR QUALITY SENSOR V1.3           *
* - Range of the output:                                 *
*                   - 3 : Fresh Air;                     *
*                   - 2 : Low Pollution;                 *
*                   - 1 : High Pollution;                *
*                   - 0 : High Pollution, Force Signal.  *
*                                                        *
* - Range of the voltage value in output: 0-1023.        *
* - The sensor is connected to LinkIt 7697 which is a    *   
*   mqtt client.                                         *
*   So the LinkIt 7697 send to the broker every T        *
*   seconds the voltage value and the index of pollution.*
*                                                        *
*   For the all documentation of the project please read *                                                   
*   the "README.pdf".                                    *
*                                                        *
*                                          Developed by  *
*                                          Carrà Edoardo *
*                                                        *
*********************************************************/

#include <PubSubClient.h> //mqtt library
#include "Air_Quality_Sensor.h" // air quality sensor grove v1 3
#include <LWiFi.h> // wifi library

int server_port = 1883;
IPAddress server(192,168,77,1); //ip of mqtt broker
//char server[] = "*****"; //if you want to use dns instead ip

char ssid[] = "********";      //your network SSID 
char pass[] = "********";  //your network password 

int status = WL_IDLE_STATUS;

WiFiClient clientWiFi;
PubSubClient client(clientWiFi);

AirQualitySensor sensor(A0); //change here the analogic pin of the air pollution sensor

//Reading message from the broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("AirQualitySensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(57600);
  Serial.println("Waiting sensor to init...");
  delay(2000);
  
  if (sensor.init())Serial.println("Sensor ready.");
  else Serial.println("Sensor ERROR!");
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to wifi");
  
  client.setServer(server, server_port);
  client.setCallback(callback);  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  else{
    //publishing the air quality index
    String valore = String(sensor.slope());
    client.publish("AQI", valore.c_str()); //Air_quality_index
    
    //publishing the voltage value of the sensor
    float x = sensor.getValue()*100;
    float aqs = x/1023;
    String val = String(aqs);    
    client.publish("AQS",val.c_str()); //Air_quality_sensor
    
    delay(60000);
  }
  client.loop();
}
