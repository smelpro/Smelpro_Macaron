/*
 * Macaron_SD_Datalogueer_and_BME280_2.ino
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */
/*Este ejemplo guarda los datos del sensor BME280 en la memoria SD CARD en formato CSV o txt con fecha y hora*/

// Libreria para el manejo del protocolo HTTP
#include <HTTPClient.h>
// Libreria para conectarse por WiFi
#include <WiFi.h>
// Librerias para el manejo del protocolo SPI y la memoria SD CARD
#include "FS.h"
#include "SD.h"
#include "SPI.h"
// Libreria para el manejo del sensor BME280
#include "DFRobot_BME280.h"
// Libreria para el manejo de la fecha y hora
#include "time.h"
// Pines de la placa de desarrollo y cabecera del RAK3172
#include "Smelpro_Macaron.h"
// Creación del objeto para el manejo de las funciones
Smelpro_RAK3172 Macaron;

typedef DFRobot_BME280_IIC    BME;    // ******** Utilice abreviaturas en lugar de nombres completos. ********

/**La dirección IIC es 0x77 cuando el pin SDO es alto */
/**La dirección IIC es 0x76 cuando el pin SDO está bajo */
BME   bme(&Wire, 0x76);   // 0x76 por defecto en la placa de desarrollo Smelpro Macaron
#define SEA_LEVEL_PRESSURE    1015.0f // constante para hallar la altitud en mmsnm

// Credenciales de red WIFI
const char *ssid     = "smelpro";
const char *password = "contraseña";


const char* ntpServer = "pool.ntp.org";
/*Para UTC -5.00 -> gmtOffset_sec = -18000, zona horaria de Peru-Lima*/
const long  gmtOffset_sec = -18000;
/*Cambie la compensación de la luz del día en milisegundos. 
Si su país observa el horario de verano , configúrelo en 3600. 
De lo contrario, configúrelo en 0.const int daylightOffset_sec = 3600;*/
const int   daylightOffset_sec = 0;

// Seleccionar el tipo de archivo
#define ARCHIVO  ".csv" // achivo delimitado por comas
//#define ARCHIVO  ".txt" // archivo bloc de notas

// tiempo de adquisición entre dato y dato
// tiempo mínimo recomendado(2seg o 2000ms)
#define TIME_ADCQUISITION 2000 
SPIClass *SDspi = NULL; 

// Variables de aplicación
unsigned long Time_adcquisition = 0;
volatile bool flag_button = false;
bool flag_start_datalogueer = false;
bool flag_cabecera = true;
char dataLogueer[100];
char type_file[50];
char date_and_time[90];
struct tm timeinfo;

//Prototipo de funciones
void IRAM_ATTR BTN_USER_ISR(void);
void printLastOperateStatus(BME::eStatus_t eStatus); // Función de lectura del sensor BME280
void listDir(fs::FS &fs, const char * dirname, uint8_t levels); // Enlistar un directorio
void createDir(fs::FS &fs, const char * path); // Crear un directorio
void removeDir(fs::FS &fs, const char * path); // Remover un directorio
void readFile(fs::FS &fs, const char * path); // Crear y Leer un archivo
void writeFile(fs::FS &fs, const char * path, const char * message); // Crear y Escribir en un archivo
void appendFile(fs::FS &fs, const char * path, const char * message); // Concatenar informacion en un archivo
void renameFile(fs::FS &fs, const char * path1, const char * path2); // Renombrar un archivo
void deleteFile(fs::FS &fs, const char * path); // Eliminar un archivo
void testFileIO(fs::FS &fs, const char * path); // Crear un archivo de ejemplo
void printLocalTime(); // Función de adquisición de fecha y hora
void Blink_led(uint8_t p, uint16_t time); // función parpadeo led

