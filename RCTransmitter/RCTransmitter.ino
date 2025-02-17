#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
// La conexión de LY y LX estan invertidas (Por conveniencia conecte de lado los joystick)
// por lo tanto los ejes X Y estan invertidos
#define LX A0
#define LY A1
#define RX A2
#define RY A3
const byte address[6] = "00001";
void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.begin(9600);
}

int ly,lx,ry,rx;
void readJoysticks() {
  ly = map(analogRead(LY), 0, 1024, 0, 100);
  lx = map(analogRead(LX), 0, 1024, 0, 100);
  ry = map(analogRead(RY), 0, 1024, 0, 100);
  rx = map(analogRead(RX), 0, 1024, 0, 100);
  Serial.println("*****************************************");
  Serial.print("Izquierda Y: ");
  Serial.println(ly);
  Serial.print("Izquierda X: ");
  Serial.println(lx);
  Serial.print("Derecha Y: ");
  Serial.println(ry);
  Serial.print("Derecha X: ");
  Serial.println(rx);
}

void loop() {
  readJoysticks();
  int joystickValues[4] = {ly,lx,ry,rx};
  radio.write(joystickValues, sizeof(joystickValues));
  delay(1000);
}
