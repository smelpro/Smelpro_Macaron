/*
 * Macaron_BME280_and_HTTP_Ubidtots.cpp
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

//Librerias ANSI C
#include <string.h>
#include <stdint.h>
// Libreria para el manejo del protocolo HTTP
#include <HTTPClient.h>
// Libreria para la conexión con la red WiFi
#include <WiFi.h>
// Libreria de DFRobot para el sensor BME280
#include "DFRobot_BME280.h"
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

// Definiciones
#define SEA_LEVEL_PRESSURE    1015.0f // Conatante para hallar la altura en msnm
#define TIME_SEND       60000 // tiempo de envío de datos a ubidots en milisegundos

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********
/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME   bme(&Wire, 0x76);   // 0x76 por defecto en la placa de desarrollo Smelpro Macaron

// Credenciales de red WIFI
const char *ssid     = "Smelpro";
const char *password = "contraseña";

// Credenciales de Ubidots, funciona con cualquiera de las 2 definiciones
//#define SERV_HTTP "things.ubidots.com"
#define SERV_HTTP "industrial.api.ubidots.com"
#define PORT_HTTP 80
#define UBT_TOKEN "BBUS-Z1w5j5b6InWH4Nfry7wwUC1AmjIwpf"

// Ciente HTTP
WiFiClient espClient;
// Objeto HTTP 
HTTPClient http;

//Prototipos de funciones
void IRAM_ATTR Btn_Interrupt (void);
void printLastOperateStatus(BME::eStatus_t eStatus);
void WiFi_Connect(void);
void WiFi_Reconncet(void);

// Variables de aplicación
int respuestaHTTP = 0;
char dataJSON[100];
uint8_t led_ubidots;
bool flag_button = false;
unsigned long Time_ = 0;


void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  pinMode(PIN_BTN_USER,INPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  // GPIO48 como interrupción por flanco de bajada
  attachInterrupt(PIN_BTN_USER, Btn_Interrupt, FALLING);  
  // Inicialización del sensor BME280
  Serial.println("bme read data test with ubidots");
  while(bme.begin() != BME::eStatusOK) {
    Serial.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bme begin success");
  delay(100);
  //Iniciar conexión WIFI
  WiFi_Connect();
  Blink_Led(3, 100);
}

void loop() 
{
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    // Leectura del sensor BME280
    float    temp  = bme.getTemperature();
    uint32_t press = (bme.getPressure()); 
    float    alti  = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
    float    humi  = bme.getHumidity();
    if(flag_button == true)
    {
      led_ubidots = !led_ubidots;
      flag_button = false;
    }
    if(WL_CONNECTED == WiFi.status())
    {
      // Armado de la trama JSON para Ubidots
      sprintf(dataJSON,"{\"temperatura\":%.2f,\"presion\":%d,\"humedad\":%.2f,\"altitud\":%.2f,\"led\":%d}",temp, press, humi, alti, led_ubidots);
      Serial.printf("Data JSON: %s\r\n",dataJSON);
      http.begin(espClient, SERV_HTTP, PORT_HTTP, "/api/v1.6/devices/prueba_http_inti/");
      http.addHeader("X-Auth-Token",UBT_TOKEN);
      http.addHeader("Content-Type", "applicaton/json");
      Serial.printf("Iniciando solicitud HTTP\r\n");

      // Solicitud POST HTTP
      respuestaHTTP = http.POST(dataJSON);
      // Evaluación de la respuesta del servidor
      if(respuestaHTTP == 200)
      {
        Serial.printf("Codigo de respuesta: %d\r\n",respuestaHTTP);
        Serial.printf("Datos enviados al server con exito\r\n");
      }
      else
      {
        Serial.printf("Codigo de respuesta: %d\r\n",respuestaHTTP);
        Serial.printf("Error POST HTTP\r\n");
      }
      //**********************************************************************************************
      // Solicitud del estado del botón de Ubidots por metodo GET
      http.begin(espClient, SERV_HTTP, PORT_HTTP, "/api/v1.6/devices/prueba_http_inti/button/lv");
      http.addHeader("X-Auth-Token",UBT_TOKEN);
      respuestaHTTP = http.GET();
      if(respuestaHTTP == 200)
      {
        Serial.printf("Codigo de respuesta: %d\r\n",respuestaHTTP);
        Serial.printf("Data recibida: ");
        String stringData = http.getString();
        Serial.println(stringData);
        Serial.printf("\r\n\r\n");
        if(stringData == "1.0")
        {
          digitalWrite(PIN_LED_BLINK,HIGH);
        }
        else
        {
          digitalWrite(PIN_LED_BLINK,LOW);
        }
      }
      else
      {
        Serial.printf("Error GET HTTP\r\n");
      }
    }
    else
    {
      // Reconexión de Wifi en caso de desconexión
      WiFi_Reconncet();
    }
  }
}

//Desarrollo de funciones
void IRAM_ATTR Btn_Interrupt(void)
{
  //Tarea de interrupción
  flag_button = true;
}
// Función para la conexión a la red WiFi
void WiFi_Connect(void)
{
  //WIFI en modo estación
  WiFi.mode(WIFI_STA);
  // Credenciales de red
  WiFi.begin(ssid,password);
  Serial.println("Inicio de conexion a AP..");
  while(WL_CONNECTED != WiFi.status())
  {
    Serial.println("Conectando WiFi..");
    delay(1000);
  }
  Serial.println("ESP32 conexion a AP OK :D");
  Serial.print("Local IP: ");
  Serial.print(WiFi.localIP());
  Serial.print(" | RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.print("| CH: ");
  Serial.println(WiFi.channel());
}
// Función para reconexión a la red WiFi
void WiFi_Reconncet(void)
{
  // Desconexión de WiFi
  WiFi.disconnect();
  // Configurar modo WIFI en modo estación
  WiFi.mode(WIFI_STA);
  // Configurando credenciales de red
  WiFi.begin(ssid,password);
  Serial.println("Inicio de reconexion a AP..");
  while(WL_CONNECTED != WiFi.status())
  {
    Serial.println("Conectando WiFi..");
    delay(1000);
  }
  Serial.println("Reconexion con AP correcta");
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