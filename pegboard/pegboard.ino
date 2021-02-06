// pegboard
// Controlador del pegboard
//
// Gustavo Enrique Jiménez
// gejimenez@gmail.com
// ILAV - CONICET - 2020


// 10/03/2020 - Agregado de cables para encender los LEDs. El multiplexado 
//              deja los LEDs ligeramente encendidos, así que se agrega un 
//              cable para cada target_columna de LEDs y el software para
//              encenderlos.
// 06/03/2020 - Lectura de posición de fichas
// 04/03/2020 - Comienzo a escribir el programa

#include <Arduino.h>
#include "tablero.h"
#include <StateMachine.h>

extern void inicializarTablero();

//Para la MEF
StateMachine machine = StateMachine();
//Estados
State* S0 =     machine.addState(&state0);
State* S1 =     machine.addState(&state1);
State* S2 =     machine.addState(&state2);
State* S2bis =  machine.addState(&state2bis);
State* S3 =     machine.addState(&state3);
State* S3bis =  machine.addState(&state3bis);
State* S4 =  machine.addState(&state4);
String estado_anterior = "S0";

// the setup function runs once when you press reset or power the board
void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.println("Iniciando pegboard-test");
  inicializarTablero();
  
  //Le paso una entrada a la funcion random  
  randomSeed(analogRead(A0));
  //----
  Serial.println("Puertos iniciados");
  Serial.println("Esperando comando (START/stop)...");
  //Limpia buffer entrada
  while (Serial.available()) Serial.read();
  //Transiciones de la MEF
  S1->addTransition(&transitionS1S2,S2);
  S2->addTransition(&transitionS2S2bis,S2bis);
  S2bis->addTransition(&transitionS2bisS2,S2);
  S2bis->addTransition(&transitionS2bisS3,S3);
  S3->addTransition(&transitionS3S3bis,S3bis);
  S3bis->addTransition(&transitionS3bisS3,S3);
  S3bis->addTransition(&transitionS3bisS4,S4);
  S4->addTransition(&transitionS4S0,S0);
  //Agrego las transiciones intermedias para leer entrada serie
  S0->addTransition(&transitionS0S1,S1);
  S0->addTransition(&transitionS0S2,S2);
  S0->addTransition(&transitionS0S2bis,S2bis);
  S0->addTransition(&transitionS0S3,S3);
  S0->addTransition(&transitionS0S3bis,S3bis);
  S1->addTransition(&transitionS1S0,S0);
  S2->addTransition(&transitionToS0,S0);
  S2bis->addTransition(&transitionToS0,S0);
  S3->addTransition(&transitionToS0,S0);
  S3bis->addTransition(&transitionToS0,S0);
}

// extern void encenderLeds();
// extern void apagarLeds();
// extern void encenderLed();
// extern int leerContenedor();
// extern bool leerTablero();



void enviarResultados(){
  String piezas[CANTIDAD_COLUMNAS] = {"Circulo", "Triangulo", "Cuadrado", "Pentagono"};
  
  Serial.print("Pieza levantada: ");
  Serial.println(piezas[pieza_levantada]);
  Serial.print("Fila de insercion: ");
  Serial.println(insercion_fila);
  Serial.print("Columna de insercion: ");
  Serial.println(insercion_columna);
  Serial.print("Tiempo al iniciar: ");
  Serial.println(tiempo_inicial);
  Serial.print("Tiempo al alzar: ");
  Serial.println(tiempo_en_alzar);
  Serial.print("Tiempo al final: ");
  Serial.println(tiempo_final);
  Serial.print("\nDemora en alzar: ");
  Serial.println(tiempo_en_alzar - tiempo_inicial);
  Serial.print("Demora en insertar desde alzado: ");
  Serial.println(tiempo_final - tiempo_en_alzar);
  Serial.print("Demora total: ");
  Serial.println(tiempo_final - tiempo_inicial);
  Serial.println("-------------------------------------");
}


