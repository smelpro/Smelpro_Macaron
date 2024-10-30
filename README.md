# 1. Smelpro Macaron 
Es una placa de desarrollo LoRa, LoRaWAN y Sigfox. Por defecto Smelpro Macaron trabaja en la red LoRa y LoRaWAN, pero, mediante una cambio o actualización de firmware en el RAK3172, puede trabajar en la red de Sigfox. 
## 1.1 Comunicación LoRa
Opera en el rango de frecuencias 9XX MHz trabajando con los estandares US915, AU915, KR920 y AS923-1/2/3/4. estos son aplicables para los siguientes paises: Perú, Brazil, Ecuador, Colombia, Venezuela, Guyana, Surinam, Bolivia, Chile, Paraguay, Uruguay, Argentina, Panama, República Dominicana, México, EE.UU, Cánada, Japón Corea del Sur, Tailandia, Veitnam, Camboya, Malasia, Indonesia, Australia y Nueva Zelanda.

Pero, debido a la electrónica empleada, pueden funcionar en otras regiones con rangos de frecuencias distinatas a la original. Esto es posible con la sustitución del módulo RAK3172 usado en la placa de desarrollo con otro respectivo para dicha región y frecuencia necesitada. Haciendo posible su funcionamiento en la mayoria de países y regiones alrededor del mundo como, por ejemplo: la unión europea, China y varios paises de África.

## 1.2 Comunicación Sigfox
Opera en la region RC4, pero debido a su amplio rango de frecuencia de trabajo, puede configurarse en otras regiones, por defecto trabaja en los siguientes paises:
América Latina: Argentina, Chile, Colombia, Costa Rica, Ecuador, El Salvador, Guatemala, Honduras, Nicaragua, Panamá, Paraguay, Perú, Trinidad y Tobago, Uruguay.
Asia Pacífico: Australia, Hong Kong, Indonesia, Malasia, Nueva Zelanda, Singapur, Taiwán, Tailandia.



![Smelpro_Macaron](/images/Inti_Alfa.jpg)

## 1.2. Caracteristicas Generales
* Microcontrolador principal: ESP32-S3-WROOM-1
* Módulo LoRa, LoRaWAN y Sigfox: RAK3172
* Comunicación inalámbrica: WiFi, bluetooth, LoRa y Sigfox.
* Hasta 34 pines GPIO del microcontrolador.
* Diversos periféricos: ADC, DAC, RTC, temporizadores, GPIOs, HMAC, I2C, I2S, LCD, LEDC, MCPWM (control PWM orientado para motores), PCNT (contador de pulsos), RMT (transceptor de control remoto), SD Host, SDMMC Host, SDM (modulación Sigma-Delta), SPI (Flash / Master / Slave), sensores touch, sensor de temperatura interna, TWAI (interfaz automotriz de dos hilos), UART (transmisor / receptor asíncrocno universal) y USB (device / host).
* Diversos tipos de comunicación por red: 
    * Wi-Fi:
        * Wi-Fi
        * ESP-NOW
        * ESP-WIFI-MESH
        * SmartConfig
        * Wi-Fi Easy Connect (DPP)
    * Ethernet
    * Thread
    * ESP-NETIF
    * IP Network layer
    * Application layer
* Diversos tipos de comunicación Bluetooth:
    * Bluetooth Common
    * Bluetooth low energy (BLE)
    * Controller && VHCI
    * ESP-BLE-MESH
    * NimBLE-based Host APIs
* Diversos protocolos de comunicación avanzada: ASIO port, ESP-Modbus, ESP-MQTT, ESP-TLS, ESP HTTP client, ESP Local control, ESP Serial Slave Link, ESP x509 Certificate Bundle, HTTP server, HTTPS server, ICMP echo, mDNS service y Mbed TLS.
* Conector coaxial tipo SMA para una antena externa para una mejor operación.
* Dos entradas USB tipo C, uno dedicado a la programación del microcontrolador y el otro puede usarse con el periférico USB del mismo microcontrolador para aplicaciones que requieran algún tipo de comuniación USB.
* Sensor de temperatura, humedad y presión (BME280).
* Memoria EEPROM de 128Kbit.
* Slot para tarjeta micro SD de tipo push-pull.
* Un led RGB neopixel (WS2812B).
* Dos leds indicadores, uno indica la alimentación de la placa y el otro esta conectado a un pin programable.
* Un boton conectado a un pin programable.
* Conector de comunicación y programación del RAK3172.
* Dos jumper de selección para configurar el funcionamiento del RAK3172, uno para determinar la dirección de la comunicación serial y el otro los estados de los pines de "Reset" y "Boot".
* Dos botones para el reinicio y programación del microcontrolador.

