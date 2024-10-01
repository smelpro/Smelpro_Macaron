/*
 * Smelpro_RAK3172.h
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

#include <Arduino.h>

// RAK3172: Modo de conexion y Clase
#define OTAA_CONECTION 1
#define ABP_CONECTION  0
#define CLASE_A        2
#define CLASE_B        3
#define CLASE_C        4

// Configuracion de la banda de frecuencia en modo LoRaWAN
typedef enum
{
  EU433 = 0,
  CN470,
  RU864,
  IN865,
  EU868,
  US915,
  AU915,
  KR920,
  AS923_1,
  AS923_2,
  AS923_3,
  AS923_4
}banda_t;

// Configuracion de la velocidad de datos en modo LoRaWAN
typedef enum
{
  Data_Rate_0 = 0,
  Data_Rate_1,
  Data_Rate_2,
  Data_Rate_3,
  Data_Rate_4,
  Data_Rate_5,
  Data_Rate_6
}data_rate_t;

// Configuracion de la potencia de transmisión en modo LoRaWAN
typedef enum
{
  TXP0 = 0,
  TXP1,
  TXP2,
  TXP3,
  TXP4,
  TXP5,
  TXP6,
  TXP7,
  TXP8,
  TXP9,
  TXP10,
  TXP11,
  TXP12,
  TXP13,
  TXP14
}transmit_power_t;

// Definiciones de ancho de banda en modo LoRa P2P
typedef enum
{
  Bandwidth_125Khz = 0,
  Bandwidth_250Khz,
  Bandwidth_500Khz,
  Bandwidth_7_8Khz,
  Bandwidth_10_4Khz,
  Bandwidth_15_63hz,
  Bandwidth_20_83hz,
  Bandwidth_31_25Khz,
  Bandwidth_41_67Khz,
  Bandwidth_62_5Khz
}Bandwidth_t;

// Definiciones de Code Rate en modo LoRa P2P
typedef enum
{
  Code_Rate_4_5 = 0,
  Code_Rate_4_6,
  Code_Rate_4_7,
  Code_Rate_4_8
}PCR_t;

class Smelpro_RAK3172{
public:
  // Funciones para inicializar la comunicacion serial con el RAK3172, 
  // dependiendo si esta trabajando en modo LoRa o Sigfox
  bool RAK3172_LoraInit(uint64_t baudRate, uint8_t pinRx, uint8_t pinTx);
  bool RAK3172_SigfoxInit(uint64_t baudRate, uint8_t pinRx, uint8_t pinTx);
  // Funciones basicas
  bool RAK3172_SendCmd(const char* cmd, const char* response, long timeOut);
  bool RAK3172_Sleep(uint64_t timeout); //milisegundos
  void RAK3172_Echo(void);
  void ConvertCharToHex(char *texto, uint8_t *array_bytes2, int size);
  int  ParseHexaChar(char A);
  void EmmptyBuffer(void);
  // Funciones para LoRaWAN
  bool RAK3172_LoraWanSetConfig(const char* _devEui, const char* _appKey, uint8_t _class, uint8_t modo_conection, banda_t _banda, data_rate_t dataRate, transmit_power_t TXP);
  bool RAK3172_LoraWanJoin(int16_t timeout, uint16_t attemps); //segundos, intentos
  bool RAK3172_LoraWanSend(uint16_t port,  char* stringHex, uint8_t attemps, uint64_t timeout); //milisegundos
  bool RAK3172_LoraWanLPSend(uint16_t port, uint8_t ack, char* stringHex, uint8_t attemps, uint64_t timeout); //milisegundos
  bool RAK3172_LoraWanRead(int* size_receive, uint8_t *array_bytes, uint8_t* canal);
  void RAK3172_LoraWanGetId(const char* cmd, int* data_receive);
  // Funciones para LoRa P2P
  bool RAK3172_P2PSetConfig(const char* _pfreq, uint8_t _spreading_factor, Bandwidth_t _Bandwidth, PCR_t _Code_Rate, uint16_t _Preamble_Length, uint8_t _Tx_Power);
  bool RAK3172_P2PSend(char* stringHex, uint64_t timeout);
  bool RAK3172_P2PRead(int* size_receive, uint8_t *array_bytes, uint64_t _timeout);
  // Funciones para Sigfox
  void RAK3172_SigfoxGetId(void);
  void RAK3172_SigfoxGetPac(void);
  bool RAK3172_SigfoxSetConfig(uint8_t _rc, uint8_t _tx_power, bool encryption_key, bool payload_encryption);
  bool RAK3172_SigfoxSendBitStatus(uint8_t bit, uint8_t attemps);
  bool RAK3172_SigfoxSendFrame(char* dato, uint8_t attemps);
  bool RAK3172_SigfoxSendASCII(char* dato, uint8_t attemps);
private:
  char textCMD[200];
  int valor;
};

