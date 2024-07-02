#define pinMotorDireito_A 12  //am
#define pinMotorDireito_B 13  //az
#define pinMotorEsquerdo_A 10   //lj
#define pinMotorEsquerdo_B 11   //vd

#include <SoftwareSerial.h>

//Declaração das variaveis para Controle do Motor
int velocidade = 0;
bool sentido = 1;
int estado = 0;



void setup() {
  //Configura o estado das portas do Arduino
  pinMode(pinMotorDireito_A, OUTPUT);
  pinMode(pinMotorDireito_B, OUTPUT);
  pinMode(pinMotorEsquerdo_A, OUTPUT);
  pinMode(pinMotorEsquerdo_B, OUTPUT);


  digitalWrite(pinMotorDireito_A, LOW);
  digitalWrite(pinMotorDireito_B, LOW);
  digitalWrite(pinMotorEsquerdo_A, LOW);
  digitalWrite(pinMotorEsquerdo_B, LOW);
  


  //Inicia a comunicação serial para monitorar o que está acontecendo pelo monitor serial
  Serial.begin(9600);
}

void loop() {

  //Logica de controle de velocidade e sentido de rotação dos motores
  if (sentido) {
    velocidade += 2;
    if (velocidade >= 255) {
       velocidade = 255;
       sentido = 0;
       estado++;
       if (estado > 5) estado = 0;
    }
  } else {
    velocidade -= 2;
    if (velocidade <= 0) {
       velocidade = 0;
       sentido = 1;
       estado++;
       if (estado > 5) estado = 0;
    }
  }

  //Controle dos motores
  switch (estado) {
    case 0:
       analogWrite(pinMotorDireito_A, velocidade);
       analogWrite(pinMotorDireito_B, 0);
       analogWrite(pinMotorEsquerdo_A, velocidade);
       analogWrite(pinMotorEsquerdo_B, 0);
       break;
    case 1:
       analogWrite(pinMotorDireito_A, 0);
       analogWrite(pinMotorDireito_B, velocidade);
       analogWrite(pinMotorEsquerdo_A, 0);
       analogWrite(pinMotorEsquerdo_B, velocidade);
       break;
    case 2:
       analogWrite(pinMotorDireito_A, velocidade);
       analogWrite(pinMotorDireito_B, 0);
       analogWrite(pinMotorEsquerdo_A, 0);
       analogWrite(pinMotorEsquerdo_B, 0);
       break;
    case 3:
       analogWrite(pinMotorDireito_A, 0);
       analogWrite(pinMotorDireito_B, 0);
       analogWrite(pinMotorEsquerdo_A, velocidade);
       analogWrite(pinMotorEsquerdo_B, 0);
       break;
    case 4:
       digitalWrite(pinMotorDireito_A, LOW);
       digitalWrite(pinMotorDireito_B, LOW);
       digitalWrite(pinMotorEsquerdo_A, LOW);
       digitalWrite(pinMotorEsquerdo_B, LOW);
       break;
  }

  delay(20);
}