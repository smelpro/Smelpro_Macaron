/*
 * Test_Read_LoRaWAN.ino
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

// Configuración Modo de conexión y Clase (LoRaWAN)
#define MODO_CONEXION   OTAA_CONECTION
#define CLASE           CLASE_C
#define TIME_RECV       4000 // tiempo de espera para la recepción de datos en milisegundos
// Configuración Serial para el RAK3172
#define RAK3172_LORA_BAUDRATE 115200
// Configuración de la banda de frecuencia
// Las bandas libres de trabajo en Perú son AS923 Y AU915
banda_t BANDA = AS923_1;
// Configuración de velocidad de transmisión (data rate)
// AS923: El rango de valores de la velocidad de datos es 2-5 (Data_Rate_2 - Data_Rate_5).
// AU915 / LA915: El rango de valores de la velocidad de datos es 0-6 (Data_Rate_0 - Data_Rate_6).
data_rate_t DATA_RATE = Data_Rate_5;
// Configuración de la potencia de transmisión 
// EU868 / RU864 / KR920 / AS923 / CN470, el rango de valores de potencia de transmisión es 0-7 donde la potencia mas alta comienza desde cero.
// US915 / AU915 y el rango de valores de potencia de transmisión es 0-14.
transmit_power_t TXP_POWER = TXP0;
// Configuración del DevEui y AppKey
const char* DevEui = "AC1F09FFFE0D6D09";
const char* AppKey = "AC1F09FFFE0D6D09AC1F09FFF9153172";
// Variables
// Máximo de datos que se puede recibir desde el gateway: 103
uint8_t data_gateway_hex[110]; //array de bytes para recepción de datos
int size_data_gateway; // tamaño de datos a recibir
uint8_t canal = 0; // canal de recepción
unsigned long Time_ = 0; // variable de configuración de tiempo

void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  // Inicializacion del RAK3172 mediante comnados AT
  while(!Macaron.RAK3172_LoraInit(RAK3172_LORA_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("Inicializando RAK3172...\r\n");
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
    Serial.println("RAK3172 conectando...\r\n");
    delay(1000);
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
  if((millis()-Time_) > TIME_RECV)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    Serial.println("Iniciando recepcion desde el gateway");
    if(Macaron.RAK3172_LoraWanRead(&size_data_gateway, data_gateway_hex, &canal) == true)
    {
      // Aqui se obtenie la información enviada desde le gateway y el canal de recepción
      PrintHex(size_data_gateway, data_gateway_hex);
      Serial.print("Canal de recepcion: ");
      Serial.println(canal);
      Blink_Led(3, 100);
    }
    else
    {
      Serial.println("No se recibieron datos");
    }
  }
  else
  {
    // Hacer otra tarea
  }
}
// Desarrollo de funciones
// Función para imprimir un array de bytes
void PrintHex(int size, uint8_t *cadena)
{
  size /= 2;
  char text_Aux1[10];
  char text_Aux2[100];
  for(int i = 0; i<size; i++)
  {
    sprintf(text_Aux1, "0x%02X ", cadena[i]);
    strcat(text_Aux2,text_Aux1);
  }
  Serial.println(text_Aux2);
  memset(text_Aux1,0,sizeof(text_Aux1));
  memset(text_Aux2,0,sizeof(text_Aux2));
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
