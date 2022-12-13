/*
  LiquidCrystal Library - Hello World
  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.
  This sketch prints "Hello World!" to the LCD
  and shows the time.
  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe
  modified 7 Nov 2016
  by Arturo Guadalupi
  This example code is in the public domain.
  http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld
*/
#include <LiquidCrystal.h>
#include "AsyncTaskLib.h"

#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);

void temperatura();
void mostrar_led();

AsyncTask tasktemp(1000,true, temperatura);
AsyncTask taskmostrar(100,true, mostrar_led);


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include "DHTStable.h"

DHTStable DHT;

#define DHT11_PIN 22
#define LED_GREEN 24
#define LED_RED 26
#define LED_BLUE 28

int var_hum = 0;
int var_temp = 0;

void setup() {
  Serial.begin(9600);
  tasktemp.Start();
 taskmostrar.Start();
  lcd.begin(16, 2);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

}

void loop() {
  tasktemp.Update(taskmostrar);
  taskmostrar.Update(tasktemp);
}

void mostrar_led() {
  if (var_temp > 28) {
    digitalWrite(LED_RED, HIGH);
  } else if (var_temp < 25) {
    digitalWrite(LED_BLUE, HIGH);
  } else {
    digitalWrite(LED_GREEN, HIGH);
  }
}

void temperatura() {
  DEBUG("TEMPERATURA");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }
  
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, LOW);
  var_hum = DHT.getHumidity();
  var_temp = DHT.getTemperature();
}
