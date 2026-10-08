#include "hardware/motor.h"
#include "config.h"
#include <AccelStepper.h>

static AccelStepper stepper(AccelStepper::DRIVER, PINO_STEP, PINO_DIR);

void motorInit() {
  stepper.setMaxSpeed(VEL_AUTO);
  stepper.setAcceleration(ACCEL_AUTO);
}

void motorUpdate() {
  stepper.run();
}

void motorIrPara(long posicao) {
  stepper.setMaxSpeed(VEL_AUTO);
  stepper.setAcceleration(ACCEL_AUTO);
  stepper.moveTo(posicao);
}

void motorJog(int direcao) {
  stepper.setMaxSpeed(VEL_JOG);
  stepper.setAcceleration(ACCEL_JOG);
  if (direcao > 0) {
    if (stepper.distanceToGo() < 200) {
      stepper.moveTo(stepper.currentPosition() + 100000);
    }
  } else {
    if (stepper.distanceToGo() > -200) {
      stepper.moveTo(stepper.currentPosition() - 100000);
    }
  }
}

void motorJogParar() {
  stepper.setMaxSpeed(VEL_JOG);
  stepper.setAcceleration(ACCEL_JOG);
  if (stepper.distanceToGo() != 0) {
    stepper.moveTo(stepper.currentPosition());
  }
}

void motorParar() {
  stepper.stop();
}

long motorPosicaoAtual() {
  return stepper.currentPosition();
}

void motorSetPosicao(long pos) {
  stepper.setCurrentPosition(pos);
}

bool motorEmMovimento() {
  return stepper.distanceToGo() != 0;
}