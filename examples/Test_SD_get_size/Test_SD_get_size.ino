/*
 * Test_SD_get_size.cpp
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

//Librerias para el manejo del socket SD CARD
#include <Arduino.h> 
#include "FS.h" 
#include "SD.h" 
#include "SPI.h" 
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

SPIClass *SDspi = NULL; 

void setup() { 
  // Inicialización del depurador Serial
  Serial.begin(115200); 
  // Inicialización de SD Card
  initSDCard(); 
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Blink_Led(3, 100);
} 
 
void initSDCard(){ 
  SDspi = new SPIClass(HSPI); 
  SDspi->begin(PIN_HSPI_SCLK, PIN_HSPI_MISO, PIN_HSPI_MOSI, PIN_HSPI_SS); 
  Serial.println("SD Card Init"); 
 
  if(!SD.begin(PIN_HSPI_SS, *SDspi)){ 
    Serial.println("Card Mount Failed"); 
    return; 
  } 
  uint8_t cardType = SD.cardType(); 
 
  if(cardType == CARD_NONE){ 
    Serial.println("No SD card attached"); 
    return; 
  } else { 
    Serial.println("SD card attached"); 
  } 
 
  Serial.print("SD Card Type: "); 
  if(cardType == CARD_MMC){ 
    Serial.println("MMC"); 
  } else if(cardType == CARD_SD){ 
    Serial.println("SDSC"); 
  } else if(cardType == CARD_SDHC){ 
    Serial.println("SDHC"); 
  } else { 
    Serial.println("UNKNOWN"); 
  } 
  uint64_t cardSize = SD.cardSize() / (1024 * 1024); 
  Serial.printf("SD Card Size: %lluMB\n", cardSize); 
  Blink_Led(5, 400);
} 
 

void loop() 
{ 
  
}
// Desarrollo de funciones
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