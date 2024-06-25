#include <SoftwareSerial.h>

#define pinM1A 4   //lj
#define pinM1B 3  //vd
#define pinM2A 6  //am
#define pinM2B 5  //az

#define pinSensorBordaD 52  //vm

#define pinEcoD 2       //cz
#define pinTriggerD 50  //rx

#define pinBRx 10  //cz
#define pinBTx 8   //não será conectado ao módulo. Fio pino Rx do módulo ligado no GND. Não será usado.

#define pinLeftLED 22  //br
#define pinRightLED 23  //az

#define pinFarol1 40   //mr
#define pinFarol2 41   //am
#define pinFarol3 42   //vm
#define pinFarol4 43   //lj

#define alertasensor1 44   //vm
#define alertasensor2 45   //az

#define pinBuzzer 11  //mr                     

SoftwareSerial bluetooth(pinBRx, pinBTx);  // RX, TX

void Eco();
void Distancia();
void controlaBuzzer();
void controlaBuzzerBorda();

//Variaveis para sensor de distancia
volatile unsigned long eco0 = 0;
volatile float distancia = 0;
volatile int modo = -1;

 //Variaveis para motor
int velocidade = 0;
int comando = 0;
int velMin = 100;
int Vel = 0;

//Variaveis para controle remoto
char mensagem;
bool SensorBordaD;

//Variaveis para sensor de distância
bool sensorAtivo = false;

  //Variaveis para detector de borda
bool detectorBorda = false;

// Variáveis ​​para LED blink
unsigned long lastBlinkTime = 0;
unsigned long previousLEDMillis = 0;
bool leftLEDState = LOW;
bool rightLEDState = LOW;

// Variáveis para controle do buzzer
unsigned long previousMillis = 0;  // Armazena o último tempo de troca
unsigned long previousMillisBorda = 0; // Armazena o último tempo de troca
bool buzzerState = false;          // Estado atual do buzzer

void setup() {
  pinMode(9, OUTPUT);
  pinMode(pinM1A, OUTPUT);
  pinMode(pinM1B, OUTPUT);
  pinMode(pinM2A, OUTPUT);
  pinMode(pinM2B, OUTPUT);

  pinMode(pinSensorBordaD, INPUT);
  pinMode(pinEcoD, INPUT);

  pinMode(pinTriggerD, OUTPUT);
  pinMode(pinLeftLED, OUTPUT);
  pinMode(pinRightLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);


  pinMode(pinFarol1, OUTPUT);
  pinMode(pinFarol2, OUTPUT);
  pinMode(pinFarol3, OUTPUT);
  pinMode(pinFarol4, OUTPUT);
  pinMode(alertasensor1, OUTPUT);
  pinMode(alertasensor2, OUTPUT);

  digitalWrite(pinFarol1, LOW);
  digitalWrite(pinFarol2, LOW);
  digitalWrite(pinFarol3, LOW);
  digitalWrite(pinFarol4, LOW);
  digitalWrite(alertasensor1, LOW);
  digitalWrite(alertasensor2, LOW);

  digitalWrite(pinTriggerD, LOW);
  
  digitalWrite(pinLeftLED, LOW);
  digitalWrite(pinRightLED, LOW);
  digitalWrite(pinBuzzer, LOW);

  Serial.begin(9600);
  bluetooth.begin(9600);

  // CONFIGURA A INTERRUPÇÃO PARA SENSOR DE DISTANCIA
  attachInterrupt(digitalPinToInterrupt(pinEcoD), Distancia, CHANGE);
}

// Controla o buzzer baseado na distância medida
void controlaBuzzer() {
  if (distancia < 10 && distancia > 0) { // Diminui a distância inicial para 10 cm
    digitalWrite(alertasensor2, HIGH);    
    unsigned long currentMillis = millis();
    int intervalo = (500 - (10 - distancia) * 15) / 6; // Intervalo diminui conforme a distância diminui e aumenta a frequência de liga/desliga por 6x (anteriormente 3x)
    if (intervalo < 50) intervalo = 50; // Limita o intervalo mínimo para 50ms
    
    if (currentMillis - previousMillis >= intervalo) {
      previousMillis = currentMillis;
      buzzerState = !buzzerState;
      if (buzzerState) {
        int freq = 1000 + (10 - distancia) * 100; // Frequência aumenta a cada 2 cm mais próximo
        tone(pinBuzzer, freq);
      } else {
        noTone(pinBuzzer); // Desliga o buzzer temporariamente
      }
    }
  } else {
    digitalWrite(alertasensor2, LOW);
    noTone(pinBuzzer); // Desliga o buzzer se a distância for maior que 10 cm
  }
}

