/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

#define LED_BUILTIN 2

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  Serial.println("--------------------");
  Serial.println("Software version :");
  Serial.print("Date : ");
  Serial.print( F(__DATE__));
  Serial.print("\r\n");
  Serial.print("Time : ");
  Serial.print( F(__TIME__));
  Serial.print("\r\n");
  Serial.println("--------------------");
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("LED OFF");
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED off
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second


  Serial.println("LED ON");
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED on
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
}