## 1.3. Características Eléctricas
* Voltaje de entrada máximo (VIN): 9.0 V
* Voltaje de funcionamiento digital: 3.3 V
* Voltaje de funcionamiento USB: 5.0 V
* Corriente máxima: 1.2 A
* Voltaje máximo de entrada digital: 3.6 V
* Voltaje mínimo de entrada digital: 3.0 V

## 1.4. Caracteristicas de radiofrecuencia
### 1.4.1 LoRa y LoRaWAN
| Descripcion           | Unidad    |
|----------------------:|-----------|
| Amtena SMA            | 3DBi      |
| Banda                 | US915, AU915, KR920, AS923-1/2/3/4    |
| Potencia              |   20dbm     |
| Versión de LoRaWAN    | V1.0.3 Class A/Class B/Class C       |
| Modo de conexion LoRaWAN | OTTA/ABP    |
|  Técnologia de transmisión  | CSS (Chirp Spread Spectrum) |
|  Modulación           | FSK       |

### 1.4.2 Sigfox
| Descripcion           | Unidad    |
|----------------------:|-----------|
| Amtena SMA            | 3DBi      |
| Potencia              |   20dbm     |
|  Modulación           | DBPSK       |
|  Data Rate            | 600bps     |
|    Frec. Uplink       | 920.8 MHz      |
|   Frec. Downlink      | 922.3 MHz      |
|  Técnologia de transmisión  | UNB (Ultra narrow band) - Sigfox       |

## 1.5. Pinout
![Smelpro_Macaron_pinout](/images/Inti-Alfa_pinout-3.png)


