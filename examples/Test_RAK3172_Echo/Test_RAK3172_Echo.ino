/*
 * Test_RAK3172_Echo.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

// Configuración Serial para el RAK3172
#define RAK3172_BAUDRATE 115200

void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  while(!Macaron.RAK3172_LoraInit(RAK3172_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializando...\r\n");
    delay(500);
  }
  Serial.println("RAK3172 iniciado!\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  Macaron.RAK3172_Echo();
}

void loop() {
  
}
// Función de parpadeo de led
void Blink_Led(uint8_t attemps, uint16_t delay_)
{
  for(uint8_t i = 0; i<attemps; i++)
  {
    digitalWrite(PIN_LED_BLINK,HIGH);
    delay(delay_);
    digitalWrite(PIN_LED_BLINK,LOW);
    delay(delay_);
  }
}
