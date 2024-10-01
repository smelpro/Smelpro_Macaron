/*
 * Macaron_BME280_and_HTTP_Tago.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

// Librerias ANSI C
#include <string.h>
#include <stdint.h>
// Libreria para el protocolo HTTP
#include <HTTPClient.h>
// Libreria para la conexión a WiFi
#include <WiFi.h>
// Libreria de DFRobot para el manejo del sensor BME280
#include "DFRobot_BME280.h"
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

// Definiciones
#define SEA_LEVEL_PRESSURE 1015.0f // constante para hallar la altitud en mmsnm
#define TIME_SEND     60000 // tiempo de envío de datos a Tago en milisegundos

typedef DFRobot_BME280_IIC BME; // ******** Utilice abreviaturas en lugar de nombres completos. ********
/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME bme(&Wire, 0x76); // 0x76 por defecto en la placa de desarrollo Smelpro Macaron

// Credenciales de red WIFI
const char *ssid     = "Smelpro";
const char *password = "contraseña";

// Credenciales de Tago 
#define SERV_HTTP "api.tago.io"
#define PORT_HTTP 80
#define TOKEN     "5088edae-88b6-4474-94eb-0fcdf0bc9a5f"

// Ciente HTTP
WiFiClient espClient;
// Objeto HTTP
HTTPClient http;

// Prototipos de funciones
void IRAM_ATTR ISR_INT(void);
void printLastOperateStatus(BME::eStatus_t eStatus);
void WiFi_Connect(void);
void WiFi_Reconncet(void);

// Variables de aplicacion
int respuestaHTTP = 0;
char dataJSON[300];
char get_tago[220];
uint8_t led_Tago;
bool flag_button = false;
unsigned long Time_ = 0;

void setup()
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK, OUTPUT);
  pinMode(PIN_BTN_USER, INPUT);
  digitalWrite(PIN_LED_BLINK, LOW);
  // GPIO48 como interrupción por flanco de bajada
  attachInterrupt(PIN_BTN_USER, ISR_INT, FALLING); 
  // Inicialización del sensor BME280
  Serial.println("bme read data test with ubidots");
  while (bme.begin() != BME::eStatusOK)
  {
    Serial.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bme begin success");
  delay(100);
  // Iniciar conexión WIFI
  WiFi_Connect();
  Blink_Led(3, 100);
}

void loop()
{
  // envío de datos cada 1 minuto (TIME_SEND)
  if((millis()-Time_) > TIME_SEND)
  {
    Time_ = millis(); // actualizo la variable de tiempo
    // Leectura de los parametros del sensor BME280
    float temp = bme.getTemperature();
    uint32_t press = (bme.getPressure());
    float alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
    float humi = bme.getHumidity();
    // Si se presiona un botón se acciona un led en tago
    if (flag_button == true)
    {
      led_Tago = !led_Tago;
      flag_button = false;
    }
    if (WL_CONNECTED == WiFi.status())
    {
      sprintf(dataJSON, "[{\"variable\": \"temperatura\",\"value\": %.2f},{\"variable\": \"presion\",\"value\": %d},{\"variable\": \"humedad\",\"value\": %.2f},{\"variable\": \"altitud\",\"value\": %.2f},{\"variable\": \"led\",\"value\": %d}]", temp, press, humi, alti, led_Tago);
      Serial.printf("Data JSON: %s\r\n", dataJSON);
      http.begin(espClient, SERV_HTTP, PORT_HTTP, "/data");
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Device-Token", TOKEN);
      Serial.printf("Iniciando solicitud POST HTTP\r\n");

      // Solicitud POST HTTP
      respuestaHTTP = http.POST(dataJSON);
      if (respuestaHTTP == 202)
      {
        Serial.printf("Datos enviados al servidor con exito\r\n");
      }
      else
      {
        Serial.printf("Codigo de respuesta: %d\r\n", respuestaHTTP);
        Serial.printf("Error POST HTTP\r\n");
      }
      //**********************************************************************************************
      // Metodo GET
      Serial.printf("Iniciado solicitud GET\r\n");
      http.begin(espClient, SERV_HTTP, PORT_HTTP, "/data?variable=led&query=last_value");
      http.addHeader("Device-Token", TOKEN);
      respuestaHTTP = http.GET();
      if (respuestaHTTP == 200)
      {
        Serial.printf("Datos recibidos del servidor con exito\r\n");
        //Serial.printf("Codigo de respuesta: %d\r\n", respuestaHTTP);
        //Serial.printf("Data recibida: ");
        String stringData = http.getString();
        //Serial.println(stringData);
        stringData.toCharArray(get_tago, 220);
        if(strstr(get_tago, "\"value\":0,"))
        {
          Serial.printf("Led apagado\r\n");
          digitalWrite(PIN_LED_BLINK, LOW);
        }
        else if(strstr(get_tago, "\"value\":1,"))
        {
          Serial.printf("Led prendido\r\n");
          digitalWrite(PIN_LED_BLINK, HIGH);
        }
        Serial.printf("\r\n\r\n");
      }
      else
      {
        Serial.printf("Error en la solicitud GET HTTP\r\n");
      }
    }
    else
    {
      // Reconetar WiFi en caso de desconexion
      WiFi_Reconncet();
    }
  }
  else
  {
    // Hacer otra tarea
  }
}

// Desarrollo de funciones
void IRAM_ATTR ISR_INT(void)
{
  // Tarea de interrupción
  flag_button = true;
}
// Función para conectectarse a la red por WiFi
void WiFi_Connect(void)
{
  // WIFI en modo estación
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Inicio de conexion a la red WiFi..");
  while (WL_CONNECTED != WiFi.status())
  {
    Serial.println("Conectando a WiFi...");
    delay(1000);
  }
  Serial.println("Conectado a la red WiFi");
  Serial.print("Local IP: ");
  Serial.print(WiFi.localIP());
  Serial.print(" | RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.print("| CH: ");
  Serial.println(WiFi.channel());
}
// Función de reconexión pata WiFi
void WiFi_Reconncet(void)
{
  // Desconexión de WiFi
  WiFi.disconnect();
  // Configurar modo WIFI en modo estación
  WiFi.mode(WIFI_STA);
  // Configurando credenciales de red
  WiFi.begin(ssid, password);
  Serial.println("Inicio de reconexion a la red WiFi..");
  while (WL_CONNECTED != WiFi.status())
  {
    Serial.println("Conectando a WiFi...");
    delay(1000);
  }
  Serial.println("Reconexion a la red WiFi correcta");
}
// Función para leer el estado de lectura del sensor BME280
void printLastOperateStatus(BME::eStatus_t eStatus)
{
  switch (eStatus)
  {
  case BME::eStatusOK:
    Serial.println("everything ok");
    break;
  case BME::eStatusErr:
    Serial.println("unknow error");
    break;
  case BME::eStatusErrDeviceNotDetected:
    Serial.println("device not detected");
    break;
  case BME::eStatusErrParameter:
    Serial.println("parameter error");
    break;
  default:
    Serial.println("unknow status");
    break;
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