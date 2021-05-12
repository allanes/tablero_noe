// pegboard-test
// Pruebas de integridad del hardware
//
// Este sketch propone una prueba secuencial de lectura de piezas 
// insertadas sobre la fila contenedora de piezas.
// 
// La configuración debe ser la del sketch principal y debe 
// hacer uso de las declaraciones dadas en tablero.h.
//
//

#include <Arduino.h>
#include "tablero.h"

extern void inicializarTablero();
void encenderYEsperar();
bool repuso_Pieza;
int cuenta;

void setup(){
  delay(500);
  Serial.begin(115200);
  Serial.println("Iniciando pegboard-test");
  inicializarTablero();
}

void loop(){
  Serial.println("Inicio del Test");
  Serial.println("Fila contenedora");
  Serial.print("Prueba retorno de carro");
  Serial.print("\r");
  Serial.print("Actualizando linea");
  Serial.print("\n");
  //---------------------------------
  int j = 0;
  while (1){
    insercion = leerContenedorPosicion(j);
    Serial.print(insercion);
    Serial.print(" ");
    j++;
    if (j == CANTIDAD_COLUMNAS){
      j = 0;
      Serial.print("\r");
    }
  }
  //---------------------------------
  Serial.println("Fin del Test");
}
