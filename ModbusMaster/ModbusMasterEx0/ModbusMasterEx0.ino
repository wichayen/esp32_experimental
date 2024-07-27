#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ModbusMaster.h" 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LED_BUILTIN 2


long lastMsg = 0;
char msg[50];
int value = 0;
char led_status;

//=================================================================================================
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



// Initial state of RGB LED. It sets the RGB LED to green.
void intialState(){
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
}


void setup(){
  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200); // Starts the serial communication
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  // display 
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Wifi connecting ...");
  display.display();
  
  intialState();
  
  pinMode(RS485_DIRECTION_PIN, OUTPUT);                                                          // RS485 Direction
  digitalWrite(RS485_DIRECTION_PIN, RS485_RXD_SELECT);
  
  SerialRS485.begin(9600, SERIAL_8N1, SerialRS485_RX_PIN, SerialRS485_TX_PIN);
  while(!SerialRS485);

  node1.begin(10, SerialRS485);    // Modbus slave ID 10
  node1.preTransmission(preTransmission); //Callback for configuring RS-485 Transreceiver correctly
  node1.postTransmission(postTransmission);
}


void loop() {
  char mytest[] = "123";
  long sendMsg = millis();
  uint16_t inputReg[10];

  if (sendMsg - lastMsg > 2000) {
    
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
    
    
    node1.writeSingleRegister(0, sendMsg);       //  register number : 40001
    node1.writeSingleRegister(1, sendMsg+1);
    node1.writeSingleRegister(2, sendMsg+2);
    node1.writeSingleRegister(3, sendMsg+3);

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
    
    
    /*
    inputReg = node1.readInputRegisters(0, 10);
    Serial.print(inputReg[0]);
    Serial.print(inputReg[1]);
    Serial.print(inputReg[2]);
    */


    
    lastMsg = sendMsg;

    // Obtain temperature sensor readings
    //float humidity = dht.readHumidity();
    //Read temperature as Celsius (the default)
    //float temp = dht.readTemperature();
	
	// Obtain temperature sensor readings
    float humidity = 0;
    //Read temperature as Celsius (the default)
    float temp = 0;
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp)) {
		Serial.println("Failed to read from DHT sensor!");
        return;
    }
  } 
}
