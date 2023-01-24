 //The circuit:
 // LCD RS pin to digital pin 12
 // LCD Enable pin to digital pin 11
 // LCD D4 pin to digital pin 5
 // LCD D5 pin to digital pin 4
 // LCD D6 pin to digital pin 3
 // LCD D7 pin to digital pin 2
 // LCD R/W pin to ground
 // LCD VSS pin to ground
 // LCD VCC pin to 5V
 // 10K resistor:
 // ends to +5V and ground
 // wiper to LCD VO pin (pin 3)
 // LCD 15 to 5V
 // LCD 16 TO GND 

//#include <LiquidMenu.h>
//POR QUÉ ERRIOR EN LIQUIDMENU?
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
//#include "AsyncTaskLib.h"
//POR QUÉ ME SALE ERRIOR EN ASYNCTASK?

const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {32, 33, 34, 35}; 
byte colPins[COLS] = {36, 37, 38}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void menu_secundario();
void guardar_eeprom();
void recuerar_eeprom();

float varTempH = 29;
float varTempL = 26;
float varUmbLuzH = 100;
float varUmbLuzL = 80;

const int numOpciones = 5;
String v_opciones[numOpciones] = { "1. UtempL", "2. UtempH", "3. UluzH", "4. UluzL", "5. Reset" };
int varOpcion = 0;
int varOpcKey = 0;

void setup() {
  recuerar_eeprom();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(v_opciones[varOpcion]);
  lcd.setCursor(0, 1);
  lcd.print(v_opciones[varOpcion+1]);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      varOpcion++;
      if (varOpcion > numOpciones - 2) {
       lcd.print(v_opciones[varOpcion]);
       lcd.setCursor(0, 1);
       lcd.print(v_opciones[varOpcion+1]);
       varOpcion = 3;
      }
    } else if (key == '*') {
      varOpcion--;
      if (varOpcion <= 0) {
       lcd.print(v_opciones[varOpcion]);
       lcd.setCursor(0, 1);
       lcd.print(v_opciones[varOpcion+1]);
       varOpcion = 0;
      }
    }
    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5') {
      varOpcKey = key - '0';
      //POR QUÉ TENGO QUE RESTAR - '0' PARA QUE FUNCIONE?
      menu_secundario();
    }
    lcd.clear();
    lcd.print(v_opciones[varOpcion]);
    lcd.setCursor(0, 1);
    lcd.print(v_opciones[varOpcion+1]);
  }
}

