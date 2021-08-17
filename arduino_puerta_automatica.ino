#include <Servo.h>

//asigno nombre a cada puerto para no tratarlo por su numero
const int verde = 2,rojo = 3, buzzer = 4, echo = 5, trig = 6;
//Defino que puerta es un servo motor
Servo puerta;

//apertura es para saber cuantos grados esta abierto el servo motor (que tan abierta esta la puerta) y prendido para volver intermitente al buzzer
int apertura = 0;
bool prendido = false;

//Estas variables determinan la entrada en la tabla de trancisiones
bool abierta = false;
bool espera = false;
int proximidad;
int laser;
  
// fila y estado tambien sirven en la tabla de trancisiones, pero para determinar el nuevo estado
int fila, estado = 0;

//setup se ejecuta 1 sola vez al inicio del programa
void setup(){

  Serial.begin(9600);
  pinMode(verde, OUTPUT);
  pinMode(rojo, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(A0, INPUT);
  puerta.attach(8);
}

int tabla_trancision[16][4] = {{0,1,0,2},
                               {0,1,0,2},
                               {0,0,2,2},
                               {2,0,2,2},
                               {0,1,0,3},
                               {0,1,0,3},
                               {0,0,3,3},
                               {2,0,3,3},
                               {1,1,0,2},
                               {1,1,0,2},
                               {0,0,2,2},
                               {2,0,2,2},
                               {1,1,0,3},
                               {1,1,0,3},
                               {0,0,3,3},
                               {2,0,3,3}};

//salida determina segun el estado, que es lo proximo a hacer
void salida(int accion){
  switch(accion){
    case 0:
      puerta_quieta();
      break;
    case 1:
      abrir_puerta();
      break;
    case 2:
      cerrar_puerta();
      break;
    case 3:
      advertir_obstaculo();
      break;
  }
}

//entrada segun los valores de las variables incluidas devuelve un valor entre 0 y 15, que indicaran la fila de la tabla de transiciones que caerá el programa
int entrada(){
  if    (proximidad > 50 && laser > 900 && abierta == false && espera == false){return 0;}
  else if (proximidad > 50 && laser > 900 && abierta == false && espera == true){return 1;}
  else if (proximidad > 50 && laser > 900 && abierta == true && espera == false){return 2;}
  else if (proximidad > 50 && laser > 900 && abierta == true && espera == true){return 3;}
  else if (proximidad > 50 && laser <= 900 && abierta == false && espera == false){return 4;}
  else if (proximidad > 50 && laser <= 900 && abierta == false && espera == true){return 5;}
  else if (proximidad > 50 && laser <= 900 && abierta == true && espera == false){return 6;}
  else if (proximidad > 50 && laser <= 900 && abierta == true && espera == true){return 7;}
  else if (proximidad <= 50 && laser > 900 && abierta == false && espera == false){return 8;}
  else if (proximidad <= 50 && laser > 900 && abierta == false && espera == true){return 9;}
  else if (proximidad <= 50 && laser > 900 && abierta == true && espera == false){return 10;}
  else if (proximidad <= 50 && laser > 900 && abierta == true && espera == true){return 11;}
  else if (proximidad <= 50 && laser <= 900 && abierta == false && espera == false){return 12;}
  else if (proximidad <= 50 && laser <= 900 && abierta == false && espera == true){return 13;}
  else if (proximidad <= 50 && laser <= 900 && abierta == true && espera == false){return 14;}
  else if (proximidad <= 50 && laser <= 900 && abierta == true && espera == true){return 15;}
  
}

//Esta es la funcion principal, marca el flujo del programa
void loop(){
  escencial();
  fila = entrada();
  estado = tabla_trancision[fila][estado];
  salida(estado);
  
}

// escencial esta hecha para poder leer los valores analogicos (para saber la distancia de alguien cercano al sensor ultra sonico y para ver la luminocidad captada por la fotoresistencia)
void escencial(){
  digitalWrite(trig,HIGH);
    delay(1);
    digitalWrite(trig,LOW);
    proximidad = pulseIn(echo,HIGH)/58.2;
    laser  =analogRead(A0);
    Serial.println(laser);
}

void abrir_puerta(){
    apertura = 180;
    puerta.write(apertura);
  digitalWrite(verde, HIGH);
    tone(buzzer,200,1000);
    abierta = true;
    delay(1000);
}

//las proximas funciones son las posibles salidas del programa segun el estado en el que se encuentra

void puerta_quieta(){
  digitalWrite(verde, LOW);
  digitalWrite(rojo, LOW);
  if(abierta){
    delay(3000);
    espera = true;
  }else{
    espera = false;
  }
  
}

void cerrar_puerta(){
  apertura -= 18;
    puerta.write(apertura);
    if(apertura < 0){
      abierta = false;
    }
    delay(100);
}

void advertir_obstaculo(){
  if(!prendido){
    tone(buzzer,200,1000);
    prendido = true;
  }else{
    digitalWrite(buzzer,LOW);
    prendido = false;
  }
  digitalWrite(rojo,HIGH);
    delay(1000);
}