void setup(){
    Time_adcquisition = millis();
    Serial.begin(115200);
    pinMode(PIN_LED_BLINK,OUTPUT);
    pinMode(PIN_BTN_USER,INPUT);
    digitalWrite(PIN_LED_BLINK,LOW);
    attachInterrupt(PIN_BTN_USER, BTN_USER_ISR, RISING);  
      //connect to WiFi
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");
    
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("bme read data test");
    while(bme.begin() != BME::eStatusOK) {
      Serial.println("bme begin faild");
      printLastOperateStatus(bme.lastOperateStatus);
      delay(2000);
    }
    Serial.println("bme begin success");
    delay(100);
    SDspi = new SPIClass(HSPI); 
    SDspi->begin(PIN_HSPI_SCLK, PIN_HSPI_MISO, PIN_HSPI_MOSI, PIN_HSPI_SS); 
    Serial.println("SD Card Init"); 
    if(!SD.begin(PIN_HSPI_SS, *SDspi))
    { 
      Serial.println("Card Mount Failed"); 
      while(1); 
    } 
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        while(1); 
    }
    // Imprime las características de la SD Card
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    //Nombre del datalogueer y formato
    sprintf(type_file, "/Datalogueer_BME280%s", ARCHIVO);
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    // Borrar el archivo en caso este creado
    deleteFile(SD, type_file);
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop()
{
  /*Si se presiona el boton comienza la adquisición de datos o continua si se detuvo, 
  si se presiona de nuevo pausa la adquision*/
  if(flag_button == true)
  {
    // Tiempo de adquisición entre cada dato
    if ((millis()-Time_adcquisition) > TIME_ADCQUISITION)
    {
      Time_adcquisition = millis();
      flag_start_datalogueer = true;
      // Se recomienda presionar el boton de usuario en el parpadeo del led para no interrumpir la escritura en la SD CARD
      Blink_led(3, 40);
      float       temp = bme.getTemperature();
      uint32_t    press = bme.getPressure();
      float       alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
      float       humi = bme.getHumidity();

      Serial.println();
      Serial.println("======== start print ========");
      Serial.print("temperature (unit Celsius): "); Serial.println(temp);
      Serial.print("pressure (unit pa):         "); Serial.println(press);
      Serial.print("altitude (unit meter):      "); Serial.println(alti);
      Serial.print("humidity (unit percent):    "); Serial.println(humi);
      Serial.println("========  end print  ========");
      /*
      // Formato de guardaddo de datos 1: (recomendado para .txt)
      // Fecha-Hora: 26/03/24 - 10:08:20, Temperatura: 28.64, Presion: 98019, Humedad: 56,74, Altitud: 303.34
      // Fecha-Hora: 26/03/24 - 10:08:20, Temperatura: 28.64, Presion: 98019, Humedad: 56,75, Altitud: 303.35
      // Fecha-Hora: 25/03/24 - 00:22:22, Temperatura: 26.31, Presion: 98019, Humedad: 62.50, Altitud: 295.04
      printLocalTime();
      sprintf(date_and_time, "%02d/%02d/%d - %02d:%02d:%02d", timeinfo.tm_mday, (timeinfo.tm_mon+1), (timeinfo.tm_year-100), timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      sprintf(dataLogueer,"Fecha-Hora: %s, Temperatura: %.2f, Presion: %d, Humedad: %.2f, Altitud: %.2f\n",date_and_time, temp, press, humi, alti);
      appendFile(SD, type_file, dataLogueer);
      */
      
      // Formato de guardado de datos 2: (recomendado para .csv)
      // Fecha-Hora, Temperatura, Presion, Humedad, Altitud
      // 26/03/24 - 10:08:20, 28.6, 28.6, 56,7, 303.3
      // 26/03/24 - 10:08:20, 28.6, 28.6, 56,7, 303.3
      if(flag_cabecera == true)
      {
        // Creación de cabecera una sola vez
        flag_cabecera = false;
        appendFile(SD, type_file, "Fecha-Hora, ");
        appendFile(SD, type_file, "Temperatura, ");
        appendFile(SD, type_file, "Presion, ");
        appendFile(SD, type_file, "Humedad, ");
        appendFile(SD, type_file, "Altitud,\n");
      }
      printLocalTime();
      sprintf(date_and_time, "%02d/%02d/%d - %02d:%02d:%02d", timeinfo.tm_mday, (timeinfo.tm_mon+1), (timeinfo.tm_year-100), timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      sprintf(dataLogueer,"%s,  %.2f, %d, %.2f, %.2f\n",date_and_time, temp, press, humi, alti);
      appendFile(SD, type_file, dataLogueer);
      
    }
    else
    {
      delay(1000);
    }
  }
  else if(flag_button == false)
  {
    delay(3000);
    if(flag_start_datalogueer == true)
    {
      Serial.println("Adquisicion de datos terminada");
      readFile(SD, type_file); // leer el archivo 
      Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
      Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
      digitalWrite(PIN_LED_BLINK, LOW);
      flag_start_datalogueer = false;
    }
    else
    {
      Serial.println("Adquisicion detenida o no iniciada");
      delay(3000);
    }
  }
}
// Desarrollo de funciones
// Función de interrupcion
void IRAM_ATTR BTN_USER_ISR(void)
{
  //Tarea de interrupcion
  flag_button = !flag_button;
  Serial.println("Button-User pressed");
}
// Función para obtener el estado del sensor BME280
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
//******************Funciones para el manejo de la SD CARD*************************//
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// Función de parpadeo de led
void Blink_led(uint8_t p, uint16_t time)
{
  for(uint8_t i = 0; i<p; i++)
  {
    digitalWrite(PIN_LED_BLINK, HIGH);
    delay(time);
    digitalWrite(PIN_LED_BLINK, LOW);
    delay(time);
  }
}