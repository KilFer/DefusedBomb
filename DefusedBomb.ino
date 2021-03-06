#include <LiquidCrystal.h>
  
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int lcd_key     = 0;
int adc_key_in  = 0;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int pinEntradas[] = {22,24,26,28,30,32,34,36,38,40,42};
int pinSalidas[] = {31,33,35,37,39,41,43,45,47,49,51};
int pinesConectados[5][2];
int pinExplosivo;
long timeFinal;
char* colorCable[] = {"ROJO ","AMARI","VERDE","NEGRO","GRIS "};
int temporizador;

int read_LCD_buttons();  // para leer los botones
void inicializacionPines();
void conexionadoPines();
void eleccionPinExplosivo();
void activacionPines();
boolean verificacionPines();
boolean verificacionPinExplosivo();
void configuracionTiempo();
void seleccionTiempo();
void nuevoJuego();
void pantallaInicio();

//===========================================================================================

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(A1));
  Serial.begin(9600);
  lcd.begin(16, 2);              // Inicializar el LCD
  pantallaInicio();
  lcd.setCursor(0,1);
  lcd.print("Calculando ruta");
  nuevoJuego();
}

void loop() {
  // put your main code here, to run repeatedly:
     lcd.setCursor(0,0);
     lcd.print("Tic... Tac...   ");
     lcd.setCursor(0,1);
     lcd.print((timeFinal-millis())/1000);
     if ( timeFinal-millis() <= 10000){
       lcd.print(" ");
     }
     
    //lcd_key = read_LCD_buttons();

    Serial.println("Paso por menu");

  if (!verificacionPinExplosivo()){
     lcd.setCursor(0,0);
     lcd.print("¡Correcto!      ");
     delay(2500);
     nuevoJuego();
  } else if (!verificacionPines() || millis() > timeFinal){
     lcd.setCursor(0,0);
     lcd.print("ERROR! PUM!     ");
     delay(2500);
     nuevoJuego();
  }
}

//=============================================================================================================================

int read_LCD_buttons()  
  { adc_key_in = analogRead(0);      // Leemos A0
    // Mis botones dan:  RIGHT 0, UP 100, DOWN 257, RIGHT 410, SELECT 640
    // Y ahora los comparamos con un margen comodo
    if (adc_key_in > 900) return btnNONE;     // Ningun boton pulsado 
    if (adc_key_in < 50)   return btnRIGHT; 
    if (adc_key_in < 150)  return btnUP;
    if (adc_key_in < 300)  return btnDOWN;
    if (adc_key_in < 500)  return btnLEFT;
    if (adc_key_in < 850)  return btnSELECT; 

    return btnNONE;  // Por si todo falla
 }

