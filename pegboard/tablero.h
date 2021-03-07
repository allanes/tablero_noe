
#include <Arduino.h>
#include <String.h>

// ensure this library description is only included once
#ifndef tablero_h
#define tablero_h
#endif
//Dimensiones del tablero
#define CANTIDAD_FILAS 5
#define CANTIDAD_COLUMNAS 4
//Tiempos de POV [ms]
#define TIEMPO_POV_ON_MS 10
#define TIEMPO_POV_OFF_MS 10

//---------Variables globales----------
//Analizador léxico
extern bool analizar;
extern String entrada;
extern bool reset_sistema;
extern bool es_comando;
//Comandos y manejo de comandos
#define COMANDO_INICIO "START"
#define COMANDO_FIN "STOP"
extern String recibido;
//Tiempos y manejo de tiempos
extern unsigned long tiempo_inicial, tiempo_en_alzar, tiempo_final;
extern unsigned long ultima_transicion;
//Manejo del tablero
extern int target_fila, target_columna;
extern int tablero_usado[CANTIDAD_FILAS][CANTIDAD_COLUMNAS];
extern int lugares_disponibles;
extern int insercion_columna, insercion_fila;
extern bool insercion;
extern int pieza_levantada;
//Conexiones de GPIO
extern byte Fila2;//PL3;
extern byte Fila1;//PL1; // Fila de arriba
extern byte Fila3;//PL5;
extern byte Fila4;//PL7; // Fila de abajo
extern byte Fila5; // Contenedor de fichas
extern byte ColumnaA; // Columna más a la izquierda
extern byte ColumnaB;
extern byte ColumnaC;
extern byte ColumnaD; // Columna más a la derecha
extern byte AnodosColumna1;
extern byte AnodosColumna2;
extern byte AnodosColumna3;
extern byte AnodosColumna4;
extern byte Filas[CANTIDAD_FILAS];
extern byte Columnas[CANTIDAD_COLUMNAS];
extern byte Columna_Anodos[CANTIDAD_COLUMNAS];

//------Funciones-------
void encenderLeds();
void apagarLeds();
void encenderLed();
int leerContenedor();
bool leerTablero();
void inicializarTablero();
void elegirProximaFilaYColumna();