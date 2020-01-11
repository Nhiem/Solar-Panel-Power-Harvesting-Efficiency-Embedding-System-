#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_INA219.h>


// INA Current sensor 
Adafruit_INA219 ina219;
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  char data[100];  
  String monitoringdata; 



// Add the MQTT broker I
const char* ssid = "Qbesu"; 
const char* password ="lisa850506"; 

// Add the MQTT Broker IP address 
const char* mqtt_server = "192.168.43.180"; 

WiFiClient espClient; 
PubSubClient client(espClient); 
long lastMsg = 0; 
char msg[50]; 
int vaule;



void setup(){
  Serial.begin(115200);

  // INA-219 
  //unit32_t currentFrequency; 
  //wire.begin(); 
  ina219.begin();
  Serial.println("Measuring voltage and current with INA219 ...");

  // Set up the Wifi 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

//  pinMode(ledPin, OUTPUT);
  
  }

void setup_wifi(){
  delay(10); 
  Serial.println(); 
  Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length){

  Serial.print("Message arrived on topic: "); 
  Serial.print(topic);
  Serial.print(". Message: ");
    String messageCurrent;

    // Transmit all the string to the Char 
    for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageCurrent += (char)message[i];
  }
   Serial.println();

  if(String(topic) =="Esp32/solar/data"){
    Serial.print("Change");
    if(messageCurrent =="on"){
    Serial.println("on"); 
   // digital (ledPin, HIGH); 
    }
    else if(messageCurrent== "off"){
      Serial.println("off");
     // digitalWrite(ledPin, LOW);
      }
    } 

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    

// INA 219

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  current_mA= current_mA*(-1)+0.7; 
  //power_mW = ina219.getPower_mW();
  power_mW= busvoltage*current_mA;

  loadvoltage = busvoltage + (shuntvoltage / 1000);
// Convert the value Busvolt to a char to array
  char Busvolstring[8];
    dtostrf(busvoltage, 1, 2, Busvolstring);
    Serial.print("BusVoltage: ");
    Serial.println(Busvolstring);
    client.publish("esp32/Busvol", Busvolstring);

// Convert the value Busvolt to a char to array
  char shuntstring[8];
    dtostrf(shuntvoltage, 1, 2, shuntstring);
    Serial.print("shunVoltage: ");
    Serial.println(shuntstring);
    client.publish("esp32/shunvol", shuntstring);

// Convert the value Busvolt to a char to array
  char Currentstring[8];
    dtostrf(current_mA, 1, 2, Currentstring);
    Serial.print("Current: ");
    Serial.println(Currentstring);
    client.publish("esp32/Current", Currentstring);
// Convert the value Busvolt to a char to array
  char Powerstring[8];
    dtostrf(power_mW, 1, 2, Powerstring);
    Serial.print("Power: ");
    Serial.println(Powerstring);
    client.publish("esp32/Powerharvest", Powerstring);

    // Convert all data into big string 
    monitoringdata= "{ \"Bus_voltage\": \"" + String(Busvolstring) + "\",  \"current_solar\": \"" + String(Currentstring)+ "\", \"power_Harvest\": \"" +String(Powerstring) + "\"}"; 
    monitoringdata.toCharArray(data, (monitoringdata.length()+1)); 


    //publishes temperature and Humidity values 
    client.publish("esp32/monitoringdata", data);
    Serial.print(data);


  }
}