[SMELPRO](https://smelpro.com/)

# 2. Cargar un Sketch a Smelpro Macaron
* Enchufe el cable USB tipo C al conector con el nombre de UART de la placa Smelpro Macaron y luego conecte a la PC.
* Espere a que la PC reconozca al driver que tiene por nombre CP2102
* Abra el IDE de Arduino y seleccione el puerto COM que se le asigno al driver.
* Puede probar cargar un ejemplo como se muestra a continuación.
## 2.1. RGB_test para Smelpro Macaron
Smelpro Macaron dispone de un led RGB de Neopixel, este ejemplo muestra como hacer uso de dicho led.
* Seleccione la placa ESP32S3 Dev Module y seleccione el puerto COM detectado en su PC.
* Y luego cargue el sketch.

![Smelpro_Macaron_tutorial_RGB_test](/Tutorial/Macaron_Test_RGB_2.gif)

## 2.2 Comunicación Serial con el RAK3172
La placa de desarrollo presenta dos metodos de comunicación directa con el RAK3172 mediante comandos AT, la principal forma es utilizando el Sketch que tiene por nombre Test_RAK3172_Echo, este ejemplo hace que el ESP32-S3-WROOM-1 se comporte como un depurador serial y de esa forma, brinda al usuario una comunicación directa con el RAK3172 usando el protocolo serial.

![Smelpro_Macaron_tutorial_Test_RAK3172_Echo](/Tutorial/Macaron_Test_RAK3172_Echo_1.gif)

La segunda forma es cambiado de posicion dos jumpers de cortocircuito para poder usar el conector molex de 6 pines

![Smelpro_Macaron_cambio_Jumpers](/Tutorial/cambio_jumpers_cortocircuito.gif)

De esta manera, se podra depurar directamente al RAK3172 por comandos AT usando un conversor de USB a TTL.

![Smelpro_Macaron_Molex_pinout](/images/Pinout_Molex_Macaron.png)

## 2.3 Como registrar a Smelpro Macaron a la red de LoRaWAN
Para poder registrar un dispositivo a un Gateway, se necesita obtener el DEVEUI y el APPKEY, una forma sencilla es utilizando el ejemplo del tutorial anterior (Smelpro_Macaron - Test_RAK3172_Echo.ino), de esta manera, se pueden obtener dichos parametros enviando los siguientes parametros:
```javascript
AT+NWM=?
AT+APPKEY=?
AT+DEVUI=?
AT+BAND=?
AT+CLASS=?
```
En primer lugar, se debe verificar que el comando AT+NWM=? devuelva el parametro 1, si devuelve 0, quiere dedir que esta en el modo P2P (comunicación punto a punto) y se debe enviar el comando AT+NWM=1 para que entre en el modo LoRaWAN, dependiendo de la banda en la que esta configurado el Gateway, para este ejemplo, el comando AT+BAND=? debe devolver como parametro el numero 8, indicando que esta configurado en la banda de 923MHz, Una vez obtenidos los parametros, se procede a registrar el dispositivo en el Gateway.

## 2.3 Como trabajar a Smelpro Macaron en el modo P2P
En el modo P2P (punto a punto), la tarjeta de desarrollo se puede comunicar con su similar o con otra tarjeta de desarrollo o módulo que tenga un tranceptor LoRa y que maneje los mismos parametros de configuración, dichos parametros se obtienen con los siguientes comandos AT:
```javascript
AT+PFREQ=?
AT+PSF=?
AT+PBW=?
AT+PCR=?
AT+PPL=?
AT+PTP=?
```
Las respuestas deben ser iguales para entablar una comunicación unidireccional o bidirecional, de la misma manera, la comunicación en el modo P2P se puede realizar entre dos o mas dispositivos LoRa, formando una red LoRa IOT tipo Mesh, para mayor información acerca de los comandos visite la pagina web de RakWireless: https://docs.rakwireless.com/RUI3/Serial-Operating-Modes/AT-Command-Manual/#overview.

# 3 Libreria Smelpro_Macaron
La libreria Smelpro_Macaron incorpora una serie de funciones para trabajar de forma sencilla los diversos componentes que tiene la placa de desarrollo asi como una libreria para el control del RAK3172. 

## 3.1. Instalación
Existen diversos metodos de instalar librerias externas:
### 3.1.1. Opcion 1:
* Descomprima este repositorio descargado en formato .ZIP y copie la carpeta dentro de : [Unidad]:\Users[usuario]\Documents\Arduino\libraries
### 3.1.2. Opcion 2:
* Desde el mismo IDE Arduino dirijase a Programa, Incluir Libreria, Añadir biblioteca .ZIP . Localice el repositorio descargado y abrir.
* Una vez instalado recargue el Ide arduino y podra ver que la libreria se encuentra en la sección de ejemplos.
### 3.1.3. Opcion 3:
* Abrir el IDE Arduino, y desde el gestor de descargas de Librerias de Arduino buscar "Smelpro_Macaron" y descargalo
(GIF de la instalacion de la libreria cuando pase los dos dias de prueba que establece arduino como requisito)(opcional).

## 3.2. Prueba de envio de un mensaje hacia el Gateway en el modo LoRaWAN
En este ejemplo, se enviará un mensaje constanate al Gateway LoRaWAN.

![Smelpro_Macaron_example_LoRaWAN](/images/Busqueda_de_ejemplo_para_enviar_un_dato_por_LORAWAN.png)

```javascript
/*
 * Test_Send_LoRaWAN.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creacion del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

#define TIME_SEND  60000 // tiempo en milisegundos de envio de datos al Gateway por LoRaWAN
// Configuracion Serial para el RAK3172
#define RAK3172_LORA_BAUDRATE 115200

// Configuracion Modo de conexion y Clase (LoRaWAN)
#define MODO_CONEXION   OTAA_CONECTION
#define CLASE           CLASE_C
// Configuracion de la banda de frecuencia
banda_t BANDA = AS923_1;
// Configuracion de DevEui y AppKey
const char* DevEui = "AC1F09FFFE0D6D09";
const char* AppKey = "AC1F09FFFE0D6D09AC1F09FFF9153172";
// variables
int size_data_gateway;
uint8_t canal = 0;
unsigned long Time_ = 0;

void setup() 
{
  // Inicializacion del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  while(!Macaron.RAK3172_LoraInit(RAK3172_LORA_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializado...\r\n");
    delay(500);
  }
  Serial.println("RAK3172 iniciado!\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  //Macaron.RAK3172_Echo();
  // Inicializacion del RAK3172 mediante comnados AT
  // Inicio de la conexión con el Gateway
  // Configuracion del DEVUI,APPKEY,Clase, Modo de conexión, Banda de frecuenc
  while(!Macaron.RAK3172_LoraWanSetConfig(DevEui,AppKey,CLASE,OTAA_CONECTION,BANDA))
  {
    Serial.println("RAK3172 seteando...\r\n");
    delay(1000);
  }
  Serial.println("RAK3172 seteado para LoRaWAN!\r\n");
  while(true)
  {
    Serial.println("RAK3172 conectado...\r\n");
    delay(100);
    // Macaron.RAK3172_LoraWanJoin(intervalo de reintentos para unirse al gateway(segundos), numero de intentos)
    if(Macaron.RAK3172_LoraWanJoin(15, 3))
    {
      Serial.println("RAK3172 conectado al Gateway !\r\n");
      break;
    }
  }
  Blink_Led(4, 100);
}

void loop() {
  // envio de datos cada 1 minuto (TIME_SEND)
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    // envio por el puerto 2 la variable constante 100
    if(Macaron.RAK3172_LoraWanSend(2, "0100", 3, 2000))
    {
      Serial.println("Enviado al Gateway !\r\n");
      Blink_Led(3, 500);
    }
  }
  else
  {
    // Hacer otra tarea
  }
}
// Funcion de parpadeo de led
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

```
* La información podra vizualizarse en la base de datos del Gateway
![Smelpro_Macaron_Test_Send_LoRaWAN](/Tutorial/Macaron_Test_Send_LoRaWAN.gif)

## 3.3. Prueba de envio de un mensaje en el modo P2P
Para realizar la comunicación en el modo P2P se necesitarán 2 placas similares o una placa Smelpro-Macaron con un módulo o placa de desarrollo que tenga incorporado un transceptor LoRa.
Los dos ejemplos para realizar la comunicación P2P entre dos placas Smelpro-Macaron tiene por nombre (Test_Send_LoRaP2P.ino y Test_Read_LoraP2P.ino), lo que haran estos ejemplos sera transmitir los datos del sensor BME280 codificador en formato hexadecimal de una placa a otra(comunicacion unidireccional).
En el siguiente tutorial se mostrara un ejemplo de comunicacion en modo P2P entre una placa Smelpro Macaron y un modulo RAK3172 de la empresa RakWireless.

![Smelpro_Macaron_Test_Send_LoRaP2P](/Tutorial/Macaron_Test_Send_LoRaP2P.gif)

## 3.4. Funcionalidad de Smelpro Macaron para la red de Sigfox
La libreria Smelpro_Macaron incorpora las funciones necesarias para trabajar en el modo LoRa P2P, LoRaWAN y Sigfox.

### 3.4.3. Registro del dispositivo en la red de Sigfox
Lo primero que se debe hacer es obtener el ID y el PAC mediante comandos AT, en nuestra libreria Smelpro_Macaron se encuentra un ejemplo basico para obtener el ID y el PAC de fomra sencilla (Test_Sigfox_get_ID_PAC.ino). Una vez obtenido dichos parametros, se podra registrar a la red de Sigfox de las siguientes formas:

* Activación por Buy Sigfox

Disponible sólo para paises de: Colombia, Ecuador, Uruguay, Costa Rica, El Salvador, Panamá,
Ingresar a la pagina Buy Sigfox, Activate my devkit, Seleccionar su País e introducir los codigos ID y PAC: <link> : https://buy.sigfox.com/activate

* Activacion por contacto directo con Unabiz

Si cuentas con una empresa, puedes registrar el dispositivo en el siguiente link: https://docs.google.com/forms/d/e/1FAIpQLSfxpMM65jwkuswvwjMLg_Nq_tgEpG1pNWO-DJAoE64ua6HvbA/viewform


Una vez registrado se podra establecer una comunicación Sigfox

#### 3.4.3.1 Glosario:
* ID : Identificador del dispositivo de 4 bytes (Es único)
* PAC :Código de autorización de transferencia de 8 bytes (Válido una vez). PAC es el certificado de propiedad del objeto, es requerido por la plataforma SIGFOX para validar las solicitudes de registro / transferencia de equipos. Una vez utilizado, la validez del código expira y se asigna un nuevo código.
* El PAC se proporciona en la generacion del nuevo firmware
Ejemplo: 006D8AF2; 10708756FE9515D5

## 3.5. Enviar un mensaje al Backend de Sigfox 

En el siguiente ejemplo se mostrará de forma sencilla como enviar un dato cada vez que se presione el botón de usuario ubicado en la placa de desarrollo.

![Smelpro_Macaron](/images/Busqueda_de_ejemplo_para_enviar_un_dato_a_sigfox.png)

```javascript
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
// Creacion del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;
// Configuracion Serial para Sigfox
#define RAK3172_SIGFOX_BAUDRATE 9600

// variables
char Buffer_for_Sigfox[200];
char textAux[20];
volatile bool flag_button = false;
// Prototipo de funciones
void IRAM_ATTR ISR_INT(void);


void setup() {
  // Inicializacion del depurador Serial
  Serial.begin(115200);
  // GPIO48 como interrupcion por flanco de bajada
  attachInterrupt(PIN_BTN_USER, ISR_INT, FALLING); 
  pinMode(PIN_LED_BLINK,OUTPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  Serial.println("Iniciando sistema");
  while(!Macaron.RAK3172_SigfoxInit(RAK3172_SIGFOX_BAUDRATE, PIN_RX_RAK3172, PIN_TX_RAK3172))
  {
    Serial.println("RAK3172 Inicializado...\r\n");
    delay(500);
  }
  Serial.println("RAK3172 for Sigfox started!\r\n");
  // Prueba mediante comandos AT con el tranceptor RAK3172
  //Macaron.RAK3172_Echo();
  // Inicializacion del RAK3172 mediante comnados AT
  // Configuracion del modo SIGFOX
  // Configuracion del modulo SIGFOX (RC, tx_power, encryption_key, payload_encryption)
  Macaron.RAK3172_SigfoxSetConfig(4, 22, false, false);
  Blink_Led(3, 100);
}

void loop() 
{
  // Si se presiona el boton envia el bit de estado 
  if(flag_button == true)
  {
    flag_button = false;
    //Smelpro_RAK3172_SIGFOX_Send_bit_status(bit= 1 ó 0, intentos))
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
// Funcion de interrupcion del boton de usuario
void IRAM_ATTR ISR_INT(void)
{
  //Tarea de interrupcion
  flag_button = true;
}

void Smelpro_RAK3172_Sigfox_add_float(float var1)      //funcion para agregar flotantes al payload
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

void Smelpro_RAK3172_Sigfox_add(int var2)          //funcion para agregar enteros al payload (hasta 255)
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
// Funcion para convertir datos ASCII a Hexadecimal
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
// Funcion de parpadeo de led
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
```
* La información podrá vizualizarse en el backend de Sigfox. Previamente debe tener asociado a su cuenta el devkit Smelpro Macaron. Se mostrara el ID de su dispositivo asociado y el nuevo codigo PAC que has sido generado tras el registro del dispositivo.
* Desde el Backend de Sigfox se podra vizualizar y procesar la información, la configuración mas importante a considerar son los Callbacks, donde podra redireccionar los mensajes por medio de HTTP o HTTPS a otros servidores.

![Smelpro_Macaron_Test_Send_LoRaP2P](/Tutorial/Macaron_Senb_bit_Sigfox.gif)
