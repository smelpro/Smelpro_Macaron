/*
 * Macaron_BME280_and_MQTT_Ubidots.ino
 *
 *  Creado: May 07, 2024
 *      Author: Smelpro
 *
 *  web: www.smelpro.com
 */

// Librerias ANSI C
#include <string.h>
// Libreria para la conexion a WiFi
#include <WiFi.h>
// Libreria para la conexion MQTT
#include <PubSubClient.h>
// Libreria para el protocolo I2C
#include <Wire.h>
// Libreria de DFRobot para el manejo del sensor BME280
#include "DFRobot_BME280.h"
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

// Definiciones
#define SEA_LEVEL_PRESSURE    1015.0f // constante para hallar la altitud en mmsnm
#define TIME_SEND       60000 // tiempo de envío de datos a Ubidots en milisegundos

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********

/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME   bme(&Wire, 0x76);   // 0x76 por defecto en la placa de desarrollo Smelpro Macaron

// Credenciales de red WIFI
const char *ssid     = "Smelpro";
const char *password = "contraseña";

// Credenciales de ubidots para la comunicación por MQTT
const char *mqtt_broker    = "industrial.api.ubidots.com";
const char *mqtt_username  = "BBUS-Z1w5j5b6InWH4Nfry7wwUC1AmjIwpf";
const char *mqtt_password  = NULL;
const int  mqtt_port       = 1883;

// topics Ubidots
const char *topic_pub      = "/v1.6/devices/prueba_http_inti";
const char *topic_bt1_sub  = "/v1.6/devices/prueba_http_inti/button/lv";

// ID MQTT 
const char *clientID       = "ESP32-CLIENT-DEV1";

// Ciente HTTP
WiFiClient espClient;
// Objeto HTTP
PubSubClient clientMQTT(espClient);

// Prototipo de funciones
void IRAM_ATTR ISR_INT(void);
void mqtt_callback(char *topic, uint8_t *payload, unsigned int length);
void WiFi_Connect(void);
void WiFi_Reconncet(void);
void MQTT_Connect(void);

// Variables de aplicacion
uint8_t led_ubidots;
bool flag_button = false;
char dataJSON[100];
char dataRcv[20];
char dataTopic[100];
bool data_mqtt_flag = false;
unsigned long Time_ = 0;

void setup() 
{
  // Inicialización del depurador Serial
  Serial.begin(115200);
  pinMode(PIN_LED_BLINK,OUTPUT);
  pinMode(PIN_BTN_USER,INPUT);
  digitalWrite(PIN_LED_BLINK,LOW);
  // GPIO48 como interrupción por flanco de bajada
  attachInterrupt(PIN_BTN_USER, ISR_INT, FALLING); 
  // Inicialización del sensor BME280
  Serial.println("bme read data test with ubidots");
  while(bme.begin() != BME::eStatusOK) {
    Serial.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bme begin success");
  delay(100);
  //Iniciar conexion WIFI
  WiFi_Connect();
  // Iniciar conexión con el broker MQTT de ubidots
  MQTT_Connect();
  Blink_Led(3, 100);
}

void loop() 
{
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
    if(clientMQTT.connected())
    {
      if((millis()-Time_) > TIME_SEND)
      {
        Time_ = millis(); // actualizo la variable de tiempo
        // Formato JSON para Ubidots
        sprintf(dataJSON,"{\"temperatura\":%.2f,\"presion\":%d,\"humedad\":%.2f,\"altitud\":%.2f,\"led\":%d}",temp, press, humi, alti, led_ubidots);
        Serial.println("Enviando datos al broker MQTT");
        clientMQTT.publish(topic_pub, dataJSON); 
        Serial.printf("Data: %s \r\n\r\n",dataJSON);
      }
      // Verifico o si ha llegado un dato por MQTT
      if(data_mqtt_flag == true)
      {
        data_mqtt_flag = false;
        if(strcmp(dataTopic, topic_bt1_sub) == 0)
        {
          if(strcmp(dataRcv, "1.0") == 0)
          {
            digitalWrite(PIN_LED_BLINK,HIGH);
            Serial.printf("Led-User ON\r\n");
          }
          else
          {
            digitalWrite(PIN_LED_BLINK,LOW);
            Serial.printf("Led-User OFF\r\n");
          }
        }
      }
    }
    else
    {
      // Reconecion MQTT
      MQTT_Connect();
    }
    // Actualización de la conexión
    clientMQTT.loop();
  }
  else
  {
    // Reconexion WIFI
    WiFi_Reconncet();
  }
  delay(1000);
}
// Función de recepción de datos por MQTT
void mqtt_callback(char *topic, uint8_t *payload, unsigned int length)
{
  Serial.printf("Dato MQTT recibido \r\n");
  strcpy(dataTopic,topic); 
  memcpy(dataRcv,payload,length); 
  dataRcv[length] = '\0';
  Serial.printf("Topic: %s \r\n",dataTopic);
  Serial.printf("Data: %s \r\n\r\n",dataRcv);
  data_mqtt_flag = true;
}
// Conexión con el broker MQTT
void MQTT_Connect(void)
{
  // Establecer la configuración del broker MQTT
  clientMQTT.setServer(mqtt_broker,mqtt_port);

  // Establecer la función para recibir datos MQTT en ESP32
  clientMQTT.setCallback(mqtt_callback); 

  // Inicilización de la conexión MQTT
  while(!clientMQTT.connected())
  {
    // Iniciamos la conxión con el broker MQTT
    if(clientMQTT.connect(clientID, mqtt_username, mqtt_password))
    {
      Serial.println("Cliente conectado OK");
      // Suscripción a los topics 
      clientMQTT.subscribe(topic_bt1_sub);
    }
    else
    {
      Serial.println("Falla en la conexion del cliente");
      Serial.println(clientMQTT.state());
      delay(2000);
    }
  }
}
// Desarrollo de funciones
void IRAM_ATTR ISR_INT(void)
{
  //Tarea de interrupción
  flag_button = true;
}
// Función para conectectarse a la red por WiFi
void WiFi_Connect(void)
{
  // Configurar modo WIFI en modo estación
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("Inicio de conexion a la red WiFi..");
  while(WL_CONNECTED != WiFi.status())
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
  WiFi.begin(ssid,password);
  Serial.println("Inicio de reconexion a la red WiFi..");
  // Iniciar conexión y consultar el estado de la conexión
  while(WL_CONNECTED != WiFi.status())
  {
    Serial.println("Conectando a WiFi...");
    delay(1000);
  }
  Serial.println("Reconexion a la red WiFi correcta");
}
// Función para leer el estado de lectura del sensor BME280
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