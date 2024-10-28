/*
 * Macaron_BME280_and_LoRaWAN_Tago.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Libreria para el manejo del sensor BME280
#include "DFRobot_BME280.h"
// Pines de la placa de desarrollo
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********
/**IIC address is 0x77 when pin SDO is high */
/**IIC address is 0x76 when pin SDO is low */
BME   bme(&Wire, 0x76);  // 0x76 por defecto en la placa de desarrollo
#define SEA_LEVEL_PRESSURE    1015.0f // constante para hallar la altitud en mmsnm

// Configuración Modo de conexión y Clase (LoRaWAN)
#define MODO_CONEXION   OTAA_CONECTION
#define CLASE           CLASE_C
#define TIME_SEND       60000 // tiempo de envío de datos al gateway en milisegundos
// Configuración Serial para el RAK3172 en modo LoRa
#define RAK3172_LORA_BAUDRATE 115200

// Configuración de la banda de frecuencia
// Las bandas libres de trabajo en Perú son AS923 Y AU915
banda_t BANDA = AS923_1;
// Configuración de velocidad de transmisión (data rate)
// AS923: El rango de valores de la velocidad de datos es 2-5 (Data_Rate_2 - Data_Rate_5).
// AU915 / LA915: El rango de valores de la velocidad de datos es 0-6 (Data_Rate_0 - Data_Rate_6).
data_rate_t DATA_RATE = Data_Rate_5;
// Configuración de la potencia de transmisión
// EU868 / RU864 / KR920 / AS923 / CN470, el rango de valores de potencia de transmisión es 0-7 donde la potencia mas alta comeinza desde cero.
// US915 / AU915, el rango de valores de potencia de transmisión es 0-14. 
transmit_power_t TXP_POWER = TXP0;
// Configuración de DevEui y AppKey
const char* DevEui = "AC1F09FFFE0D6D09";
const char* AppKey = "AC1F09FFFE0D6D09AC1F09FFF9153172";
// variables globales
int data_receive_gateway;
char textDebug[128];
char textAux[50];
unsigned long Time_ = 0;

//Prototipo de funciones
void printLastOperateStatus(BME::eStatus_t eStatus);
void Smelpro_RAK3172_ConvertToTextHexadecimal(char *text, uint8_t *n, uint8_t sizeN);
void Blink_Led(uint8_t attemps, uint16_t delay_);

void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  // Inicialización del sensor BME280
  while(bme.begin() != BME::eStatusOK) {
    Serial.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bme begin success");
  Serial.println("Iniciando sistema");
  // Inicialización del RAK3172 mediante comnados AT
  while(!Macaron.RAK3172_LoraInit(RAK3172_LORA_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializado...\r\n");
    delay(1000);
  }
  Serial.println("RAK3172 rebooted !\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  // Macaron.RAK3172_Echo();
  // Inicio de la conexión con el gateway
  // Configuración del DEVUI, APPKEY, CLASE, MODO_DE_CONEXION, BANDA, VELOCIDAD_DE_DATOS, POTENCIA_DE_TRANSMISION
  while(!Macaron.RAK3172_LoraWanSetConfig(DevEui,AppKey,CLASE,OTAA_CONECTION,BANDA,DATA_RATE,TXP_POWER))
  {
    Serial.println("RAK3172 seteando...\r\n");
    delay(1000);
  }
  Serial.println("RAK3172 seteado para LoRaWAN!\r\n");
  while(true)
  {
    Serial.println("RAK3172 conectando...\r\n");
    delay(100);
    // Macaron.RAK3172_LoraWanJoin(intervalo de reintentos para unirse al gateway(segundos), número de intentos)
    if(Macaron.RAK3172_LoraWanJoin(15, 3))
    {
      Serial.println("RAK3172 conectado al Gateway !\r\n");
      break;
    }
  }
  Blink_Led(3, 100);
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

    //Adaptación de datos para el envío por LoRaWAN
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
    //***********************************************************************
    // Macaron.RAK3172_LoraWanSend(puerto, payload, intentos, tiempo(milisegundos))
    if(Macaron.RAK3172_LoraWanSend(2, textDebug, 3, 2000))
    {
      Blink_Led(3, 200);
    }
    else
    {
      Serial.println("No hubo confirmacion del dato recibido desde el Gateway !\r\n");
      Serial.println("Aumentar la cantidad de intentos o verificar que el Gateway este en linea\r\n");
    }
  }
  else
  {
    // Hacer otra tarea
  }
}
// Desarrollo de funciones
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