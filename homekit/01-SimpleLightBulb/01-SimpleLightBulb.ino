#include "HomeSpan.h"


#define LED_BUILTIN 2

class LEDtester : Service::LightBulb {//定義したクラス

  private:
  int LEDpin;//LEDピン番号
  SpanCharacteristic *LEDstatus;

  public:

  LEDtester(int thePin) : Service::LightBulb() {
    LEDstatus = new Characteristic::On();
    LEDpin=thePin;//引数をLEDのピン番号にする
    pinMode(LEDpin,OUTPUT);
  } // end constructor()

  boolean update(){
    digitalWrite(LEDpin, LEDstatus->getNewVal());
    return(true);
  } // end update()

};  //クラス定義はここまで


void setup() {
  Serial.begin(115200);
  homeSpan.begin(Category::Lighting,"HomeSpan LightBulb");
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new LEDtester(LED_BUILTIN);//電球機能を別クラスにした
}

void loop(){
  homeSpan.poll();
}
