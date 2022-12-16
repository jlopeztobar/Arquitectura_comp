/*
  LiquidCrystal Library - display() and noDisplay()

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD and uses the
 display() and noDisplay() functions to turn on and off
 the display.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

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

 http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay

*/

// include the library code:
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <stdio.h>
#include <string.h>
#include "AsyncTaskLib.h"
#define LED_RED 39
#define LED_BLUE 40
#define LED_GREED 41
const int buzzerPin = 8;

int validarContrasenia();
void seguirIntentandolo();
void contraseniaCorrecta();
void contraseniaIncorrecta();
void limpiarArreglo();

//contraseña establecidad previamente
const int contrasenia[5] ={'1','2','3','4'};
//contraseña a digitar
int contrasenia_digitada[5];
//caracter que digita el teclado
int key = 0;
int reinicio = 0;

//variables
int contador = 0;
int intentos = 1; 
int fallos = 3;
int bandera = 0;
int aux = 0;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {32, 33, 34, 35}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {36, 37, 38}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//tareas asincronas
#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);

//funcion para que el teclado siempre esté activo
void leerContrasenia();
AsyncTask taskleer(100,true,leerContrasenia);

//validaciones de contraseñas
void validar();


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("SISTEMA DE SEGURIDAD");
  delay(3000);  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREED, OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  taskleer.Start();

  //digitalWrite(LED_BLUE, HIGH);
}
void loop() {

  taskleer.Update();
  
}
void validar(){
  Serial.println("VALIDAR");
if( (intentos <= fallos) && (bandera != 1)){
       lcd.print("SISTEMA DE SEGURIDAD");
       //leerContrasenia();
       if((validarContrasenia() == 1)){
           
            contraseniaCorrecta();
            bandera = 1;
        }else
        {
            seguirIntentandolo();
           
        }
  }else if(bandera == 1){
          contraseniaCorrecta();
  }
  else{
        contraseniaIncorrecta();
    }
  
}
void leerContrasenia()
{
      lcd.clear();
      lcd.print("INTENTO " + intentos);    
     do{
            char key = keypad.getKey();
            if (key){
            Serial.println(key);
            contrasenia_digitada[aux] = key;
            aux++;
            }          
      }while(aux<4);
      validar(); 
      Serial.println("SALIO");     
          
}
int validarContrasenia(){
 int i = 0;
 int j = 0;
 do{
      if(contrasenia_digitada[i] == contrasenia[i]){
          j++;
        }
        i++;
    }while(i < 4);

    if(j == 4){
      return 1;
      }else{
        return 0;
      }
}
void limpiarArreglo(){

    for(int i =0; i < 4; i++){
        contrasenia_digitada[i] = reinicio;
      }
}
void contraseniaCorrecta(){
        limpiarArreglo();
        lcd.clear();
        lcd.print("CLAVE CORRECTA");
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREED, HIGH);
        delay(5000);
  }
void seguirIntentandolo(){
        limpiarArreglo();
        lcd.clear();
        lcd.print("CLAVE INCORRECTA");
        lcd.print("SIGUE INTENTANDO");
        digitalWrite(LED_GREED, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_BLUE, HIGH);
        intentos++;
        delay(3000);
}
 void contraseniaIncorrecta(){
        limpiarArreglo();
        lcd.clear();
        lcd.print("CLAVE INCORRECTA");
        lcd.print("MAXIMO NUMERO DE INTENTOS");
        digitalWrite(LED_GREED, LOW);


        for(int i = 200;i <= 800;i++) //frequence loop from 200 to 800
{
tone(buzzerPin,i); //turn the buzzer on
delay(5); //wait for 5 milliseconds
}
delay(4000); //wait for 4 seconds on highest frequence
for(int i = 800;i >= 200;i--)//frequence loop from 800 downto 200
{
tone(buzzerPin,i);
delay(10);
}

        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_BLUE, LOW);
        delay(5000);
        
}
