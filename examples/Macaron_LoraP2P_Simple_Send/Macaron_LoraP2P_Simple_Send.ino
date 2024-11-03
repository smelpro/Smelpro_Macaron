/*
 * Test_Send_LoraP2P.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Libreria para el manejo del sensor BME280
#include "DFRobot_BME280.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********
/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME   bme(&Wire, 0x76);  // 0x76 por defecto en la placa de desarrollo
#define SEA_LEVEL_PRESSURE    1015.0f // constante para hallar la altitud en mmsnm

#define TIME_SEND  60000 // tiempo de envío de datos en modo P2P en milisegundos
// Configuración Serial para el RAK3172 en el modo LoRa
#define RAK3172_LORA_BAUDRATE 115200

// Variables P2P RAK3172
// configuración de frecuencia
/*
 Para este ejemplo, el valor de frecuencia predeterminada es 923000000 MHz y el rango de frecuencia operable es de 915000000 a 930000000.
 La variante (RAK3172) de alta frecuencia es compatible sólo con 600000000 - 960000000.
 La variante (RAK3172) de baja frecuencia es compatible sólo con 150000000 - 600000000.
 NOTA: El módulo LoRa de la placa de desarrollo Smelpro Macaron es de alta frecuencia, para mas información verificar su hoja de datos
*/
// configuración de frecuencia P2P segun hoja de datos
const char* FRECUENCIA_P2P = "923000000"; 
// configuración de factor de dispersion (5-12)
uint8_t spreading_factor = 7;
// Configuración de ancho de banda (0 = 125, 1 = 250, 2 = 500, 3 = 7.8, 4 = 10.4, 5 = 15.63,6 = 20.83, 7 = 31.25, 8 = 41.67, 9 = 62.5)
Bandwidth_t bandwidth_P2P = Bandwidth_125Khz;
// Tasa de código de configuración para el modo P2P (0=4/5, 1=4/6, 2=4/7, 3=4/8), por defecto 0=4/5
PCR_t Code_Rate_P2P = Code_Rate_4_5;
// Configuración de longitud del preámbulo P2P (5-65535)
uint16_t Preamble_Length = 8;
// Configuración de la potencia de transmisión (5-22)
uint8_t Tx_Power = 15;

// variables
char textDebug[128];
char textAux[50];
unsigned long Time_ = 0;

void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  // Inicialización del sensor BME280
  while(bme.begin() != BME::eStatusOK) {
    Serial.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bme begin success");
  while(!Macaron.RAK3172_LoraInit(RAK3172_LORA_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializando...\r\n");
    delay(500);
  }
  Serial.println("RAK3172 iniciado!\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  //Macaron.RAK3172_Echo();
  // Inicialización del RAK3172 mediante comnados AT
  // Inicio de configuración en modo P2P
  while(!Macaron.RAK3172_P2PSetConfig(FRECUENCIA_P2P, spreading_factor, bandwidth_P2P,Code_Rate_P2P, Preamble_Length, Tx_Power))
  {
    Serial.println("RAK3172 seteando...\r\n");
    delay(1000);
  }
  Serial.println("RAK3172 seteado en modo P2P!\r\n");
  Blink_Led(4, 100);
}

void loop() {
  // envío de datos cada 1 minuto (TIME_SEND)
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    //Aquiriendo datos del sensor BME280
    float   temp = bme.getTemperature();
    uint32_t  press = bme.getPressure();
    float   alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
    float   humi = bme.getHumidity();

    //Adaptación de datos para el envio por LoRaP2P
    uint16_t temp2 = (temp*100);
    uint16_t press2 = (press/100);
    uint16_t alti2 = (alti*10);
    uint16_t humi2 = (humi*100);

    Serial.println();
    Serial.println("======== start print ========");
    Serial.print("temperature (unit Celsius): "); Serial.println(temp);
    Serial.print("pressure (unit pa):         "); Serial.println(press);
    Serial.print("altitude (unit meter):      "); Serial.println(alti);
    Serial.print("humidity (unit percent):    "); Serial.println(humi);
    Serial.println("========  end print  ========");
    Smelpro_RAK3172_ConvertToTextHexadecimal(textAux, (uint8_t*)&temp2, sizeof(uint16_t));
    sprintf(textDebug, "%s", textAux);
    Smelpro_RAK3172_ConvertToTextHexadecimal(textAux, (uint8_t*)&press2, sizeof(uint16_t));
    strcat(textDebug, textAux);
    Smelpro_RAK3172_ConvertToTextHexadecimal(textAux, (uint8_t*)&alti2, sizeof(uint16_t));
    strcat(textDebug, textAux);
    Smelpro_RAK3172_ConvertToTextHexadecimal(textAux, (uint8_t*)&humi2, sizeof(uint16_t));
    strcat(textDebug, textAux);

    // Macaron.RAK3172_P2PSend(char* stringHex, uint64_t timeout)
    if(Macaron.RAK3172_P2PSend(textDebug, 2000))
    {
      // Los datos fueron enviados correctamente en modo P2P
      Blink_Led(3, 200);
    }
  }
  else
  {
    // Hacer otra tarea
  }
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
// Función para convertir datos ASCII a Hexadecimal
void Smelpro_RAK3172_ConvertToTextHexadecimal(char *text, uint8_t *n, uint8_t sizeN)
{
  char str_n[32];
  char hexByte[16];

  n += (sizeN - 1);
  memset(str_n, 0, sizeof(str_n));
  memset(hexByte, 0, sizeof(hexByte));
  for (int i = 0; i < sizeN; i++)
	{
		sprintf(hexByte, "%X", *n);

		if (strlen(hexByte) == 1)
		{
			sprintf(hexByte, "0%X", *n);
		}
		strcat(str_n, hexByte);
		n--;
	}
  strcat(str_n, "\0");
  strcpy(text, str_n);
}
// Función para obtener el estado de sensor BME280
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