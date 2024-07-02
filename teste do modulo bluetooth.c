#include <SoftwareSerial.h>
const byte LED1 = 44;
const byte LED2 = 45;
#define pinBtRx 10  // Mude para um pino diferente, por exemplo, 2
#define pinBtTx 11  // Mude para um pino diferente, por exemplo, 3

SoftwareSerial bluetooth(pinBtRx, pinBtTx); // RX, TX

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  bluetooth.begin(9600);
}

void loop() {
  char message;
  if (bluetooth.available()) {
    message = bluetooth.read();
    if(message = "F"){
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      delay(1000);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
    } 
  else {
    //Serial.print(" - ");
  }
}
}