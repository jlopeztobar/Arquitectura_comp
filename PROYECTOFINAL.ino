/*
PROYECTO REALIZADO POR: JUAN ESTEBAN LOPEZ
*/
// include the library code:
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <stdio.h>
#include <string.h>
#include "AsyncTaskLib.h"
#include <EEPROM.h>
#include "DHTStable.h"

#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a); // definicion del serial debug

//*************************  Definición de pines de conexión  *********************** 
#pragma region def_pines_conexion

// Pines para el LED
#define LED_RED 39
#define LED_BLUE 40
#define LED_GREEN 41

// sensor de temperatura
#define DHT11_PIN 22
DHTStable DHT;


// Pin buzzer 
const int buzzerPin = 8;

//variables para el control del buzzer analogico
#define pin_buzz 8
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
int duraciones[] = { 8, 8, 4, 4 };    // array con la duracion de cada nota
int melodia[] = { NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7 };//// array con las notasa a reproducir


// pines y configuracion del teclado 
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

// Pines y configuración del dysplay 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#pragma endregion

#pragma region def_ estados_inputs // se definen los enumeradores para los estados y las input, tambien las banderas de inicializacion

// enumerador de los estados finitos
enum estado
{
  estado1_seguridad,// sistema de seguridad
  estado2_config,// menu de configuracion
  estado3_monitoreo,// estado de monitoreo de sensores
  estado4_alarma// estado de alarma
};
estado currentState; // Variable global para control de estados

enum Input
{
  Unknown, // entrada en vacio
  reset,  // entrada de reset
  forward, // entrada para continuar al siguiente estado
  Backward // entrada para retroceder un estado
};
Input currentInput; // Variable global para control de entradas

//declaraciones de banderas auxiliares para inicializar los estados 
bool flag_start_estado1 = true;
bool flag_start_estado2 = true;
bool flag_start_estado3 = true;
bool flag_start_estado4 = true;

#pragma endregion


///************  Declaracion de variables y metodos para los estados *****/ //
#pragma region declaraciones_estado1

//declaracion de metodos estado 1
int validarContrasenia();
void seguirIntentandolo();
void contraseniaCorrecta();
void contraseniaIncorrecta();
void limpiarArreglo();
//validaciones de contraseñas
void validar();

//contraseña establecidad previamente
const int contrasenia[5] ={'1','2','3','4'};

//contraseña a digitar
int contrasenia_digitada[5];

//caracter que digita el teclado
int key = 0;
int reinicio = 0;

//variables auxiliares
int contador = 0;
int intentos = 1; 
int fallos = 3;
int bandera = 0;
int aux = 0;



//funcion para que el teclado siempre esté activo
void leerContrasenia();
AsyncTask taskleer(100,true,leerContrasenia);

#pragma endregion

#pragma region declaraciones_estado2

// declaracion de  metodos del estado 2
void menu_secundario();
void guardar_eeprom();
void recuerar_eeprom();

// umbrales predefinidos
float varTempH = 29; // umbral temperatura alta
float varTempL = 26; // umbral temperatura baja
float varUmbLuzH = 100; // umbral luz alta
float varUmbLuzL = 80;  // umbral luz baja

const int numOpciones = 8; //cantidad de opciones en el menu de configuracion
String v_opciones[numOpciones-2] = { "1. UtempL", "2. UtempH", "3. UluzH", "4. UluzL", "5. Reset", "6. Continuar"}; // opciones del menu de configuracion(no se muestran las opciones reset y back) 
int varOpcion = 0;
int varOpcKey = 0;


#pragma endregion

#pragma region declaraciones_estado3

int var_hum = 0; // variable para el valor de la humedad
int var_temp = 0; // variable para la temperatura

void temperatura();
void mostrar_led();

AsyncTask tasktemp(1000,true, temperatura);
AsyncTask taskmostrar(100,true, mostrar_led);

#pragma endregion



void setup() {
  
  // se inicia el serial para debug
  Serial.begin(9600);
    Serial.println("inicio"); 

  // inicializacion del lcd
  lcd.begin(16, 2);
  lcd.clear();

    //se inicializan los pines del led  y buzzer como salidas
    //leds
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  //buzzer
  pinMode(buzzerPin,OUTPUT);

 

  currentState =  estado::estado1_seguridad; // se inicializa la maquina en estado inicial
  currentInput = Input::Unknown; // se inicializa la entrada en vacio

}

void loop() {

  char key = keypad.getKey(); //se lee el teclado para detectar las opciones reset y back en todo momento
  if (key) {
      press_key(key);
  }

  actualizar_estado(); // actualiza la maquina de estados segun la entrada 

}