void menu_secundario() {
  lcd.clear();
  switch (varOpcKey) {
    case 1:
      lcd.clear();
      lcd.print("TempH: ");
      lcd.print(varTempH);
      lcd.setCursor(0,1);
      lcd.print("Cambiar? 1si/2no");
      while(true){
          char key = keypad.getKey();
          if (key == '1') {
            lcd.clear();
            lcd.print("Ingrese valor:");
            lcd.setCursor(0,1);
            String str = "";
            while (true) {
              char key = keypad.getKey();
              if (key) {
                if (key == '#') {
                  float varValidar = str.toFloat();
                  if(varValidar < varTempL){
                    lcd.clear();
                    lcd.print("No puede ser menor");
                    delay(300);
                  }else if(varValidar > 250){
                    lcd.clear();
                    lcd.print("datos incorrectos");
                    delay(300);
                  }else{
                    varTempH = varValidar;
                    lcd.clear();
                    lcd.print("datos guardado");
                    delay(300);
                    guardar_eeprom();
                  }
                  break;
                }
                else {
                  str += key;
                  lcd.print(key);
                }
              }
            }
            break;
          } else if (key == '2') {
            lcd.clear();
            lcd.print("Sin cambios");
            break;
          }
      }
      break;
    case 2:
      lcd.clear();
      lcd.print("TempL: ");
      lcd.print(varTempL);
      lcd.setCursor(0,1);
      lcd.print("Cambiar? 1si/2no");
      while(true){
          char key = keypad.getKey();
          if (key == '1') {
            lcd.clear();
            lcd.print("Ingrese valor:");
            lcd.setCursor(0,1);
            String str = "";
            while (true) {
              char key = keypad.getKey();
              if (key) {
                if (key == '#') {
                  float varValidar = str.toFloat();
                  if(varValidar >= varTempH){
                    lcd.clear();
                    lcd.print("No puede ser mayor");
                    lcd.setCursor(0,1);
                    lcd.print("No puede ser igual");
                    delay(300);
                  }else{
                    varTempL = varValidar;
                    lcd.clear();
                    lcd.print("datos guardado");
                    delay(300);
                    guardar_eeprom();
                  }
                  break;
                }
                else {
                  str += key;
                  lcd.print(key);
                }
              }
            }
            break;
          } else if (key == '2') {
            lcd.clear();
            lcd.print("Sin cambios");
            break;
          }
      }
      break;
    case 3:
      lcd.clear();
      lcd.print("UluzH: ");
      lcd.print(varUmbLuzH);
      lcd.setCursor(0,1);
      lcd.print("Cambiar? 1si/2no");
      while(true){
          char key = keypad.getKey();
          if (key == '1') {
            lcd.clear();
            lcd.print("Ingrese valor:");
            lcd.setCursor(0,1);
            String str = "";
            while (true) {
              char key = keypad.getKey();
              if (key) {
                if (key == '#') {
                  float varValidar = str.toFloat();
                  if(varValidar < varUmbLuzL){
                    lcd.clear();
                    lcd.print("No puede ser menor");
                    delay(300);
                  }else if(varValidar > 1000){
                    lcd.clear();
                    lcd.print("datos incorrectos");
                    delay(300);
                  }else{
                    varUmbLuzH = varValidar;
                    lcd.clear();
                    lcd.print("datos guardado");
                    delay(300);
                    guardar_eeprom();
                  }
                  break;
                }
                else {
                  str += key;
                  lcd.print(key);
                }
              }
            }
            break;
          } else if (key == '2') {
            lcd.clear();
            lcd.print("Sin cambios");
            break;
          }
      }
      break;
    case 4:
      lcd.clear();
      lcd.print("UluzL: ");
      lcd.print(varUmbLuzL);
      lcd.setCursor(0,1);
      lcd.print("Cambiar? 1si/2no");
      while(true){
          char key = keypad.getKey();
          if (key == '1') {
            lcd.clear();
            lcd.print("Ingrese valor:");
            lcd.setCursor(0,1);
            String str = "";
            while (true) {
              char key = keypad.getKey();
              if (key) {
                if (key == '#') {
                  float varValidar = str.toFloat();
                  if(varValidar >= varUmbLuzH){
                    lcd.clear();
                    lcd.print("No puede ser mayor");
                    lcd.setCursor(0,1);
                    lcd.print("No puede ser igual");
                    delay(300);
                  }else{
                    varUmbLuzL = varValidar;
                    lcd.clear();
                    lcd.print("datos guardado");
                    delay(300);
                    guardar_eeprom();
                  }
                  break;
                }
                else {
                  str += key;
                  lcd.print(key);
                }
              }
            }
            break;
          } else if (key == '2') {
            lcd.clear();
            lcd.print("Sin cambios");
            break;
          }
      }
      break;
    case 5:
      lcd.print("Reset");
      lcd.setCursor(0, 1);
      lcd.print("Datos por defecto");
      varTempH = 29;
       varTempL = 26;
       varUmbLuzH = 100;
       varUmbLuzL = 80;
      break;
    default:
      lcd.print("ocurrio un error");
      break;
  }
  delay(1000);
  varOpcion = 0;
}
void guardar_eeprom(){
      EEPROM.write(0, varTempH); 
      EEPROM.write(1, varTempL); 
      EEPROM.write(2, varUmbLuzH); 
      EEPROM.write(3, varUmbLuzL); 
}
void recuerar_eeprom(){
     varTempH =  EEPROM.read(0); 
     varTempL =  EEPROM.read(1); 
     varUmbLuzH =  EEPROM.read(2); 
     varUmbLuzL = EEPROM.read(3); 
}
