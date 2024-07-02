#define pinSensorObstDianteiro 53  //pt
#define pinSensorObstTraseiro 52  //vm
#include <SoftwareSerial.h>

void setup() {
  pinMode(pinSensorObstDianteiro, INPUT);
  pinMode(pinSensorObstTraseiro, INPUT);
  Serial.begin(9600);
 
}

void loop(){
  //Teste dos Sensores de Linha
  Serial.print("Sensor Dianteiro:");
  Serial.print(digitalRead(pinSensorObstDianteiro));
  delay(1000);
  Serial.print("\nSensor Traseiro:");
  Serial.print(digitalRead(pinSensorObstTraseiro));
  Serial.print("\n");
  delay(1000);

}