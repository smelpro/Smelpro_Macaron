/*
 * Macaron_Sigfox_Send_Bit_Satus.ino
 *
 *  Creado: May 30, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;
// Configuración Serial para Sigfox
#define RAK3172_SIGFOX_BAUDRATE 9600

// variables
char Buffer_for_Sigfox[200];
char textAux[20];
volatile bool flag_button = false;
// Prototipo de funciones
void IRAM_ATTR ISR_INT(void);


void setup() {
  // Inicialización del depurador Serial
  Serial.begin(115200);
  // GPIO48 como interrupción por flanco de bajada
  attachInterrupt(PIN_BTN_USER, ISR_INT, FALLING); 
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  // Inicialización del RAK3172 mediante comnados AT
  while(!Macaron.RAK3172_SigfoxInit(RAK3172_SIGFOX_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializado...\r\n");
    delay(500);
  }
  Serial.println("RAK3172 for Sigfox started!\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  //Macaron.RAK3172_Echo();
  // Configuración del módulo SIGFOX (RC, tx_power, encryption_key, payload_encryption)
  Macaron.RAK3172_SigfoxSetConfig(4, 22, false, false);
  Blink_Led(3, 100);
}

void loop() 
{
  // Si se presiona el botón, envía el bit de estado 
  if(flag_button == true)
  {
    flag_button = false;
    //Macaron.RAK3172_SigfoxSendBitStatus(bit= 1 ó 0, intentos))
    if(Macaron.RAK3172_SigfoxSendBitStatus(1, 4))
    {
      Blink_Led(3, 100);
      Serial.println("Dato enviado al backend de Sigfox.");
    }
    else
    {
      Serial.println("Dato no enviado.");
    }
  }
  else
  {
    // Hacer otra tarea
  }
}
// Desarrollo de funciones
// Función de interrupción del botón de usuario
void IRAM_ATTR ISR_INT(void)
{
  //Tarea de interrupción
  flag_button = true;
}
//función para agregar flotantes al payload
void Smelpro_RAK3172_Sigfox_add_float(float var1)      
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
void Smelpro_RAK3172_Sigfox_add(int var2)          
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
