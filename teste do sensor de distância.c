#include <SoftwareSerial.h>

#define pinSensorDistEchoDianteiro 2     //cz
#define pinSensorDistTriggerDianteiro 50  //rx

void enviaPulso();
void medeDistancia();

//Variaveis para sensor de distancia
volatile unsigned long Triggertime = 0;
volatile float distancia = 0;
volatile int modo = -1;



void setup() {

  pinMode(pinSensorDistEchoDianteiro, INPUT);
  pinMode(pinSensorDistTriggerDianteiro, OUTPUT);
  //Desliga a porta de controle do sensor de distancia
  digitalWrite(pinSensorDistTriggerDianteiro, LOW);
  //Inicia a comunicação serial para monitorar o que está acontecendo pelo monitor serial
  Serial.begin(9600);
  //CONFIGURA A INTERRUPÇÃO PARA FUNCIONAMENTO DO SENSOR DE DISTANCIA
  attachInterrupt(digitalPinToInterrupt(pinSensorDistEchoDianteiro), medeDistancia, CHANGE); 
}


void loop(){
 //Teste do Sensor de Distancia
  //ENVIA O COMANDO PARA O MÓDULO LER A DISTANCIA
  enviaPulso();
  //A RESPOSTA DA DISTANCIA VEM POR INTERRUPÇÃO, SÓ PRECISA ESPERAR ALGUNS MILISSEGUNDOS
  delay(25); // TEMPO DE RESPOSTA APÓS A LEITURA
  Serial.print(" Dist. dianteira:");
  Serial.print(distancia);
  Serial.println("cm");
  delay(1000);
}
//PROGRAMAÇÃO DAS FUNÇÕES
//Envia um pulso para o sensor de distância
void enviaPulso(){
  // ENVIA O SINAL PARA O MÓDULO INICIAR O FUNCIONAMENTO
  digitalWrite(pinSensorDistTriggerDianteiro, HIGH);
  // AGUARDAR 10 uS PARA GARANTIR QUE O MÓDULO VAI INICIAR O ENVIO
  delayMicroseconds(10);
  // DESLIGA A PORTA PARA FICAR PRONTO PARA PROXIMA MEDIÇÃO
  digitalWrite(pinSensorDistTriggerDianteiro, LOW);
  // INDICA O MODO DE FUNCIONAMENTO (AGUARDAR PULSO)
  modo = 0;
}

//Calcula a distância detectada
//A interrupção será acionada quando o reflexo do pulso for recebido e então executa esta função.
void medeDistancia(){
  switch (modo) {
    case 0: {
        Triggertime = micros();
        modo = 1;
        break;
      }
    case 1: {
        distancia = (float)(micros() - Triggertime) / 58.3; // distancia em CM
        Triggertime = 0;
        modo = -1;
        break;
      }
  }
}