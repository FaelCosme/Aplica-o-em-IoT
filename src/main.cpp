#include "AccelStepper.h"

// Pinos do driver do motor de passo
#define stepPin 4
#define dirPin  5   // pino de direção do driver (ajuste conforme sua ligação)
#define motorInterfaceType 1

// Botões
const int pinoBotao1 = 21; // gira num sentido
const int pinoBotao2 = 22; // gira no sentido contrário

// Configurações de giro
const float velocidadeMotor = 800.0;      // passos por segundo
const unsigned long duracaoGiro = 3000;   // tempo girando, em ms (3 segundos)

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

// Controle de estado
enum EstadoMotor { PARADO, GIRANDO_FRENTE, GIRANDO_TRAS };
EstadoMotor estado = PARADO;
unsigned long tempoInicioGiro = 0;

void setup() {
  stepper.setMaxSpeed(1000);

  pinMode(pinoBotao1, INPUT);
  pinMode(pinoBotao2, INPUT);
}

void loop() {
  int estadoBotao1 = digitalRead(pinoBotao1);
  int estadoBotao2 = digitalRead(pinoBotao2);

  // Só aceita novo comando se o motor estiver parado
  if (estado == PARADO) {
    if (estadoBotao1 == LOW) {
      stepper.setSpeed(velocidadeMotor);   // sentido horário
      tempoInicioGiro = millis();
      estado = GIRANDO_FRENTE;
    } else if (estadoBotao2 == LOW) {
      stepper.setSpeed(-velocidadeMotor);  // sentido anti-horário (oposto)
      tempoInicioGiro = millis();
      estado = GIRANDO_TRAS;
    }
  }

  // Enquanto estiver girando, continua rodando o motor
  if (estado == GIRANDO_FRENTE || estado == GIRANDO_TRAS) {
    stepper.runSpeed();

    // Verifica se já passou o tempo definido
    if (millis() - tempoInicioGiro >= duracaoGiro) {
      stepper.setSpeed(0);
      estado = PARADO;
    }
  }
}