// Controla o buzzer baseado na distância medida
void controlaBuzzerBorda() {
  unsigned long currentMillisBorda = millis();
  int intervalo = 100;  // Limita o intervalo mínimo para 100ms
  if (currentMillisBorda - previousMillisBorda >= intervalo) {
    previousMillisBorda = currentMillisBorda;
    buzzerState = !buzzerState;
    if (buzzerState) {
      tone(pinBuzzer, 1000);
    } else {
      noTone(pinBuzzer);  // Desliga o buzzer temporariamente
    }
  } else {
    noTone(pinBuzzer);  // Desliga o buzzer se a distância for maior que 10 cm
  }
}

void Distancia() {
  switch (modo) {
    case 0:
      eco0 = micros();
      modo = 1;
      break;
    case 1:
      distancia = (float)(micros() - eco0) / 58.3;  // distancia em CM
      eco0 = 0;
      modo = -1;
      break;
  }
}

void Eco() {
  // ENVIA O SINAL PARA O MÓDULO INICIAR O FUNCIONAMENTO
  digitalWrite(pinTriggerD, HIGH);
  // AGUARDAR 10 uS PARA GARANTIR QUE O MÓDULO VAI INICIAR O ENVIO   
  delayMicroseconds(10);
  // DESLIGA A PORTA PARA FICAR PRONTO PARA PROXIMA MEDIÇÃO
  digitalWrite(pinTriggerD, LOW);
  // INDICA O MODO DE FUNCIONAMENTO (AGUARDAR PULSO)
  modo = 0;
}