///****************  metodos para la maquina de estados  *****************///
#pragma region maquina_estados

void actualizar_estado(){ // se hace la seleccion de funciones a realizar segun el estado actual y la entrada

  switch (currentState)
  {
    case estado::estado1_seguridad:  // actualiza el estado de la maquina partiendo desde el estado 1
        switch (currentInput){
        case Input::Unknown: funcion_estado1(); break; // con entrada en vacio, la maquina continua en el estado actual y ejecuta sus funciones
        case Input::reset: //********** RESET
            currentState = estado::estado1_seguridad; 
            flag_start_estado1  = true;
            currentInput = Input::Unknown;
            lcd.clear();
            lcd.setCursor(0, 0);
           break;
        case Input::forward: //**********forward
            currentState = estado::estado2_config; 
            flag_start_estado1  = true;
            currentInput = Input::Unknown;
            lcd.clear();
            lcd.setCursor(0, 0);
          break;
        case Input::Backward: //********** BACK
            currentState = estado::estado1_seguridad; 
            flag_start_estado1  = true;
            currentInput = Input::Unknown;
            lcd.clear();
            lcd.setCursor(0, 0);
           break;
        default: break;
      }
      break;
    case estado::estado2_config: // actualiza el estado de la maquina partiendo desde el estado 2
        switch (currentInput){
          case Input::Unknown: funcion_estado2(); break; // con entrada en vacio, la maquina continua en el estado actual y ejecuta sus funciones
          case Input::reset: //********** RESET
              currentState = estado::estado1_seguridad; 
              flag_start_estado2  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          case Input::forward: //**********forward
              currentState = estado::estado3_monitoreo; 
              flag_start_estado2  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          case Input::Backward: //********** BACK
              currentState = estado::estado1_seguridad; 
              flag_start_estado2  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          default: break;
        }
       break;
    case estado::estado3_monitoreo: // actualiza el estado de la maquina partiendo desde el estado 3
     switch (currentInput){
          case Input::Unknown: funcion_estado3(); break; // con entrada en vacio, la maquina continua en el estado actual y ejecuta sus funciones
          case Input::reset: //********** RESET
              currentState = estado::estado1_seguridad; 
              flag_start_estado3  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          case Input::forward: //**********forward
              currentState = estado::estado4_alarma; 
              flag_start_estado3  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          case Input::Backward: //********** BACK
              currentState = estado::estado2_config; 
              flag_start_estado3  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          default: break;
        }
      break;
    case estado::estado4_alarma: // actualiza el estado de la maquina partiendo desde el estado 4
     switch (currentInput){
          case Input::Unknown: funcion_estado4(); break; // con entrada en vacio, la maquina continua en el estado actual y ejecuta sus funciones
          case Input::reset: //********** RESET
              currentState = estado::estado1_seguridad; 
              flag_start_estado4  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          case Input::forward: //**********forward
              currentState = estado::estado3_monitoreo; 
              flag_start_estado4  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          case Input::Backward: //********** BACK
              currentState = estado::estado3_monitoreo; 
              flag_start_estado4  = true;
              currentInput = Input::Unknown;
              lcd.clear();
              lcd.setCursor(0, 0);
            break;
          default: break;
        }
      break;
    default: break;
  }
}

 void press_key(char key) { // este metodo me ejecuta la lectura de la entrada desde las teclas 8 (back) y 9 (reset)
  if (key == '8') {
     lcd.clear();
    lcd.setCursor(0, 0);
      lcd.print("BACK");
      Serial.println("BACK");
      delay(500);

        currentInput = Input::Backward; //********** BACK

  } 
  if (key == '9') {
     lcd.clear();
    lcd.setCursor(0, 0);
      lcd.print("RESET");
      Serial.println("RESET");
      delay(500);
       currentInput = Input::reset; //********** RESET

  }

}

void funcion_estado1(){ // metodo principal del estado 1
 Serial.println(" maquina en estado 1 seguridad"); // se imprime en el debug el estado actual

 if (flag_start_estado1) { // se inicializa las variables y tareas del metodo solo 1 vez al iniciar el estado
    setup_estado1();
    flag_start_estado1 = false;
  }

  taskleer.Update(); // se actualiza la lectura del teclado para la contraseña

}

