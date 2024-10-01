/*
 * Test_BME280_ReadData.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */
 
// Libreria de DFRobot para el sensor BME280
#include "DFRobot_BME280.h"
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********
/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME   bme(&Wire, 0x76);   // 0x76 por defecto en la placa de desarrollo smelpro Macaron

#define SEA_LEVEL_PRESSURE  1015.0f // Constante para hallar la altura en msnm
#define TIME_SEND           3000 // tiempo de visualizacion de datos por serial

//variables 
unsigned long Time_ = 0;

// Función para obtener el estado del sensor BME280
void printLastOperateStatus(BME::eStatus_t eStatus)
{
  switch(eStatus) {
  case BME::eStatusOK:    Serial.println("everything ok"); break;
  case BME::eStatusErr:   Serial.println("unknow error"); break;
  case BME::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
  case BME::eStatusErrParameter:    Serial.println("parameter error"); break;
  default: Serial.println("unknow status"); break;
  }
}

void setup()
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  // Inicialización del led de usuario
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("bme read data test");
  // Inicialización del sensor BME280
  while(bme.begin() != BME::eStatusOK) {
    Serial.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bme begin success");
  delay(1000);
  Blink_Led(3, 100);
}

void loop()
{
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    // Lectura de los parametros del sensor BME280
    float   temp = bme.getTemperature();
    uint32_t    press = bme.getPressure();
    float   alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
    float   humi = bme.getHumidity();
    // Depuración por serial
    Serial.println();
    Serial.println("======== start print ========");
    Serial.print("temperature (unit Celsius): "); Serial.println(temp);
    Serial.print("pressure (unit pa):         "); Serial.println(press);
    Serial.print("altitude (unit meter):      "); Serial.println(alti);
    Serial.print("humidity (unit percent):    "); Serial.println(humi);
    Serial.println("========  end print  ========");
  }
  else
  {
    // Hacer otra tarea
  }
}
// Dessarrollo de funciones
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
