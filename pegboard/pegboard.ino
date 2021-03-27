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
bool repuso_Pieza;

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
State* S5 = machine.addState(&state5);
State* S5bis = machine.addState(&state5bis);
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
  S4->addTransition(&transitionS4S5,S5);
  S5->addTransition(&transitionS5S5bis,S5bis);
  S5bis->addTransition(&transitionS5bisS5,S5);
  S5bis->addTransition(&transitionS5bisS2,S2);
  //Agrego las transiciones intermedias para leer entrada serie
  S0->addTransition(&transitionS0S1,S1);
  S0->addTransition(&transitionS0S2,S2);
  S0->addTransition(&transitionS0S2bis,S2bis);
  S0->addTransition(&transitionS0S3,S3);
  S0->addTransition(&transitionS0S3bis,S3bis);
  S0->addTransition(&transitionS0S5,S5);
  S0->addTransition(&transitionS0S5bis,S5bis);
  S1->addTransition(&transitionS1S0,S0);
  S2->addTransition(&transitionToS0,S0);
  S2bis->addTransition(&transitionToS0,S0);
  S3->addTransition(&transitionToS0,S0);
  S3bis->addTransition(&transitionToS0,S0);
  S5->addTransition(&transitionToS0,S0);
  S5bis->addTransition(&transitionToS0,S0);
}

void enviarResultados(){
  // int siguiente_pos_disponible = CANTIDAD_LUGARES_MAXIMOS - lugares_disponibles;
  String piezas[CANTIDAD_COLUMNAS] = {"Circulo", "Triangulo", "Cuadrado", "Pentagono"};
  Serial.print("Pieza N");
  Serial.println(siguiente_pos_disponible);
  Serial.print("Forma levantada: ");
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
  if (siguiente_pos_disponible == CANTIDAD_LUGARES_MAXIMOS){
    Serial.println("Fin del proceso...");
  }
  Serial.println("-------------------------------------");
}

void prepararSiguienteCiclo(){
  //Elije proximo led
  elegirProximaFilaYColumna(); //queda en variables globales target_
  //Muestrea el tiempo de inicio de todo el ciclo
  Serial.println("--------------------------");
  if (siguiente_pos_disponible == 0){
    Serial.println("Ciclo Iniciado");
  }
  tiempo_inicial = millis();
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
    prepararSiguienteCiclo();
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
  pieza_levantada = levantoDelContenedor();
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
    siguiente_pos_disponible++;
    estado_anterior = "S4";
  }
}

bool transitionS4S0(){
  if (siguiente_pos_disponible != CANTIDAD_LUGARES_MAXIMOS) return false;
  resetTablero();
  estado_anterior = "S0";
  return true;
}

bool transitionS4S5(){
  if (siguiente_pos_disponible == CANTIDAD_LUGARES_MAXIMOS) return false;
  Serial.print("Lugares usados: ");
  Serial.println(siguiente_pos_disponible);
  return true;
}
//------------------------
void state5(){
  if(machine.executeOnce){
    apagarLeds();
    //Encender LED
    digitalWrite(Filas[CANTIDAD_FILAS-1], LOW); //CATODO
    digitalWrite(Columna_Anodos[pieza_levantada], HIGH);
    ultima_transicion = millis();
    estado_anterior = "S5";
  }
}
bool transitionS5S5bis(){
  if (millis() - ultima_transicion > TIEMPO_POV_ON_MS){
    return true;
  }
  return false;
}
//------------------------
void state5bis(){
  if(machine.executeOnce){
    apagarLeds();
    //Referencia para POV
    ultima_transicion = millis();
    estado_anterior = "S5bis";
  }
  /*
  while (1){
    repuso_Pieza = insertoPiezaLevantada();
    if (repuso_Pieza){
      Serial.println("Contenedor leido. Repuso pieza");
    }else{
      Serial.println("Contenedor leido. NO repuso pieza");
    }
  }
  */
 repuso_Pieza = false;
 if (!leerTablero()){
  repuso_Pieza = insertoPiezaLevantada();
 }
}

bool transitionS5bisS5(){
  if (millis() - ultima_transicion > TIEMPO_POV_OFF_MS)
    return true;
  return false;
}

bool transitionS5bisS2(){
  if (!repuso_Pieza)
    return false;
  //else
  //si se inserta en la columna de la pieza levantada
  prepararSiguienteCiclo();
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
bool transitionS0S5(){
  if (es_comando) return false;
  if (estado_anterior == "S5") return true;
  return false;
}
bool transitionS0S5bis(){
  if (es_comando) return false;
  if (estado_anterior == "S5bis") return true;
  return false;
}
bool transitionS1S0(){
  if (!es_comando || entrada == COMANDO_FIN){
    apagarLeds();
    if (entrada == COMANDO_FIN){
      analizar = false;
      entrada = "";
      Serial.println("Ciclo abortado");
    }
    return true;
  }
  return false;
  }
bool transitionToS0(){
  if (Serial.available() > 0) return true;
  return false;
}