//=============================================================================================================================

 void inicializacionPines()
 {
  //Primero se van a seleccionar los 5 pines de entrada.
  boolean distintos = false;
  while(!distintos){
    for(int i=0; i<5; i++){ 
      int valorRandom = (int) random(11);
      Serial.print("Valor obtenido: ");
      Serial.println(valorRandom);
      pinesConectados[i][0] = valorRandom;
    }
    //se han escrito los 5 valores. Se comprueban que sean distintos.
    distintos = true;
    for(int i=0; i<5; i++){
      for(int j=i+1; j<5; j++){
        if (pinesConectados[i][0] == pinesConectados[j][0]){
          distintos = false;
          Serial.println("Se ha repetido 1 caracter.");
          Serial.print("Posicion i=");
          Serial.print(i);
          Serial.print("; valor=");
          Serial.print(pinesConectados[i][0]);
          Serial.print(" Posicion j=");
          Serial.print(j);
          Serial.print("; valor=");
          Serial.println(pinesConectados[j][0]);
        }
      }
    }
  } 

  //Una vez seleccionados los 5 pines de entrada, se seleccionan los 5 de salida.
  Serial.println("Valores de entrada medidos.");
  distintos = false;
  while(!distintos){
    for(int i=0; i<5; i++){ 
      int valorRandom = (int) random(11);
      Serial.print("Valor obtenido: ");
      Serial.println(valorRandom);
      pinesConectados[i][1] = valorRandom;
    }
    //se han escrito los 5 valores. Se comprueban que sean distintos.
    distintos = true;
    for(int i=0; i<5; i++){
      for(int j=i+1; j<5; j++){
        if (pinesConectados[i][1] == pinesConectados[j][1]){
          distintos = false;
          Serial.println("Se ha repetido 1 caracter.");
          Serial.print("Posicion i=");
          Serial.print(i);
          Serial.print("; valor=");
          Serial.print(pinesConectados[i][1]);
          Serial.print(" Posicion j=");
          Serial.print(j);
          Serial.print("; valor=");
          Serial.println(pinesConectados[j][1]);
        }
      }
    }
  } 

  //SE HAN OBTENIDO LOS VALORES DE ENTRADA Y SALIDA. Ahora se transforma a los puertos reales.
  for(int i=0; i<5; i++){
    Serial.print("Valor de entrada final:");
    Serial.println(pinEntradas[pinesConectados[i][0]]);
    pinesConectados[i][0] = pinEntradas[pinesConectados[i][0]];
    pinMode(pinesConectados[i][0],INPUT);      
  }
  for(int i=0; i<5; i++){
    pinesConectados[i][1] = pinSalidas[pinesConectados[i][1]];
    pinMode(pinesConectados[i][1],OUTPUT);  
  }
 }


 //=============================================================================================================================

 void conexionadoPines(){
  for(int i=0;i<5;i++){
    boolean configurado = false;
    while(!configurado){
    lcd.setCursor(0,0);
    lcd.print(colorCable[i]);
    lcd.print(" - ");
    lcd.print(pinesConectados[i][0]);
    lcd.print(" a ");
    lcd.print(pinesConectados[i][1]);
    lcd.setCursor(0,1);
    lcd.print("Pulse SELECT");
      if(read_LCD_buttons()==btnSELECT){
        configurado = true;
        digitalWrite(pinesConectados[i][1],HIGH);
        if (digitalRead(pinesConectados[i][0]) != HIGH){
            configurado = false;
        }
        digitalWrite(pinesConectados[i][1],LOW);
        if (digitalRead(pinesConectados[i][0]) != LOW){
            configurado = false;
        }
        // Se ha hecho ya la comprobacion. ¿Se ha configurado correctamente?
        if (configurado == false){
          lcd.setCursor(0,0);
          lcd.print("ERROR! REVISE   ");
        }
        delay(500); 
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Revisando...    ");
  delay(500);
  boolean configurado = true;
  for(int i=0;i<5;i++){
    digitalWrite(pinesConectados[i][1],HIGH);
        if (digitalRead(pinesConectados[i][0]) != HIGH){
            configurado = false;
        }
        digitalWrite(pinesConectados[i][1],LOW);
        if (digitalRead(pinesConectados[i][0]) != LOW){
            configurado = false;
        }
    }
  if(configurado == false){
    lcd.setCursor(0,0);
    lcd.print("ERROR! Revise.  ");
    conexionadoPines();
  } else {
    lcd.setCursor(0,0);
    lcd.print("Todo OK!        ");
    delay(500);  
  }
}

//=============================================================================

void eleccionPinExplosivo(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cable explosivo?");
  pinExplosivo = 0;
  lcd.setCursor(0,1);
  lcd.print(colorCable[pinExplosivo]);
  delay(100);
  int botonPulsado = read_LCD_buttons();
  while(botonPulsado != btnSELECT){
      botonPulsado = read_LCD_buttons();
      if (botonPulsado == btnDOWN){
          pinExplosivo--;
          if (pinExplosivo < 0){
            pinExplosivo = 5;
          }
          if (pinExplosivo == 5) {
            lcd.setCursor(0,1);
            lcd.print("ALEATORIO");
          } else {
            lcd.setCursor(0,1);
            lcd.print(colorCable[pinExplosivo]);
            lcd.print("     ");
          }
          delay(250);
      }
      else if (botonPulsado == btnUP){
          pinExplosivo++;
          if (pinExplosivo > 5){
            pinExplosivo = 0;
          }
          if (pinExplosivo == 5) {
            lcd.setCursor(0,1);
            lcd.print("ALEATORIO");
          } else {
            lcd.setCursor(0,1);
            lcd.print(colorCable[pinExplosivo]);
            lcd.print("     ");
          }
          delay(200);
      }
   }

  if(pinExplosivo == 5) {
    pinExplosivo = (int) random(5);
  }
  lcd.setCursor(0,0);
  lcd.print("Cable elegido:  ");
  Serial.print("Pin explosivo: ");
  Serial.print(pinesConectados[pinExplosivo][0]);
  Serial.print("-");
  Serial.println(pinesConectados[pinExplosivo][1]);
  delay(2500);
}

//=============================================================================

void activacionPines(){
  for(int i=0;i<5;i++){
     digitalWrite(pinesConectados[i][1],HIGH);
  }
}

boolean verificacionPines(){
  boolean todoCorrecto = true;
  for (int i=0; i<5; i++) {
    digitalWrite(pinesConectados[i][1],HIGH);
    if (digitalRead(pinesConectados[i][0]) != HIGH){
       todoCorrecto = false;
    }
    digitalWrite(pinesConectados[i][1],LOW);
    if (digitalRead(pinesConectados[i][0]) != LOW){
       todoCorrecto = false;
    }
    digitalWrite(pinesConectados[i][1],HIGH);  
  }
  if(todoCorrecto == false){
    Serial.println("Algun pin suelto!");
  }
  return todoCorrecto;
}
boolean verificacionPinExplosivo(){
  boolean todoCorrecto = true;
  Serial.println("Paso por verificacion explosivo");
  digitalWrite(pinesConectados[pinExplosivo][1],LOW);
  if (digitalRead(pinesConectados[pinExplosivo][0]) != LOW){
     todoCorrecto = false;
  }
  digitalWrite(pinesConectados[pinExplosivo][1],HIGH);
  if (digitalRead(pinesConectados[pinExplosivo][0]) != HIGH){
     todoCorrecto = false;
  }
  if(todoCorrecto == false){
    Serial.println("Bomba desactivada!");
  }
  return todoCorrecto;
}

//============================================================================

void configuracionTiempo(){
  long timeInicio = millis(); // Milisegundos!
  timeFinal = timeInicio + temporizador;
  Serial.println("");
  Serial.print("Tiempo actual: ");
  Serial.println(timeInicio);  
  Serial.print("Tiempo final: ");
  Serial.println(timeFinal);
}

//=============================================================================

void seleccionTiempo(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Selecc. tiempo ");
  int tiempoSeleccionado = 60;
  lcd.setCursor(0,1);
  lcd.print(tiempoSeleccionado);
  lcd.print("  segundos");
  delay(100);
  int botonPulsado = read_LCD_buttons();
  while(botonPulsado != btnSELECT){
      botonPulsado = read_LCD_buttons();
      if (botonPulsado == btnDOWN){
          tiempoSeleccionado = tiempoSeleccionado - 5;
          if (tiempoSeleccionado < 10){
            tiempoSeleccionado = 10;
          }
          lcd.setCursor(0,1);
          lcd.print(tiempoSeleccionado);
          lcd.print(" ");
          delay(200);
      }
      else if (botonPulsado == btnUP){
          tiempoSeleccionado = tiempoSeleccionado + 5;
          if (tiempoSeleccionado > 120){
            tiempoSeleccionado = 120;
          }
          lcd.setCursor(0,1);
          lcd.print(tiempoSeleccionado);
          delay(200);
      }
   }
  lcd.setCursor(0,0);
  lcd.print("Tiempo decidido:");
  delay(2500);
  temporizador = tiempoSeleccionado * 1000;
}

//========================================================================

void nuevoJuego(){
  inicializacionPines();
  lcd.clear();
  conexionadoPines();
  verificacionPines();
  activacionPines();
  seleccionTiempo();
  eleccionPinExplosivo();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LISTO!");
  delay(2000);
  configuracionTiempo();
}

//=======================================================================

void pantallaInicio(){
  lcd.setCursor(0,0);
  lcd.print("BASPLO! El juego");
  lcd.setCursor(0,1);
  lcd.print("Por F.Belaza");
  delay(1500);  
}
