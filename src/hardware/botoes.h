#pragma once

void botoesInit();
void botoesUpdate();            // chamar dentro do loop()

// btn1
bool btn1Apertou();             // borda de pressão (uma vez por toque)
bool btn1Pressionado();         // estado contínuo

// btn2
bool btn2Apertou();
bool btn2Pressionado();

// btnSave
bool btnSaveApertou();