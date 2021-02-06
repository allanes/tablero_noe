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

#include <StateMachine.h>


#define CANTIDAD_FILAS 5
#define CANTIDAD_COLUMNAS 4
#define TIEMPO_POV_ON_MS 10
#define TIEMPO_POV_OFF_MS 10

byte Fila1=48;//PL1; // Fila de arriba
byte Fila2=46;//PL3;
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
byte Filas[CANTIDAD_FILAS]={Fila1,Fila2,Fila3,Fila4,Fila5};
byte Columnas[CANTIDAD_COLUMNAS]={ColumnaA,ColumnaB,ColumnaC,ColumnaD};
byte Columna_Anodos[CANTIDAD_COLUMNAS] = {AnodosColumna1, AnodosColumna2, AnodosColumna3, AnodosColumna4};

String comando_inicio = "START";
String comando_fin = "STOP";
String recibido;
unsigned long time;
int target_fila, target_columna;
int insercion_columna, insercion_fila;
bool insercion;
int pieza_levantada = -1;
unsigned long tiempo_inicial, tiempo_en_alzar, tiempo_final;
unsigned long ultima_transicion;
//Para analizador léxico
bool analizar;
String comando_a_analizar = "";
bool reset_sistema = false;


//Para la MEF
StateMachine machine = StateMachine();
//Estados
State* S0 =     machine.addState(&state0);
State* S1 =     machine.addState(&state1);
State* S1bis =  machine.addState(&state1bis);
State* S2 =     machine.addState(&state2);
State* S2bis =  machine.addState(&state2bis);
State* S3 =     machine.addState(&state3);
State* estado_anterior = S0;

// the setup function runs once when you press reset or power the board
void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.println("Iniciando pegboard-test");
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
  //Le paso una entrada a la funcion random
  pinMode(A0, INPUT);
  randomSeed(analogRead(A0));
  //----
  Serial.println("Puertos iniciados");
  Serial.println("Esperando comando (START/stop)...");
  //Limpia buffer entrada
  while (Serial.available()) Serial.read();
  //Transiciones de la MEF
  S0->addTransition(&transitionS0S1,S1);
  S1->addTransition(&transitionS1S1bis,S1bis);
  S1bis->addTransition(&transitionS1bisS1,S1);
  S1bis->addTransition(&transitionS1bisS2,S2);
  S2->addTransition(&transitionS2S2bis,S2bis);
  S2bis->addTransition(&transitionS2bisS2,S2);
  S2bis->addTransition(&transitionS2bisS3,S3);
  S3->addTransition(&transitionS3S0,S0);
}

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

int leerContenedor(){
  int ret = -1;
  int fila_contenedora = CANTIDAD_FILAS - 1;
  digitalWrite(Filas[fila_contenedora], HIGH);
  // for (int col = 0; col < CANTIDAD_COLUMNAS - 1; col++){
  for (int col = 0; col < 1; col++){
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
  if (machine.executeOnce){
    Serial.println("-------------------------------------");
  }
  //Leer puerto serie
  while (Serial.available() > 0){
    char caracter = Serial.read();
    if (caracter != '\n'){
      recibido += caracter;
      analizar = false;
    }else {
      analizar = true;
      comando_a_analizar = recibido;
      recibido = "";
    } 
  }
}

bool transitionS0S1(){
  if (!analizar) return false;
  if (comando_a_analizar.equals(comando_inicio)){
    comando_a_analizar = "";
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
void state1(){
  if(machine.executeOnce){
    //Enciende Led Elegido
    encenderLed();
    //Toma una referencia para POV
    ultima_transicion = millis();
  }
}

bool transitionS1S1bis(){
  if (millis() - ultima_transicion > TIEMPO_POV_ON_MS){
    return true;
  }
  return false;
}
//-------------------------
void state1bis(){
  if(machine.executeOnce){
    apagarLeds();
    //Referencia para POV
    ultima_transicion = millis();
  }
  pieza_levantada = leerContenedor();
}

bool transitionS1bisS1(){
  if (millis() - ultima_transicion > TIEMPO_POV_OFF_MS){
    return true;
  }
  return false;
}

bool transitionS1bisS2(){
  if (pieza_levantada == -1)
    return false;

  //Muestrea tiempo que tardó en alzar la pieza
  tiempo_en_alzar = millis();
  return true;
}

//-------------------------
void state2(){
  if(machine.executeOnce){
    //Enciende Led Elegido
    encenderLed();
    //Referencia para POV
    ultima_transicion = millis();
  }
}

bool transitionS2S2bis(){
  if (millis() - ultima_transicion > TIEMPO_POV_ON_MS){
    return true;
  }
  return false;
}
//------------------------
void state2bis(){
  if(machine.executeOnce){
    apagarLeds();
    //Referencia para POV
    ultima_transicion = millis();
  }
  insercion = leerTablero();  
}

bool transitionS2bisS3(){
  if (insercion){
    //Muestrea el tiempo final de insercion de la pieza
    tiempo_final = millis();
    return true;
  }
  return false;
}

bool transitionS2bisS2(){
  if (millis() - ultima_transicion > TIEMPO_POV_OFF_MS)
    return true;
  return false;
}
//------------------------
void state3(){
  if(machine.executeOnce){
    apagarLeds();
    enviarResultados();
  }
}

bool transitionS3S0(){
  return true;
}
