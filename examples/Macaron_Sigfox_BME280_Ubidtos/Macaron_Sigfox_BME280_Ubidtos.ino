/*
 * Macaron_Sigfox_BME280_Ubidtos.ino
 *
 *  Creado: May 30, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Libreria para el manejo del sensor BME280
#include "DFRobot_BME280.h"
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

#define TIME_SEND       60000 // tiempo de envío de datos al backend de Sigfox en milisegundos
// Configuración Serial para el RAK3172 en modo Sigfox
#define RAK3172_SIGFOX_BAUDRATE 9600

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********
/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME   bme(&Wire, 0x76);  // 0x76 por defecto en la placa de desarrollo
#define SEA_LEVEL_PRESSURE    1015.0f // constante para hallar la altitud en mmsnm

// variables
char Buffer_for_Sigfox[200];
char textAux[20];
unsigned long Time_ = 0;

void setup() {
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
  // Inicializacion del RAK3172 mediante comnados AT
  while(!Macaron.RAK3172_SigfoxInit(RAK3172_SIGFOX_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializado...\r\n");
    delay(500);
  }
  // Prueba mediante comandos AT con el tranceptor RAK3172
  // Macaron.RAK3172_Echo();
  Serial.println("RAK3172 for Sigfox started!\r\n");
  // Configuración del módulo SIGFOX (RC, tx_power, encryption_key, payload_encryption)
  while(!Macaron.RAK3172_SigfoxSetConfig(4, 22, false, false))
  {
    Serial.println("RAK3172 Configurando...\r\n");
    delay(100);
  }
  Blink_Led(3, 100);
}

void loop() 
{
  // envio de datos cada 1 minuto (TIME_SEND)
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    //Aquiriendo datos del sensor BME280
    float   temp = bme.getTemperature();
    uint32_t  press = bme.getPressure();
    float   alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
    float   humi = bme.getHumidity();

    //Adaptación de datos para el envio por LoRaWAN
    uint16_t press2 = (uint16_t)(press/100);
    uint16_t alti2 = (uint16_t)(alti);
    Serial.printf("Temperatura: %.2f, Humedad: %.2f, presion: %d, altitud: %d\r\n", temp, humi, press2, alti2);
    // Armado de la trama para enviar a sigfox
    Smelpro_RAK3172_Sigfox_addFloat(temp);
    Smelpro_RAK3172_Sigfox_addFloat(humi);
    Smelpro_RAK3172_ConvertToTextHexadecimal(textAux, (uint8_t*)&press2, sizeof(uint16_t));
    strcat(Buffer_for_Sigfox, textAux);
    Smelpro_RAK3172_ConvertToTextHexadecimal(textAux, (uint8_t*)&alti2, sizeof(uint16_t));
    strcat(Buffer_for_Sigfox, textAux);
    // Trama a enviar al backend de Sigfox
    Serial.println(Buffer_for_Sigfox); 
    if(Macaron.RAK3172_SigfoxSendFrame(Buffer_for_Sigfox, 4)) // Macaron.RAK3172_SigfoxSendFrame(datos, intentos)
    {
      memset(Buffer_for_Sigfox,0x00,sizeof(Buffer_for_Sigfox)); // limpiamos el buffer
      Serial.println("Dato enviado al backend de Sigfox");
    }
    else
    {
      memset(Buffer_for_Sigfox,0x00,sizeof(Buffer_for_Sigfox)); // limpiamos el buffer
      Serial.println("Dato no enviado.");
    }
  }
  else
  {
    // hacer otra tarea
  }
}
// Desarrollo de funciones
// función para agregar flotantes al payload
void Smelpro_RAK3172_Sigfox_addFloat(float var1)      
{
  byte* a1 = (byte*) &var1;     //convertimos el dato a bytes
  char buff_float[30];
  String str_float;
  //agregamos al comando AT$SF= nuestra informacion a enviar
  for(int i=0;i<4;i++)
  {
    str_float = String(a1[i], HEX);    //convertimos el valor hex a string 
    str_float.toCharArray(buff_float, sizeof(buff_float));
    if(str_float.length()<2)
    {
      char z[2] = "0";
      strcat(z,buff_float);
      strcat(Buffer_for_Sigfox, z);
      //bufer+=0+str1;            //si no, se agrega un cero
    }
    else
    {
      strcat(Buffer_for_Sigfox, buff_float);  //si esta completo, se copia tal cual
    }
  }
}
//función para agregar enteros al payload (hasta 255)
void Smelpro_RAK3172_Sigfox_addInt(int var2)          
{
  byte* a2 = (byte*) &var2;     //convertimos el dato a bytes
  char buff_int[30];
  String str_int;
  str_int=String(a2[0], HEX);      //convertimos el valor hex a string
  str_int.toCharArray(buff_int, sizeof(buff_int));
  //verificamos si nuestro byte esta completo
  if(str_int.length()<2)
  {
    char z[2] = "0";
    strcat(z,buff_int);
    strcat(Buffer_for_Sigfox, z);             //si no, se agrega un cero
  }
  else
  {
    strcat(Buffer_for_Sigfox, buff_int);     //si esta completo, se copia tal cual
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
		sprintf(hexByte, "%x", *n);

		if (strlen(hexByte) == 1)
		{
			sprintf(hexByte, "0%x", *n);
		}
		strcat(str_n, hexByte);
		n--;
	}
  strcat(str_n, "\0");
  strcpy(text, str_n);
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
