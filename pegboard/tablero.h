
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
//Tiempo para mover tablero, por fila [ms]
//(el tiempo total que demora es 4 * TIEMPO_FILA)
#define TIEMPO_FILA 2000UL

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
extern int siguiente_pos_disponible;
extern int insercion_columna, insercion_fila;
extern int insercion;
extern int pieza_levantada;
enum {BUSCA_LEVANTADO, BUSCA_INSERCION};
#define CANTIDAD_LUGARES_MAXIMOS ((CANTIDAD_FILAS - 1) * CANTIDAD_COLUMNAS)
//Conexiones de GPIO
extern byte Fila2;//PL3;
extern byte Fila1;//PL1; // Fila de arriba
extern byte Fila3;//PL5;
extern byte Fila4;//PL7; // Fila de abajo
extern byte Fila5; // Contenedor de fichas
extern byte ColumnaTableroA; // Columna más a la izquierda
extern byte ColumnaTableroB;
extern byte ColumnaTableroC;
extern byte ColumnaTableroD; // Columna más a la derecha
extern byte ColumnaDepositoA; // Columna más a la izquierda
extern byte ColumnaDepositoB;
extern byte ColumnaDepositoC;
extern byte ColumnaDepositoD; // Columna más a la derecha
extern byte AnodosColumna1;
extern byte AnodosColumna2;
extern byte AnodosColumna3;
extern byte AnodosColumna4;
extern byte Filas[CANTIDAD_FILAS];
extern byte Columnas_Tablero[CANTIDAD_COLUMNAS];
extern byte Columna_Anodos[CANTIDAD_COLUMNAS];
extern byte Columnas_Deposito[CANTIDAD_COLUMNAS];

//------Funciones-------
void encenderLeds();
void apagarLeds();
void encenderLed();
void encenderFilas();
int levantoDelContenedor();
bool insertoPiezaLevantada();
bool leerTablero();
void inicializarTablero();
void elegirProximaFilaYColumna();
void elegirProximaFilaYColumnaSecuencial();
void resetTablero();