void loop(){
  machine.run();
}
//=======================================
void state0(){
  while (Serial.available() > 0){
    char caracter = Serial.read();
    if (caracter != '\n'){
      recibido += caracter;
      analizar = false;
    }else {
      analizar = true;
      entrada = recibido;
      recibido = "";
    } 
  }
}

void state1(){
  if (machine.executeOnce){
    //Procesa la entrada
    es_comando = false;
    if (entrada == COMANDO_INICIO || entrada == COMANDO_FIN){
      es_comando = true;
    }
    estado_anterior = "S0";
  }
  //Leer puerto serie
  
}

bool transitionS1S2(){
  if (!es_comando) return false;

  if (entrada == COMANDO_INICIO){
    entrada = "";
    analizar = false;
    //Elije proximo led
    target_fila = (int)random(0, CANTIDAD_FILAS - 2);
    target_columna = (int)random(0, CANTIDAD_COLUMNAS - 1);
    //Muestrea el tiempo de inicio de todo el ciclo
    Serial.println("Ciclo Iniciado");
    tiempo_inicial = millis();
    return true;
  }
  return false;
}
//-------------------------
void state2(){
  if(machine.executeOnce){
    //Enciende Led Elegido
    encenderLed();
    //Toma una referencia para POV
    ultima_transicion = millis();
    estado_anterior = "S2";
  }
}

bool transitionS2S2bis(){
  if (millis() - ultima_transicion > TIEMPO_POV_ON_MS){
    return true;
  }
  return false;
}
//-------------------------
void state2bis(){
  if(machine.executeOnce){
    apagarLeds();
    //Referencia para POV
    ultima_transicion = millis();
    estado_anterior = "S2bis";
  }
  pieza_levantada = leerContenedor();
}

bool transitionS2bisS2(){
  if (millis() - ultima_transicion > TIEMPO_POV_OFF_MS){
    return true;
  }
  return false;
}

bool transitionS2bisS3(){
  if (pieza_levantada == -1)
    return false;

  //Muestrea tiempo que tardó en alzar la pieza
  tiempo_en_alzar = millis();
  return true;
}

//-------------------------
void state3(){
  if(machine.executeOnce){
    //Enciende Led Elegido
    encenderLed();
    //Referencia para POV
    ultima_transicion = millis();
    estado_anterior = "S3";
  }
}

bool transitionS3S3bis(){
  if (millis() - ultima_transicion > TIEMPO_POV_ON_MS){
    return true;
  }
  return false;
}
//------------------------
void state3bis(){
  if(machine.executeOnce){
    apagarLeds();
    //Referencia para POV
    ultima_transicion = millis();
    estado_anterior = "S3bis";
  }
  insercion = leerTablero();  
}

bool transitionS3bisS4(){
  if (insercion){
    //Muestrea el tiempo final de insercion de la pieza
    tiempo_final = millis();
    return true;
  }
  return false;
}

bool transitionS3bisS3(){
  if (millis() - ultima_transicion > TIEMPO_POV_OFF_MS)
    return true;
  return false;
}
//------------------------
void state4(){
  if(machine.executeOnce){
    apagarLeds();
    enviarResultados();
    estado_anterior = "S0";
  }
}

bool transitionS4S0(){
  return true;
}

//-------------------------
bool transitionS0S1(){
  return analizar;
}
bool transitionS0S2(){
  if (es_comando) return false;
  if (estado_anterior == "S2") return true;
  return false;
  }
bool transitionS0S2bis(){
  if (es_comando) return false;
  if (estado_anterior == "S2bis") return true;
  return false;
}
bool transitionS0S3(){
  if (es_comando) return false;
  if (estado_anterior == "S3") return true;
  return false;
}
bool transitionS0S3bis(){
  if (es_comando) return false;
  if (estado_anterior == "S3bis") return true;
  return false;
}
bool transitionS1S0(){
  if (!es_comando || entrada == COMANDO_FIN)
    return true;
  return false;
  }
bool transitionToS0(){
  if (Serial.available() > 0) return true;
  return false;
}