void funcion_estado2(){ // metodo principal del estado 2

 Serial.println(" maquina en estado 2 configuracion"); // se imprime en el debug el estado actual

  if (flag_start_estado2) { // se inicializa las variables y tareas del metodo solo 1 vez al iniciar el estado
    setup_estado2();
    flag_start_estado2 = false;
  }

   char key = keypad.getKey();
  if (key) {  // me permite avanzar en el menu  con la tecla # y retroceder con *
    if (key == '#') {
      if(varOpcion < numOpciones - 4 ){
        varOpcion++;
       }
      
      if (varOpcion <= numOpciones - 3) {
       lcd.print(v_opciones[varOpcion]);
       lcd.setCursor(0, 1);
       lcd.print(v_opciones[varOpcion+1]);
       
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
    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5'|| key == '6'|| key == '8'|| key == '9') { // verificacion de la opcion seleccionada del menu
      varOpcKey = key - '0';
      //POR QUÉ TENGO QUE RESTAR - '0' PARA QUE FUNCIONE?
      menu_secundario();
    }
    lcd.clear();
    lcd.print(v_opciones[varOpcion]); //se imprime en pantalla el menu de configuracion dependiendo de la posicion del cursor de opciones
    lcd.setCursor(0, 1);
    lcd.print(v_opciones[varOpcion+1]);
  }
}

void funcion_estado3(){  // metodo principal del estado 3

   Serial.println(" maquina en estado 3 monitoreo"); // se imprime en el debug el estado actual

  if (flag_start_estado3) { // se inicializa las variables y tareas del metodo solo 1 vez al iniciar el estado
    setup_estado3();
    flag_start_estado3 = false;
  }
  tasktemp.Update(tasktemp); // se actualiza las tareas de leer la temperatura y la de verificacion
  taskmostrar.Update(taskmostrar);

  lcd.clear(); // se muestra en pantalla y el debug los valores del sensor
  lcd.setCursor(0, 0);
  lcd.print("hum " );
  lcd.print(var_hum );
  Serial.println("hum" );
  Serial.println(var_hum );
  lcd.setCursor(0, 1);
  lcd.print("temp " );
  lcd.print(var_temp );
  Serial.println("temp");
    Serial.println(var_temp );

  delay(10);

}

void funcion_estado4(){  // metodo principal del estado 4
  
   Serial.println("maquina en estado 4 alarma"); // se imprime en el debug el estado actual

  if (flag_start_estado4) { // se inicializa las variables y tareas del metodo solo 1 vez al iniciar el estado
    setup_estado4();
    flag_start_estado4 = false;
  }

 
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, LOW); // led en azul
  delay(200);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW); // led rojo

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARMA TEM HIGH");
  sonar_buzz(); // se hace sonar el buzzer en señal de alarma
    delay(5000);// se da tiempo de espera de alarma

 // tiempo = millis(); // se da tiempo de espera de alarma
 // while (millis() < tiempo + 5000UL) {// crea un retardo para la correcta visualizacion
 // }

   currentInput = Input::forward; //********** forward, se actualiza la entrada para cambar de estado

}
#pragma endregion


///****************  metodos estado 1 sistema de seguridad  *****************///
#pragma region metodos_estado1_seguridad

void setup_estado1(){ // se inicializan las variables y tareas del estado 1
    //caracter que digita el teclado
  key = 0;
  reinicio = 0;

  //variables
   contador = 0;
   intentos = 1; 
   fallos = 3;
   bandera = 0;
   aux = 0;


   // inicializacion de las tareas asincronas

  taskleer.Start();
}

