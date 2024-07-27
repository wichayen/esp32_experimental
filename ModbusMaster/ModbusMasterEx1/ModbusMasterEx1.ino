#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include "ModbusMaster.h"


//#define DHT_EN
#define OLED_EN

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//#define RELAY1_PIN  12
//#define RELAY2_PIN  13
#define DHTPIN 32 // DHT Sensor connected to D32.
#define DHTTYPE DHT11 // Type of DHT sensor.

#define LED_BUILTIN 2

#define STATUS_LED_ON 1
#define STATUS_LED_OFF 0


DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

// Setup WiFi SSID and Password
const char* ssid = "xxx";
const char* password = "xxx";
// Setup HiveMQ MQTT broker. Access dashboard via: broker.mqtt-dashboard.com
//const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "broker.mqtt-dashboard.com";
//const char* mqtt_server = "broker.emqx.io";
#define mqtt_port 1883
#define MQTT_NAME     "xxx"
//#define MQTT_USERNAME "xxx"
//#define MQTT_PASSWORD "xxx"


// Below are alternatives for MQTT brokers. Uncomment to test them out.
/*
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_server = "iot.eclipse.org";
*/

WiFiClient espClient;
PubSubClient client(espClient);


unsigned long time_1 = 0;
unsigned long time_2 = 0;
unsigned char led_status = STATUS_LED_OFF;


//==================================================================
// Modbus

#define SerialRS485_RX_PIN    16
#define SerialRS485_TX_PIN    15
#define SerialRS485  Serial2                    
#define RS485_DIRECTION_PIN   4   
#define RS485_RXD_SELECT      LOW
#define RS485_TXD_SELECT      HIGH

bool relay1 = false;
bool relay2 = true;
bool relay3 = false;
bool relay4 = true;

uint8_t result;

ModbusMaster node1;    

void preTransmission()
{
  digitalWrite(RS485_DIRECTION_PIN, RS485_TXD_SELECT);
}

void postTransmission()
{
  digitalWrite(RS485_DIRECTION_PIN, RS485_RXD_SELECT);
}

//==================================================================


// The function below connects your NodeMCU to the wireless network
void setup_wifi(){
	Serial.begin(115200);
	Serial.println();
	Serial.print("connecting to ");
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
	Serial.println("Experience the magic of MQTT");
	delay(2000);
}



// The setup function sets your NodeMCU GPIOs attached to LED pins to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup(){
	digitalWrite(LED_BUILTIN, LOW);
	//digitalWrite(RELAY1_PIN, LOW);
	//digitalWrite(RELAY2_PIN, LOW);

	pinMode(LED_BUILTIN, OUTPUT);
	//pinMode(RELAY1_PIN, OUTPUT);
	//pinMode(RELAY2_PIN, OUTPUT);

	Serial.begin(115200); // Starts the serial communication


#ifdef DHT_EN
  dht.begin();
#endif
	
#ifdef OLED_EN
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println(F("SSD1306 allocation failed"));
		for(;;);
	}
	delay(1000);
	display.clearDisplay();
	display.setTextColor(WHITE);

	// display 
	display.setTextSize(1);
	display.setCursor(0,30);
	display.print("Wifi connecting ...");
	display.display();
#endif

	setup_wifi();
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);

#ifdef OLED_EN
  digitalWrite(LED_BUILTIN, HIGH);
  display.clearDisplay(); 
  display.setCursor(0,30);
  display.print("WiFi connected !!!");
  display.display();
#endif
	
	pinMode(RS485_DIRECTION_PIN, OUTPUT);                                                          // RS485 Direction
	digitalWrite(RS485_DIRECTION_PIN, RS485_RXD_SELECT);

	SerialRS485.begin(9600, SERIAL_8N1, SerialRS485_RX_PIN, SerialRS485_TX_PIN);
	while(!SerialRS485);

	node1.begin(10, SerialRS485);    // Modbus slave ID 10
	node1.preTransmission(preTransmission); //Callback for configuring RS-485 Transreceiver correctly
	node1.postTransmission(postTransmission);
}



