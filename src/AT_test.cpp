// for github pull

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "AT_Speaker.hpp" // F1 -> easy c/c++ -> create new class -> constructor -> Choose his name
//#include <Mbee.h>

#define Software_RX 8
#define Software_TX 9
#define LedPin 13

SoftwareSerial DEBUG(Software_RX, Software_TX);

AT_Speaker at_courier;

bool alarmFlag = 0;
bool wireConnectionFlag = false;
int  temp_buf = 0;
bool ok_settings = false;
bool onlyWireControl = false;

void ErrorSound_ON()
{
  digitalWrite(LedPin, HIGH);
  DEBUG.println("SCREAM! ON");
  alarmFlag = true;
  delay(2000);
}

void ErrorSound_OFF()
{
  digitalWrite(LedPin, LOW);
  DEBUG.println("SCREAM! OFF");
  alarmFlag = false;
  delay(2000);
}

bool SetWireControl()
{
  if (wireConnectionFlag == false && digitalRead(A0) == false)
  {
    digitalWrite(A0, HIGH);
    wireConnectionFlag = true;
    DEBUG.println("Set WIRE Connection OK");
    return false;
  }
  else
    digitalWrite(A0, LOW);
    wireConnectionFlag = false;
    DEBUG.println("Set WIRE Connection OK");
    return true;
}

void setup() {
  pinMode(LedPin,OUTPUT);
  digitalWrite(LedPin, LOW);
  Serial1.begin(9600);
  while (!Serial1) {
    ; // ожидаем подключения к последовательному порту. Необходимо только для Leonardo
  }
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  DEBUG.begin(115200);
  // at_courier.setSerial(&DEBUG);
  at_courier.setSerial(&Serial1);
  at_courier.setDebugPort(&DEBUG);
  // Serial1.println("Text from main.cpp!");
  delay(1000);
  // at_courier.print_txt();
  // Serial1.println("Start Serial port...");
  DEBUG.println(F("DEBUG port is OK....."));
  at_courier.testFunc();
}

void loop() 
{
  static int cntr = 0;
  // put your main code here, to run repeatedly:
  DEBUG.println();
  DEBUG.print(F("...DEBUG... "));
  DEBUG.println(random(0,101));

  uint32_t x = 2;
  uint8_t lol = 10;
  uint8_t int_test = 1;
  x = 10 * 1000000;
  DEBUG.print("x==");
  DEBUG.println(x);

  for (uint8_t f = 8; f > 0 ; f--)
  {
    x = x * 10;
    DEBUG.print("x*x10 == ");
    DEBUG.println(x);
  }

  DEBUG.println(pow(10,8));
  DEBUG.println(pow(10, (lol - int_test - 1)) );  // work! wahaaat
  uint32_t ttt = pow(10, (lol - int_test - 1));
  DEBUG.println(ttt);
  DEBUG.println(ttt * 8);
  // while(1);
  
  // Serial1.print("main serial -> ");
  // Serial1.println(random(-100,0));
  // at_courier.sendTriplePlus();
  switch(at_courier.setDefaultSettings())
  {
    case true:
    {
      DEBUG.println(F("we have default parameters !"));
      ok_settings = true;
      break;
    }
    case OVERWRITE_PARAMETRERS:
    {
      DEBUG.println(F("we have OVERWRITE_PARAMETRERS !"));
      ok_settings = true;
      break;
    }
    case false:
    {
      DEBUG.println(F("Failed with podliva! WIRE_MODE ONLY ACTIVATED!"));
      ok_settings = false;
      break;
    }



  }





  // BEST!
  // int temp1 = at_courier.setDefaultSettings();
  // if (temp1 == OVERWRITE_PARAMETRERS)
  // {
  //   DEBUG.println("we have OVERWRITE_PARAMETRERS !");
  //   ok_settings = true;
  // }
  // if (temp1 == true)
  // {
  //   DEBUG.println("we have default parameters !");
  //   ok_settings = true;
  // }
  // if (temp1 == false)
  // {
  //   onlyWireControl = true;
  //   ok_settings = false;
  //   DEBUG.println(F("Failed with podliva! WIRE_MODE ONLY ACTIVATED!"));
  //   while(1);
  // }

  delay(500);
  digitalWrite(LedPin, !digitalRead(LedPin));
  DEBUG.println();
  DEBUG.println(F("Cycle end...."));
  DEBUG.println();
  delay(1000);
  digitalWrite(LedPin, !digitalRead(LedPin));

  // Win!
  if (ok_settings == true)
  {
    DEBUG.println("SUCCESSFUL!");
    while(1);
  }

  // emergency stop for rescue rewrite Mbee
  cntr ++;
  if (cntr >= 10)
  {  
    DEBUG.println(F("loop x 10! STOP THIS"));
    while(1);
  }

}