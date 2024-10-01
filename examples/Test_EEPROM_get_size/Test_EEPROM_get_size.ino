/*
 * Test_EEPROM_get_size.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

//Libreria para el manejo del periferico I2C
#include "Wire.h"
//Libreria para el manejo de la memoria EEPROM
#include "I2C_eeprom.h"
// Pines de la placa de desarrollo
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

// Inicialización de la EEPROM, en la placa de desarrollo Smelpro Macaron tiene la direción 0X50
/*La EEPROM de la  placa de desarrollo (M24128) de STMicroelectronics es compatible con la
libreria para la EEPROM 24LC128 de Microchip*/
I2C_eeprom ee(0x50, I2C_DEVICESIZE_24LC128); // eeprom M24128

uint32_t start, diff;
// Porgrama básico para determinar el tamaño de la memoria EEPROM

void setup()
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  while (!Serial);  
  Serial.println(__FILE__);
  Serial.print("I2C_EEPROM_VERSION: ");
  Serial.println(I2C_EEPROM_VERSION);
  // Inicialización del periferico I2C para la memoria EEPROM
  Wire.begin();
  // Inicialización de la EEPROM
  ee.begin();
  if (! ee.isConnected())
  {
    Serial.println("ERROR: Can't find eeprom\nstopped...");
    while (1);
  }

  Serial.println("\nDetermine size");
  delay(10);

  start = micros();
  uint32_t size = ee.determineSize(false); 
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.print(diff);
  Serial.println(" us.");
  if (size == 0)
  {
    Serial.println("SIZE: could not determine size");
  }
  else if (size > 1024)
  {
    Serial.print("SIZE: ");
    Serial.print(size / 1024);
    Serial.println(" KB.");
  }
  else
  {
    Serial.print("SIZE: ");
    Serial.print(size);
    Serial.println(" bytes.");
  }

  Serial.print("PAGE: ");
  uint8_t pageSize = ee.getPageSize(size);
  Serial.print(pageSize);
  Serial.println(" bytes.");

  Serial.println("Done...");
  Blink_Led(3, 100);
}


void loop()
{
  // Tarea principal
}
// Función de parpadeo de Led
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


