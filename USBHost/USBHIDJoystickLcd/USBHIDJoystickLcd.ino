#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include "hidjoystickrptparser.h"

#define LED_BUILTIN 2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

void setup() {
        Serial.begin(115200);

        if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
          Serial.println(F("SSD1306 allocation failed"));
          for(;;);
        }
        //delay(1000);
        display.clearDisplay();
        display.setTextColor(WHITE);
        
        // display 
        display.setTextSize(1);
        display.setCursor(0,30);
        display.print("USB connecting ...");
        display.display();
        
#if !defined(__MIPSEL__)
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
        Serial.println("Start");

        if (Usb.Init() == -1)
                Serial.println("OSC did not start.");

        delay(200);

        if (!Hid.SetReportParser(0, &Joy))
                ErrorMessage<uint8_t > (PSTR("SetReportParser"), 1);
        // display 
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,30);
        display.print("USB init finish !");
        display.display();
}

void loop() {
        Usb.Task();
}