void loop() {
  // Captura mensagens via Bluetooth
  if (bluetooth.available()) {
    mensagem = bluetooth.read();
  }

  // Captura do sensor de linha
  SensorBordaD = digitalRead(pinSensorBordaD);

  // Ativa o sensor de distância
  Eco();

  // Controle do Buzzer
  if (!SensorBordaD) controlaBuzzer();
  
  // Controle dos LEDs
  unsigned long currentMillis = millis();

  // Reinicia o estado dos LEDs se nenhum comando específico for recebido
  if (mensagem != 'L' && mensagem != 'R' && mensagem != 'G' && mensagem != 'I') {
    digitalWrite(pinLeftLED, LOW);
    digitalWrite(pinRightLED, LOW);
  }

  // Lógica para acender os LEDs com base no comando recebido
  switch (mensagem) {
    case 'L':  // Esquerda
      if (currentMillis - previousLEDMillis >= 333) {  // 333ms para aproximar 3Hz
        previousLEDMillis = currentMillis;
        digitalWrite(pinLeftLED, !digitalRead(pinLeftLED));  // Inverte o estado do LED esquerdo
      }
      digitalWrite(pinRightLED, LOW);
      break;
    case 'R':  // Direita
      if (currentMillis - previousLEDMillis >= 333) {  // 333ms para aproximar 3Hz
        previousLEDMillis = currentMillis;
        digitalWrite(pinRightLED, !digitalRead(pinRightLED));  // Inverte o estado do LED direito
      }
      digitalWrite(pinLeftLED, LOW);
      break;
    case 'G':  // Para frente e esquerda
      if (currentMillis - previousLEDMillis >= 333) {  // 333ms para aproximar 3Hz
        previousLEDMillis = currentMillis;
        digitalWrite(pinLeftLED, !digitalRead(pinLeftLED));  // Inverte o estado do LED esquerdo
      }
      digitalWrite(pinRightLED, LOW);
      break;
    case 'I':  // Para frente e direita
      if (currentMillis - previousLEDMillis >= 333) {  // 333ms para aproximar 3Hz
        previousLEDMillis = currentMillis;
        digitalWrite(pinRightLED, !digitalRead(pinRightLED));  // Inverte o estado do LED direito
      }
      digitalWrite(pinLeftLED, LOW);
      break;
  }

  // Controle dos motores
  switch (mensagem) {
    case 'S':  // Parar
      comando = 0;
      break;
    case 'D':  // Parar tudo
      comando = 0;
      break;
    case 'F':  // Para frente
      comando = 1;
      break;
    case 'B':  // Ré
      comando = 2;
      break;
    case 'L':  // Esquerda
      comando = 3;
      break;
    case 'R':  // Direita
      comando = 4;
      break;
    case 'G':  // Para frente e esquerda
      comando = 5;
      break;
    case 'I':  // Para frente e direita
      comando = 6;
      break;
    case 'H':  // Ré e esquerda
      comando = 7;
      break;
    case 'J':  // Ré e direita
      comando = 8;
      break;
    // Controle de velocidade de "0" a "q"
    case '0':
      Vel = 0;
      break;
    case '1':
      Vel = 1;
      break;
    case '2':
      Vel = 2;
      break;
    case '3':
      Vel = 3;
      break;
    case '4':
      Vel = 4;
      break;
    case '5':
      Vel = 5;
      break;
    case '6':
      Vel = 6;
      break;
    case '7':
      Vel = 7;
      break;
    case '8':
      Vel = 8;
      break;
    case '9':
      Vel = 9;
      break;
    case 'q':
      Vel = 10;
      break;
    // Fim do controle de velocidade

    // Habilita faróis
    case 'v':  // Desativa
      digitalWrite(pinFarol1, LOW);
      digitalWrite(pinFarol2, LOW);
      digitalWrite(pinFarol3, LOW);
      digitalWrite(pinFarol4, LOW);
      sensorAtivo = false;
      break;
    case 'V':  // Ativa
      digitalWrite(pinFarol1, HIGH);
      digitalWrite(pinFarol2, HIGH);
      digitalWrite(pinFarol3, HIGH);
      digitalWrite(pinFarol4, HIGH);
      sensorAtivo = true;
      break;
  }

  // Cálculo da velocidade
  velocidade = velMin + (((255 - velMin) / 10) * Vel);

  // Controle do detector de borda

    if(SensorBordaD&&((comando==1)||(comando==5)||(comando==6))) {
      comando = 0;
    } 
    
    if (SensorBordaD) {
    digitalWrite(alertasensor1, HIGH);
    controlaBuzzerBorda();
    } else {
    digitalWrite(alertasensor1, LOW);
  }

  // Redução de velocidade com base na distância medida
  if ((distancia < 12) && ((comando == 1) || (comando == 5) || (comando == 6))) {
    if (distancia < 7) {
      velocidade = 0;
      } else {
      velocidade = map(distancia, 12, 7, velocidade, 0);
    }
  }

  // Controle dos motores com base no comando
  switch (comando) {
    case 0:  // Parado
      digitalWrite(pinM1A, LOW);
      digitalWrite(pinM1B, LOW);
      digitalWrite(pinM2A, LOW);
      digitalWrite(pinM2B, LOW);
      break;
    case 1:  // Para frente
      analogWrite(pinM1A, velocidade);
      analogWrite(pinM1B, 0);
      analogWrite(pinM2A, velocidade);
      analogWrite(pinM2B, 0);
      break;
    case 2:  // Ré
      analogWrite(pinM1A, 0);
      analogWrite(pinM1B, velocidade);
      analogWrite(pinM2A, 0);
      analogWrite(pinM2B, velocidade);
      break;
    case 3:  // Esquerda
      analogWrite(pinM1A, 0);
      analogWrite(pinM1B, velocidade);
      analogWrite(pinM2A, velocidade);
      analogWrite(pinM2B, 0);
      break;
    case 4:  // Direita
      analogWrite(pinM1A, velocidade);
      analogWrite(pinM1B, 0);
      analogWrite(pinM2A, 0);
      analogWrite(pinM2B, velocidade);
      break;
    case 5:  // Para frente e esquerda
      analogWrite(pinM1A, velocidade / 3);
      analogWrite(pinM1B, 0);
      analogWrite(pinM2A, velocidade);
      analogWrite(pinM2B, 0);
      break;
    case 6:  // Para frente e direita
      analogWrite(pinM1A, velocidade);
      analogWrite(pinM1B, 0);
      analogWrite(pinM2A, velocidade / 3);
      analogWrite(pinM2B, 0);
      break;
    case 7:  // Ré e esquerda
      analogWrite(pinM1A, 0);
      analogWrite(pinM1B, velocidade);
      analogWrite(pinM2A, 0);
      analogWrite(pinM2B, velocidade / 3);
      break;
    case 8:  // Ré e direita
      analogWrite(pinM1A, 0);
      analogWrite(pinM1B, velocidade / 3);
      analogWrite(pinM2A, 0);
      analogWrite(pinM2B, velocidade);
      break;
  }
  // Aguarda um pequeno tempo antes de repetir o loop
  delay(20);
}