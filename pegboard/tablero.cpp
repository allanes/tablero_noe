#include "tablero.h"

//---------Variables globales----------
//Analizador léxico
bool analizar;
String entrada = "";
bool reset_sistema = false;
bool es_comando;
//Comandos y manejo de comandos
#define COMANDO_INICIO "START"
#define COMANDO_FIN "STOP"
String recibido = "";
//Tiempos y manejo de tiempos
unsigned long tiempo_inicial, tiempo_en_alzar, tiempo_final;
unsigned long ultima_transicion;
//Manejo del tablero
int target_fila, target_columna;
int insercion_columna, insercion_fila;
bool insercion;
int pieza_levantada = -1;
int tablero_usado[CANTIDAD_FILAS][CANTIDAD_COLUMNAS] = {0};
#define CANTIDAD_LUGARES_MAXIMOS ((CANTIDAD_FILAS - 1) * CANTIDAD_COLUMNAS)
int lugares_disponibles = CANTIDAD_LUGARES_MAXIMOS;
//Conexiones de GPIO
byte Fila2=46;//PL3;
byte Fila1=48;//PL1; // Fila de arriba
byte Fila3=44;//PL5;
byte Fila4=42;//PL7; // Fila de abajo
byte Fila5=40; // Contenedor de fichas
byte ColumnaA=38; // Columna más a la izquierda
byte ColumnaB=36;
byte ColumnaC=34;
byte ColumnaD=32; // Columna más a la derecha
byte AnodosColumna1 = 30;
byte AnodosColumna2 = 28;
byte AnodosColumna3 = 26;
byte AnodosColumna4 = 24;
byte Filas[CANTIDAD_FILAS]={    Fila1,
                                Fila2,
                                Fila3,
                                Fila4,
                                Fila5};
byte Columnas[CANTIDAD_COLUMNAS]={  ColumnaA,
                                    ColumnaB,
                                    ColumnaC,
                                    ColumnaD};
byte Columna_Anodos[CANTIDAD_COLUMNAS] = {  AnodosColumna1, 
                                            AnodosColumna2, 
                                            AnodosColumna3, 
                                            AnodosColumna4};

void encenderLeds(){
  for (int i = 0; i < CANTIDAD_FILAS; i++){
    digitalWrite(Filas[i], LOW);
    for (int j = 0; j < CANTIDAD_COLUMNAS; j++){
      digitalWrite(Columna_Anodos[j], HIGH);
      delay(50);
      digitalWrite(Columna_Anodos[j], LOW);
    }
    digitalWrite(Filas[i], HIGH);
  }
}

void encenderCantidadUsados(){  
  int lugares_usados;
  for (int k=0; k<2; k++){
    //lugares_disponibles es 16 al principio
    lugares_usados = CANTIDAD_LUGARES_MAXIMOS - lugares_disponibles - 1;
    digitalWrite(Filas[lugares_usados / (CANTIDAD_FILAS - 1)], LOW);
    digitalWrite(Columna_Anodos[lugares_usados % CANTIDAD_COLUMNAS], HIGH);
    delay(250);
    apagarLeds();
    delay(150);
  }
}

void apagarLeds(){
  //Apaga todos los LEDS
  for (int i=0; i<CANTIDAD_FILAS; i++){
    digitalWrite(Filas[i], HIGH);
  }
  for (int j=0; j<CANTIDAD_COLUMNAS; j++){
    digitalWrite(Columna_Anodos[j], LOW);
  }
}

void encenderLed(){
  apagarLeds();
  //Prende el que va
  digitalWrite(Filas[target_fila], LOW);
  digitalWrite(Columna_Anodos[target_columna], HIGH);
}

int leerContenedor(){
  int ret = -1;
  int fila_contenedora = CANTIDAD_FILAS - 1;
  digitalWrite(Filas[fila_contenedora], HIGH);
  for (int col = 0; col < CANTIDAD_COLUMNAS - 1; col++){
  // pythfor (int col = 0; col < 1; col++){
    if (digitalRead(Columnas[col]) == false){ //Estan todas las piezas puestas y cuando levanta una se va a LOW esa columna
      Serial.print("Levanto PIEZA ");
      Serial.println(col);
      ret = col;
      break;
    }
  }
  return ret;
}

bool leerTablero(){
  bool ret = false;
  for (int i = 0; i < CANTIDAD_FILAS; i++){
    digitalWrite(Filas[i], LOW);
  }
  for (int j = 0; j < CANTIDAD_COLUMNAS; j++){
    digitalWrite(Columna_Anodos[j], LOW);
  }
  for(int i = 0; i < CANTIDAD_FILAS - 1; i++){
    digitalWrite(Filas[i], HIGH);
    for(int j = 0; j < CANTIDAD_COLUMNAS; j++){
      if(digitalRead(Columnas[j])){ //si es True, insertó pieza
        ret = true;
        insercion_fila = i;
        insercion_columna = j;
      }
    }
    digitalWrite(Filas[i], LOW);
  }
  return ret;
}

void inicializarTablero(){
    pinMode(Fila1,OUTPUT);
    pinMode(Fila2,OUTPUT);
    pinMode(Fila3,OUTPUT);
    pinMode(Fila4,OUTPUT);
    pinMode(Fila5,OUTPUT);
    digitalWrite(Fila1,HIGH);
    digitalWrite(Fila2,HIGH);
    digitalWrite(Fila3,HIGH);
    digitalWrite(Fila4,HIGH);
    digitalWrite(Fila5,HIGH);
    //mio
    pinMode(AnodosColumna1, OUTPUT);
    pinMode(AnodosColumna2, OUTPUT);
    pinMode(AnodosColumna3, OUTPUT);
    pinMode(AnodosColumna4, OUTPUT);
    digitalWrite(AnodosColumna1, LOW);
    digitalWrite(AnodosColumna2, LOW);
    digitalWrite(AnodosColumna3, LOW);
    digitalWrite(AnodosColumna4, LOW);
    //-----
    pinMode(ColumnaA,INPUT);  
    pinMode(ColumnaB,INPUT);  
    pinMode(ColumnaC,INPUT);  
    pinMode(ColumnaD,INPUT);

    //Enciende todos los LEDs. Sirve para comprobar que todos funcionen
    encenderLeds();
    //Para la funcion random
    pinMode(A0, INPUT);
}

void elegirProximaFilaYColumna(){
  do{
    target_fila = (int)random(0, CANTIDAD_FILAS - 1);
    target_columna = (int)random(0, CANTIDAD_COLUMNAS);
  }while(tablero_usado[target_fila][target_columna] == 1);
  
  tablero_usado[target_fila][target_columna] = 1;
}

void resetTablero(){
  //Reseteo el tablero_usado para volver a empezar
    lugares_disponibles = CANTIDAD_LUGARES_MAXIMOS;
    for (int i=0; i<CANTIDAD_FILAS-1; i++){
      for (int j=0; j<CANTIDAD_COLUMNAS; j++){
        tablero_usado[i][j] = 0;
      }
    }
    encenderLeds();
}