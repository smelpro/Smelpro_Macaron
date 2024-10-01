/*
 * Smelpro_Macaron.h
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

#include <Arduino.h>
// Libreria para el manejo del tranceptor RAK3172
#include "Smelpro_RAK3172.h"

// Definicion de pines de la tarjeta de desarrollo
#define PIN_LED_BLINK  		2
#define PIN_LED_RGB     	3
#define PIN_WR_EEPROM 	    21
#define PIN_RST_RAK3172 	14
#define PIN_BTN_USER	    48
#define PIN_RX_RAK3172 		18
#define PIN_TX_RAK3172 		17
#define PIN_I2C_SCL			9
#define PIN_I2C_SDA			8
#define PIN_HSPI_MISO 		37 
#define PIN_HSPI_MOSI 		35 
#define PIN_HSPI_SCLK 		36 
#define PIN_HSPI_SS  		39