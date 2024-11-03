/*
 * Test_LPSend_LoRaWAN.ino
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

#define TIME_SEND  60000 // tiempo en milisegundos de envio de datos al Gateway por LoRaWAN
// Configuración Serial para el RAK3172
#define RAK3172_LORA_BAUDRATE 115200

// Configuración Modo de conexión y Clase (LoRaWAN)
#define MODO_CONEXION   OTAA_CONECTION
#define CLASE           CLASE_C
// Configuración de la banda de frecuencia
// Las bandas libres de trabajo en Perú son AS923 Y AU915
banda_t BANDA = AS923_1;
// Configuración de velocidad de transmisión (data rate)
// AS923: El rango de valores de la velocidad de datos es 2-5 (Data_Rate_2 - Data_Rate_5).
// AU915 / LA915: El rango de valores de la velocidad de datos es 0-6 (Data_Rate_0 - Data_Rate_6).
data_rate_t DATA_RATE = Data_Rate_5;
// Configuración de la potencia de transmisión 
// EU868 / RU864 / KR920 / AS923 / CN470, el rango de valores de potencia de transmisión es 0-7(TXP0 - TXP7) donde la potencia mas alta comienza desde cero.
// US915 / AU915, el rango de valores de potencia de transmisión es 0-14(TXP0 - TXP14).
transmit_power_t TXP_POWER = TXP0;
// Configuración de DevEui y AppKey
const char* DevEui = "AC1F09FFFE0D6D09";
const char* AppKey = "AC1F09FFFE0D6D09AC1F09FFF9153172";
// variables
int size_data_gateway;
uint8_t canal = 0;
unsigned long Time_ = 0;

void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  // Inicialización del RAK3172 mediante comnados AT
  while(!Macaron.RAK3172_LoraInit(RAK3172_LORA_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializado...\r\n");
    delay(500);
  }
  Serial.println("RAK3172 iniciado!\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  //Macaron.RAK3172_Echo();
  // Inicio de la conexión con el Gateway
  // Configuración del DEVUI, APPKEY, CLASE, MODO_DE_CONEXION, BANDA, VELOCIDAD_DE_DATOS, POTENCIA_DE_TRANSMISION
  while(!Macaron.RAK3172_LoraWanSetConfig(DevEui,AppKey,CLASE,OTAA_CONECTION,BANDA,DATA_RATE,TXP_POWER))
  {
    Serial.println("RAK3172 seteando...\r\n");
    delay(1000);
  }
  Serial.println("RAK3172 seteado para LoRaWAN!\r\n");
  while(true)
  {
    Serial.println("RAK3172 conectado...\r\n");
    delay(100);
    // Macaron.RAK3172_LoraWanJoin(intervalo de reintentos para unirse al gateway(segundos), número de intentos)
    if(Macaron.RAK3172_LoraWanJoin(15, 3))
    {
      Serial.println("RAK3172 conectado al Gateway !\r\n");
      break;
    }
  }
  Blink_Led(4, 100);
}

void loop() {
  // envío de datos cada 1 minuto (TIME_SEND)
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    //***************************************************************************
    Serial.println("Enviando el dato con la funcion LPSEND!\r\n");
    // Funcion LPSEND: permite enviar datos mas grandes (long packages) (longitud máxima es 1000 bytes).
    // Para que no se pierdan los datos a enviar, tiene un parametro en la función llamado ack (1->confimación, 0->no confirmación)
    // La desventaja de esta función es que se añade valores hexadecimales al dato a enviar
    // Macaron.RAK3172_LoraWanLPSend(puerto, ack(1->confirmación, 0->no confirmación), payload, intentos, tiempo(milisegundos))
    if(Macaron.RAK3172_LoraWanLPSend(2, 1, "0100", 4, 2000))
    {
      Serial.println("Enviado al Gateway !\r\n");
      Blink_Led(6, 100);
    }
    else
    {
      // En el caso de que el ack = 1
      Serial.println("No hubo confirmacion del dato recibido desde el Gateway !\r\n");
      Serial.println("Aumentar la cantidad de intentos o verificar que el Gateway este en linea\r\n");
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
