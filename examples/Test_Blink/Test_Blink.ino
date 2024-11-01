/*
 * Test_Blink.ino
 *
 *  Creado: Nov 1, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

#include "Smelpro_Macaron.h"

// Se declara la funcipon void, se utiliza el led de prueba ( PIN_LED_BLINK) , el cual es el pin 2

void setup() {
  // Inicializamos el pin PIN_LED_BLINK como salida.
  pinMode(PIN_LED_BLINK, OUTPUT);
}

// Corremos la función loop
void loop() {
  digitalWrite(PIN_LED_BLINK, HIGH);  // Encendemos el led, nivel lógico alto
  delay(1000);                        // Esperamos 1 segundo(1000ms)
  digitalWrite(PIN_LED_BLINK, LOW);   // Encendemos el led, nivel lógico bajo
  delay(1000);                        // Esperamos 1 segundo(1000ms)
}
