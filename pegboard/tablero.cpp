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
int target_fila=0, target_columna=-1;
int insercion_columna, insercion_fila;
int insercion;
int pieza_levantada = -1;
int tablero_usado[CANTIDAD_FILAS][CANTIDAD_COLUMNAS] = {0};
int lugares_disponibles = CANTIDAD_LUGARES_MAXIMOS;
int siguiente_pos_disponible = 0;
//Conexiones de GPIO
byte Fila2=46;//PL3;
byte Fila1=48;//PL1; // Fila de arriba
byte Fila3=44;//PL5;
byte Fila4=42;//PL7; // Fila de abajo
byte Fila5=40; // Contenedor de fichas
byte ColumnaTableroA=38; // Columna más a la izquierda
byte ColumnaTableroB=36;
byte ColumnaTableroC=34;
byte ColumnaTableroD=32; // Columna más a la derecha
byte ColumnaDepositoA=4; // Columna más a la izquierda
byte ColumnaDepositoB=5;
byte ColumnaDepositoC=6;
byte ColumnaDepositoD=7; // Columna más a la derecha
byte AnodosColumna1 = 30;
byte AnodosColumna2 = 28;
byte AnodosColumna3 = 26;
byte AnodosColumna4 = 24;
byte Filas[CANTIDAD_FILAS]={    Fila1,
                                Fila2,
                                Fila3,
                                Fila4,
                                Fila5};
byte Columnas_Tablero[CANTIDAD_COLUMNAS]={  ColumnaTableroA,
                                    ColumnaTableroB,
                                    ColumnaTableroC,
                                    ColumnaTableroD};
byte Columna_Anodos[CANTIDAD_COLUMNAS] = {  AnodosColumna1, 
                                            AnodosColumna2, 
                                            AnodosColumna3, 
                                            AnodosColumna4};
byte Columnas_Deposito[CANTIDAD_COLUMNAS] = {  ColumnaDepositoA,
                                              ColumnaDepositoB,
                                              ColumnaDepositoC,
                                              ColumnaDepositoD};


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

bool insertoPiezaLevantada(){
  bool ret = false;

  digitalWrite(Filas[CANTIDAD_FILAS - 1], HIGH);
  ret = digitalRead(Columnas_Deposito[pieza_levantada]);
  digitalWrite(Filas[CANTIDAD_FILAS - 1], LOW);
  return ret;
}

int levantoDelContenedor(){
  int ret = -1;
  //Enciendo los LEDs transmisores IR
  digitalWrite(Filas[CANTIDAD_FILAS - 1], HIGH);
  //Leo colector 
  for (int col = 0; col < CANTIDAD_COLUMNAS; col++){
    //Leo el colector del fototransistor columna por columna
    if (digitalRead(Columnas_Deposito[col]) == LOW){
      ret = col;
      break;
    }
  }
  digitalWrite(Filas[CANTIDAD_FILAS - 1], LOW);
  return ret;
}

bool leerTablero(){
  bool ret = false;
  
  for (int i = 0; i < CANTIDAD_FILAS - 1; i++){
    digitalWrite(Filas[i], LOW);
  }
  // for (int j = 0; j < CANTIDAD_COLUMNAS; j++){
  //   digitalWrite(Columna_Anodos[j], LOW);
  // }  
  for(int i = 0; i < CANTIDAD_FILAS - 1; i++){
    digitalWrite(Filas[i], HIGH); //Filas = Catodos
    for(int j = 0; j < CANTIDAD_COLUMNAS; j++){
      if(digitalRead(Columnas_Tablero[j])){ //si es True, insertó pieza
        insercion_fila = i;
        insercion_columna = j;
        digitalWrite(Filas[i], LOW);
        return true;
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
    pinMode(ColumnaTableroA,INPUT);  
    pinMode(ColumnaTableroB,INPUT);  
    pinMode(ColumnaTableroC,INPUT);  
    pinMode(ColumnaTableroD,INPUT);
    pinMode(ColumnaDepositoA,INPUT_PULLUP);  
    pinMode(ColumnaDepositoB,INPUT_PULLUP);  
    pinMode(ColumnaDepositoC,INPUT_PULLUP);  
    pinMode(ColumnaDepositoD,INPUT_PULLUP);
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

void elegirProximaFilaYColumnaSecuencial(){
  target_columna++;
  if (target_columna == CANTIDAD_COLUMNAS){
    target_columna = 0;
    target_fila++;
    if (target_fila == CANTIDAD_FILAS){
      target_fila = 0;
    }
  }
  Serial.print("Posicion ");
  Serial.print(target_fila);
  Serial.print(", ");
  Serial.println(target_columna);
}

void resetTablero(){
  //Reseteo el tablero_usado para volver a empezar
    // lugares_disponibles = CANTIDAD_LUGARES_MAXIMOS;
    siguiente_pos_disponible = 0;
    for (int i=0; i<CANTIDAD_FILAS-1; i++){
      for (int j=0; j<CANTIDAD_COLUMNAS; j++){
        tablero_usado[i][j] = 0;
      }
    }
    encenderLeds();
}