// This functions is executed when some device publishes a message to a topic that your NodeMCU is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your NodeMCU is subscribed you can actually do something
void callback(char* topic, byte* payload, unsigned int length) {
  char msg[16];
  
	Serial.print("Message received [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
    msg[i] = payload[i];
  }
  Serial.println();
  
  if(String(topic) == "myRobloxHome/led1"){
    if(!strcmp(msg, "on")){
      led_status = STATUS_LED_ON;
      digitalWrite(LED_BUILTIN, LOW);
    }
    if(!strcmp(msg, "off")){
      led_status = STATUS_LED_OFF;
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  if(String(topic) == "myRobloxHome/relay1"){
    if(!strcmp(msg, "on")){
      //digitalWrite(RELAY1_PIN, HIGH);
    }
    if(!strcmp(msg, "off")){
      //digitalWrite(RELAY1_PIN, LOW);
      }
  }

  if(String(topic) == "myRobloxHome/relay2"){
    if(!strcmp(msg, "on")){
      //digitalWrite(RELAY2_PIN, HIGH);
    }
    if(!strcmp(msg, "off")){
      //digitalWrite(RELAY2_PIN, LOW);
      }
  }
  
}



// This functions reconnects your NodeMCU to your MQTT broker
// Change the function below if you want to subscribe to more topics with your NodeMCU 
void reconnect() {
  
	if(led_status == STATUS_LED_ON){
		digitalWrite(LED_BUILTIN, LOW);
	}

	if(led_status == STATUS_LED_ON){
		digitalWrite(LED_BUILTIN, HIGH);
	}
 
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		//if (client.connect("ESP8266Client")) {
		//if (client.connect(MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
		if (client.connect(MQTT_NAME)) {
			Serial.println("connected");

			// You can subscribe to more topics (to control more LEDs in this example)
			client.subscribe("myRobloxHome/led1");
      client.subscribe("myRobloxHome/relay1");
      client.subscribe("myRobloxHome/relay2");
		} 
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}



float humidity = 0;
float temp = 0;
float humidity_old = 0;
float temp_old = 0;

void loop() {

	  if (millis() - time_1 > 1000) {
  		time_1 = millis();
      //Serial.print(time_1);
      //Serial.print("\r\n");
#ifdef DHT_EN
  		// Obtain temperature sensor readings
  		humidity = dht.readHumidity();
  		//Read temperature as Celsius (the default)
  		temp = dht.readTemperature();
  
      // Publish Temperature and Humidity values to MQTT Topic
      // To view the published values, use MQTTBox and subscribe to the topics after setting up the broker credentials

      Serial.print("humidity_old : ");  Serial.print(humidity_old); Serial.print("\r\n");
      Serial.print("humidity : ");      Serial.print(humidity);     Serial.print("\r\n");
      Serial.print("temp_old : ");      Serial.print(temp_old);     Serial.print("\r\n");
      Serial.print("temp : ");          Serial.print(temp);         Serial.print("\r\n");
      
      if( humidity_old != humidity || temp_old != temp){
        client.publish("myRobloxHome/temperature", (char *)(String(temp)).c_str());
        client.publish("myRobloxHome/humidity", (char *)(String(humidity)).c_str());
      }
      humidity_old = humidity;
      temp_old = temp;
#endif
      
  		// Check if any reads failed and exit early (to try again).
  		if (isnan(humidity) || isnan(temp)) {
  			Serial.println("Failed to read from DHT sensor!");
  		  return;
  		}
  
#ifdef OLED_EN
  		Serial.print("Humidity: ");
  		Serial.print(humidity);
  		Serial.print(" %\t Temperature: ");
  		Serial.print(temp);
  		Serial.print("\r\n");
  
  		// clear display
  		display.clearDisplay();
  
  		// display temperature
  		display.setTextSize(1);
  		display.setCursor(0,0);
  		display.print("Temperature: ");
  		display.setTextSize(2);
  		display.setCursor(0,10);
  		display.print(temp);
  		display.print(" ");
  		display.setTextSize(1);
  		display.cp437(true);
  		display.write(167);
  		display.setTextSize(2);
  		display.print("C");
  
  		// display humidity
  		display.setTextSize(1);
  		display.setCursor(0, 35);
  		display.print("Humidity: ");
  		display.setTextSize(2);
  		display.setCursor(0, 45);
  		display.print(humidity);
  		display.print(" %"); 
  
  		display.display();
#endif
		
		
		Serial.println("Start control");
    
    node1.writeSingleCoil(0, relay1);   //  register number : 00001
    node1.writeSingleCoil(1, relay2);
    node1.writeSingleCoil(2, relay3);
    node1.writeSingleCoil(3, relay4);
    relay1 = !relay1;
    relay2 = !relay2;
    relay3 = !relay3;
    relay4 = !relay4;
    
    result = node1.readCoils(0, 10);
    if(result == node1.ku8MBSuccess){
      Serial.print("Coil = ");
      Serial.print((node1.getResponseBuffer(0)>>0)&0x01);
      Serial.print(" , ");
      Serial.print((node1.getResponseBuffer(0)>>1)&0x01);
      Serial.print(" , ");
      Serial.print((node1.getResponseBuffer(0)>>2)&0x01);
      Serial.print(" , ");
      Serial.println((node1.getResponseBuffer(0)>>3)&0x01);
    }else{
      Serial.println("readCoils failed");
    }
    
    result = node1.readDiscreteInputs(0x0000, 10); 
    if(result == node1.ku8MBSuccess)
    {
      Serial.print("Input = ");
      Serial.print((node1.getResponseBuffer(0)>>0)&0x01);      //  register number : 10001
      Serial.print(" , ");
      Serial.print((node1.getResponseBuffer(0)>>1)&0x01);
      Serial.print(" , ");
      Serial.print((node1.getResponseBuffer(0)>>2)&0x01);
      Serial.print(" , ");
      Serial.println((node1.getResponseBuffer(0)>>3)&0x01);
    }else{
      Serial.println("readDiscreteInputs failed");
    }
    
    result = node1.readInputRegisters(0x0000, 10); 
    if(result == node1.ku8MBSuccess)
    {
      Serial.print("Input register = ");
      Serial.print(node1.getResponseBuffer(0));        //  register number : 30001
      Serial.print(" , ");
      Serial.print(node1.getResponseBuffer(1));
      Serial.print(" , ");
      Serial.print(node1.getResponseBuffer(2));
      Serial.print(" , ");
      Serial.println(node1.getResponseBuffer(3));
    }else{
      Serial.println("readInputRegisters failed");
    }
    
    
    node1.writeSingleRegister(0, time_1);       //  register number : 40001
    node1.writeSingleRegister(1, time_1+1);
    node1.writeSingleRegister(2, time_1+2);
    node1.writeSingleRegister(3, time_1+3);

    result = node1.readHoldingRegisters(0x0000, 10); 
    if(result == node1.ku8MBSuccess)
    {
      Serial.print("Holding register = ");
      Serial.print(node1.getResponseBuffer(0));
      Serial.print(" , ");
      Serial.print(node1.getResponseBuffer(1));
      Serial.print(" , ");
      Serial.print(node1.getResponseBuffer(2));
      Serial.print(" , ");
      Serial.println(node1.getResponseBuffer(3));
    }else{
      Serial.println("readHoldingRegisters failed");
    }
    
	}

  /*
  if(millis() - time_2 > INTERVAL_MESSAGE2){
    time_2 = millis();
  }
  */
  
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
 
}