void validar(){ //se hace la validacion de la contraseña ingresada
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

void leerContrasenia(){ //se lee la entrada del teclado y se guarda en un vector
      lcd.clear();
      lcd.print(" INTENTO " + intentos);    
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

int validarContrasenia(){ // valida si la contraseña ingresada es la correcta
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

void limpiarArreglo(){ //limpia el vector de la contraseña digitada

    for(int i =0; i < 4; i++){
        contrasenia_digitada[i] = reinicio;
      }
       aux = 0;
}

void contraseniaCorrecta(){ // se ejecuta si la contraseña es correcta
        limpiarArreglo();
        lcd.clear();
        lcd.print("CLAVE CORRECTA");
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        delay(5000);

        currentInput = Input::forward; //**********forward, si la contraseña es correcta se actualiza la entrada para ir al siguiente estado
        //flag_start_estado1  = true;
        //lcd.clear();
       // lcd.setCursor(0, 0);

}

void seguirIntentandolo(){// se ejecuta si la contraseña es incorrecta y lleva menos de 3 intentos
        limpiarArreglo();
        lcd.clear();
        lcd.print("CLAVE INCORRECTA");
        lcd.print("SIGUE INTENTANDO");
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_BLUE, HIGH);
        intentos++;
        delay(3000);
}

 void contraseniaIncorrecta(){ // se ejecuta si la contraseña es incorrecta
          limpiarArreglo();
          lcd.clear();
          lcd.print("CLAVE INCORRECTA");
          lcd.print("MAXIMO NUMERO DE INTENTOS");
          digitalWrite(LED_GREEN, LOW);


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

  noTone(buzzerPin);
  currentInput = Input::reset; //**********reset

          
}
#pragma endregion

///****************  metodos estado 2 menu de configuracion  *****************///
#pragma region metodos_estado2_config

void setup_estado2(){ // se inicializan las variables y tareas del estado 2
  varTempH = 29;
  varTempL = 26;
  varUmbLuzH = 100;
  varUmbLuzL = 80;

  String v_opciones[numOpciones] = { "1. UtempH", "2. UtempL", "3. UluzH", "4. UluzL", "5. Reset" ,"6. Continuar"};
  varOpcion = 0;
  varOpcKey = 0;


 recuerar_eeprom();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(v_opciones[varOpcion]);
  lcd.setCursor(0, 1);
  lcd.print(v_opciones[varOpcion+1]);
}

void menu_secundario() { // muestra el menu de modificacion de umbrales de los sensores
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
       guardar_eeprom();
      break;
    case 6:
      currentInput = Input::forward; //********** forward
      //  flag_start_estado2  = true;
      //  lcd.clear();
       // lcd.setCursor(0, 0);
      break;
    case 8:
     currentInput = Input::Backward; //********** BACK
       // flag_start_estado2  = true;
       // lcd.clear();
       // lcd.setCursor(0, 0);
     break;
    case 9:
     currentInput = Input::reset; //********** RESET
       // flag_start_estado2  = true;
      //  lcd.clear();
      //  lcd.setCursor(0, 0);
     break;
    default:
      lcd.print("ocurrio un error");
      break;
  }
  delay(1000);
  varOpcion = 0;
}

void guardar_eeprom(){ // se guardan los umbrales en la memoria eeprom
      EEPROM.write(0, varTempH); 
      EEPROM.write(1, varTempL); 
      EEPROM.write(2, varUmbLuzH); 
      EEPROM.write(3, varUmbLuzL); 
}

void recuerar_eeprom(){ // recupera los valores de los umbrales almacenados en la memoria eeprom
     varTempH =  EEPROM.read(0); 
     varTempL =  EEPROM.read(1); 
     varUmbLuzH =  EEPROM.read(2); 
     varUmbLuzL = EEPROM.read(3); 
}

#pragma endregion

///****************  metodos estado 3 monitoreo  *****************///
#pragma region metodos_estado3_monitoreo

void setup_estado3(){ // se inicializan las variables y tareas del estado 3
  tasktemp.Start();
 taskmostrar.Start();
  recuerar_eeprom();
  var_hum = 0;
 var_temp = 0;
}


void mostrar_led() { //muestra el led dependiendo de la temperatura
  if (var_temp > varTempH) {
    digitalWrite(LED_RED, HIGH);
      currentInput = Input::forward; //********** forward
      //  flag_start_estado3  = true;
     //   lcd.clear();
     //   lcd.setCursor(0, 0);
  } else if (var_temp < varTempL) {
    digitalWrite(LED_BLUE, HIGH);
  } else {
    digitalWrite(LED_GREEN, HIGH);
  }
}

void temperatura() { // valida el estado del sensor y recupera los valores de temperatura y humedad des sensor 
  Serial.println("TEMPERATURA");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.println("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.println("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.println("Time out error,\t");
      break;
    default:
      Serial.println("Unknown error,\t");
      break;
  }
  
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, LOW);
  var_hum = DHT.getHumidity();
  var_temp = DHT.getTemperature();
}

#pragma endregion

///****************  metodos estado 4 alarma *****************///
#pragma region metodos_estado4_alarma

void setup_estado4(){ // se inicializan las variables y tareas del estado 3
  
}

void sonar_buzz() { // funcion encargada de hacer sonar el buzzer
  for (int i = 0; i < 3; i++) {      // bucle repite 4 veces, 1 por cada nota
    int duracion = 1000 / duraciones[i];    // duracion de la nota en milisegundos
    tone(pin_buzz, melodia[i], duracion);  // ejecuta el tono con la duracion
    int pausa = duracion * 1.30;      // calcula pausa
    delay(pausa);         // demora con valor de pausa
    noTone(pin_buzz);        // detiene reproduccion de tono
  }
}

#pragma endregion
