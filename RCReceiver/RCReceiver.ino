#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define ALERON_IZQ 6
#define ALERON_DER 5
#define COLA 4
#define TIMON 3
#define ESC 2

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Definicion de objetos Servo para cada superficie
Servo servoAleronIzq;
Servo servoAleronDer;
Servo servoTimon;
Servo servoCola;

// Definicion de objeto Servo para ESC
Servo servoEsc;
// Funcion auxiliar para calcular el desfase (+-15 grados) a partir de un valor entre 0-100
int calculateDeviation(int input, int deadLow = 48, int deadHigh = 52, int maxDeflection = 60) {
  if(input >= deadLow && input <= deadHigh)
    return 0;
  else
      return map(input, 0, 100, -maxDeflection, maxDeflection);
}

// Funcion para controlar el cabeceo (elevador) que usa el eje y del joystick izquierdo
void controlPitch(int pitchInput) {
  int deviation = calculateDeviation(pitchInput);
  int angle = 90 + deviation; // 90 representa la posicion central
  servoCola.write(angle);
}

// Funcion para controlar el alabeo (alerones) que usa el eje x del joystick izquierdo
void controlRoll(int rollInput) {
  int deviation = calculateDeviation(rollInput);
  // Para el alabeo, los alerones se mueven en direcciones opuestas
  int angleIzq = 90 - deviation;
  int angleDer = 90 + deviation;
  servoAleronIzq.write(angleIzq);
  servoAleronDer.write(angleDer);
}

// Funcion para controlar la guinada (timon) usando el eje x del joystick derecho
void controlYaw(int yawInput) {
  int deviation = calculateDeviation(yawInput);
  int angle = 90 + deviation;
  servoTimon.write(angle);
}

void controlSpeed(int speedInput) {
  int speed = map(speedInput, 50, 100, 0, 180);
  servoEsc.write(speed);
}


void setup() {
  Serial.begin(9600);

  // Inicializacion del radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("Escuchando...");

  // Asociar servos a sus pines
  servoAleronIzq.attach(ALERON_IZQ);
  servoAleronDer.attach(ALERON_DER);
  servoTimon.attach(TIMON);
  servoCola.attach(COLA);
  servoEsc.attach(ESC, 1000,2000);

  // Posicionar servos en 90 grados (centro)
  servoAleronIzq.write(90);
  servoAleronDer.write(90);
  servoTimon.write(90);
  servoCola.write(90);
}

void loop() {
 
  if (radio.available()) {
    int joystickValues[4] = {50,50,50,50};
    radio.read(&joystickValues, sizeof(joystickValues));
    
    Serial.println("Valores de Joystick recibidos:");
    Serial.print("Izquierda Y: ");
    Serial.println(joystickValues[0]);
    Serial.print("Izquierda X: ");
    Serial.println(joystickValues[1]);
    Serial.print("Derecha Y: ");
    Serial.println(joystickValues[2]);
    Serial.print("Derecha X: ");
    Serial.println(joystickValues[3]);
    
    Serial.println("*****************************************");

    // Control de superficies:
    controlPitch(joystickValues[0]);
    controlRoll(joystickValues[1]);
    controlYaw(joystickValues[3]);
    controlSpeed(joystickValues[2]);
  }
}
