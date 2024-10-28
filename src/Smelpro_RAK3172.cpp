/*
 * Smelpro_RAK3172.cpp
 *
 *  Creado: May 07, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

#include "Smelpro_RAK3172.h"

// Desarrollo de funciones
// Funcion para inicializar el tranceptor RAK3172 en el modo LoRa
bool Smelpro_RAK3172::RAK3172_LoraInit(uint64_t baudRate, uint8_t pinRx, uint8_t pinTx) {
  bool flag = false;
  typedef enum
  {
    Config_Serial = 0,
    AT,
    RESET,
    AT2
  }state_t;
  state_t LORA = Config_Serial;
  while(flag == false)
  {
    switch(LORA)
    {
      case Config_Serial:
      // Inicializacion de la comunicacion Serial con el transceptor RAK3172
      Serial2.begin(baudRate, SERIAL_8N1, pinRx, pinTx);
      LORA = AT;
      break;
      //*************************
      case AT:
      if(RAK3172_SendCmd("AT", "OK", 1000))
      {
        LORA = RESET;
      }
      break;
      //****************
      case RESET:
      if(RAK3172_SendCmd("ATZ", ".", 1000))
      {
        LORA = AT2;
      }
      break;
      //****************
      case AT2:
      if(RAK3172_SendCmd("AT", "OK", 1000))
      {
        flag = true;
      }
      break;
      //****************
    }
  }
  return true;
}
// Funcion para enviar un comando AT al tranceptor RAK3172 y evaluar la respuesta 
bool Smelpro_RAK3172::RAK3172_SendCmd(const char* cmd, const char* response, long timeOut) {
  boolean result;
  Serial.println(cmd);
  Serial2.println(cmd);
  delay(100);
  Serial2.setTimeout(timeOut); //En milisegundos
  if(Serial2.find(response)){
    Serial.print(response);
    Serial.println(F(" is True ! "));
    result = true;
  }else{
    Serial.print(response);
    Serial.println(F(" is False ! "));
    result = false;
  }
  delay(50);
  return result;
}
// Funcion para configurar al tranceptor RAK3172 en bajo consumo
bool Smelpro_RAK3172::RAK3172_Sleep(uint64_t timeout) //milisegundos
{
  sprintf(textCMD, "AT+SLEEP=%ld", timeout);
  RAK3172_SendCmd(textCMD, "OK", 1);
  return true;
}
// Funcion para depurar mediante comandos AT al tranceptor RAK3172
void Smelpro_RAK3172::RAK3172_Echo(void)
{
  char bufferRak3172[512];
  int lenBuffer = 0;
  while(1)
  {
    if(Serial.available()!=0)
    {
      char a = Serial.read();
      Serial2.print(a);
    }
    else if(Serial2.available()!=0)
    {
      char b = Serial2.read();
      Serial.print(b);
      bufferRak3172[lenBuffer] = b;
      lenBuffer++;
      delayMicroseconds(100);
    }
  }
}
//Funcion para convertir una cadena ASCII a Hexadecimal (array de bytes)
void Smelpro_RAK3172::ConvertCharToHex(char *texto, uint8_t *array_bytes2, int size)
{
  size -= 1;
  size /= 2;
  char *p = &texto[0];
  for(int i=0; i< size;i++) {
    int val = ParseHexaChar(*p++)*16 + ParseHexaChar(*p++);
    array_bytes2[i] = val;
    //Serial.printf("%d\n",val);
    //Serial.println(array_bytes2[i],HEX);
  }
} 
// Funcion para convertir un dato char a hexadecimal
int Smelpro_RAK3172::ParseHexaChar(char A) 
{
  return (A > '9')? (A &~ 0x20) - 'A' + 10: (A - '0');
}
// Funcion para limpiar el buffer de recepcion del tranceptor RAK3172
void Smelpro_RAK3172::EmmptyBuffer(void)
{
  char bufferSIM7000[512];
  int lenBuffer = 0;
  while(Serial2.available()!=0){
    char b = Serial2.read();
    //Serial.print(b);
    bufferSIM7000[lenBuffer] = b;
    lenBuffer++;
    delayMicroseconds(100);
  }
  delay(100);
}
//*********************************** Funciones para trabajar el RAK3172 en modo LoRaWAN ***************************//
// Funcion para configurar los parametros LoRaWAN
bool Smelpro_RAK3172::RAK3172_LoraWanSetConfig(const char* _devEui, const char* _appKey, uint8_t _class, uint8_t modo_conection, banda_t _banda, data_rate_t dataRate, transmit_power_t TXP)
{
  char text[100];
  bool flag = false;
  typedef enum
  {
    AT = 0,
    VERIFY_MODE,
    SET_MODE,
    APPKEY,
    DEVEUI,
    CLASE,
    MODO_CONEXION,
    DATA_RATE,
    TXP_POWER,
    BANDA,
    ACTIVATE_SEND_CONFIRMATION
  }state_t;
  state_t LORAWAN = AT;
  while(flag == false)
  {
    switch(LORAWAN)
    {
      case AT:
      if(RAK3172_SendCmd("AT", "OK", 1000))
      {
        LORAWAN = VERIFY_MODE;
      }
      break;
      //****************
      case VERIFY_MODE:
      EmmptyBuffer();
      // Verificar si esta en modo LoRaWAN(1) o P2P(0)
      if(RAK3172_SendCmd("AT+NWM=?", "AT+NWM=1", 1000))
      {
        LORAWAN = APPKEY;
      }
      else if(RAK3172_SendCmd("AT+NWM=?", "AT+NWM=0", 1000))
      {
        Serial.println("RAK3172 en modo P2P, cambiando a LoRaWAN.....");
        LORAWAN = SET_MODE;
      }
      break;
      //***************
      case SET_MODE:
      EmmptyBuffer();
      // Si estuvo de manera inicial en modo P2P, seteamos a LoRaWAN
      if(RAK3172_SendCmd("AT+NWM=1", "LoRaWAN.", 1000))
      {
        Serial.println("RAK3172 en modo LoRaWAN.....");
        LORAWAN = APPKEY;
      }
      break;
      //***************
      case APPKEY:
      EmmptyBuffer();
      sprintf(text,"AT+APPKEY=%s",_appKey);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        LORAWAN = DEVEUI;
      }
      break;
      //****************
      case DEVEUI:
      EmmptyBuffer();
      sprintf(text,"AT+DEVEUI=%s",_devEui);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        LORAWAN = CLASE;
      }
      break;
      //****************
      case CLASE:
      EmmptyBuffer();
      if(_class == CLASE_A)
      {
        if(RAK3172_SendCmd("AT+CLASS=A", "OK", 1000))
        {
          LORAWAN = MODO_CONEXION;
        }
      }
      else if(_class == CLASE_B)
      {
        if(RAK3172_SendCmd("AT+CLASS=B", "OK", 1000))
        {
          LORAWAN = MODO_CONEXION;
        }
      }
      else if(_class == CLASE_C)
      {
        if(RAK3172_SendCmd("AT+CLASS=C", "OK", 1000))
        {
          LORAWAN = MODO_CONEXION;
        }
      }
      break;
      //****************
      case MODO_CONEXION:
      EmmptyBuffer();
      if(modo_conection == OTAA_CONECTION)
      {
        if(RAK3172_SendCmd("AT+NJM=1", "OK", 1000))
        {
          LORAWAN = DATA_RATE;
        }
      }
      else if(modo_conection == ABP_CONECTION)
      {
        if(RAK3172_SendCmd("AT+NJM=0", "OK", 1000))
        {
          LORAWAN = DATA_RATE;
        }
      }
      break;
      //****************
      case DATA_RATE:
        EmmptyBuffer();
        switch(dataRate)
        {
          case Data_Rate_0:
            if(RAK3172_SendCmd("AT+DR=0", "OK", 1000))
            {
              Serial.println("Data Rate configurado correctamente.");
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
          //*******************************************************************
          case Data_Rate_1:
            if(RAK3172_SendCmd("AT+DR=1", "OK", 1000))
            {
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
          //*******************************************************************
          case Data_Rate_2:
            if(RAK3172_SendCmd("AT+DR=2", "OK", 1000))
            {
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
          //*******************************************************************
          case Data_Rate_3:
            if(RAK3172_SendCmd("AT+DR=3", "OK", 1000))
            {
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
          //********************************************************************
          case Data_Rate_4:
            if(RAK3172_SendCmd("AT+DR=4", "OK", 1000))
            {
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
          //********************************************************************
          case Data_Rate_5:
            if(RAK3172_SendCmd("AT+DR=5", "OK", 1000))
            {
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
          //*********************************************************************
          case Data_Rate_6:
            if(RAK3172_SendCmd("AT+DR=6", "OK", 1000))
            {
              LORAWAN = TXP_POWER;
            }
            else
            {
              Serial.println("Data Rate no configurado.");
              Serial.println("Data Rate no soportado para la frecuencia configurada!");
              while(1);
            }
          break;
        }
      break;
      //***************************************************************
      case TXP_POWER:
        EmmptyBuffer();
        switch(TXP)
        {
          case TXP0:
            if(RAK3172_SendCmd("AT+TXP=0", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*******************************************************************
          case TXP1:
            if(RAK3172_SendCmd("AT+TXP=1", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*******************************************************************
          case TXP2:
            if(RAK3172_SendCmd("AT+TXP=2", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*******************************************************************
          case TXP3:
            if(RAK3172_SendCmd("AT+TXP=3", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //********************************************************************
          case TXP4:
            if(RAK3172_SendCmd("AT+TXP=4", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //********************************************************************
          case TXP5:
            if(RAK3172_SendCmd("AT+TXP=5", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*********************************************************************
          case TXP6:
            if(RAK3172_SendCmd("AT+TXP=6", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP7:
            if(RAK3172_SendCmd("AT+TXP=7", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP8:
            if(RAK3172_SendCmd("AT+TXP=8", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP9:
            if(RAK3172_SendCmd("AT+TXP=9", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP10:
            if(RAK3172_SendCmd("AT+TXP=10", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP11:
            if(RAK3172_SendCmd("AT+TXP=11", "OK", 1000))
            {
              Serial.println("Potencia configurada correctamente.");
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP12:
            if(RAK3172_SendCmd("AT+TXP=12", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP13:
            if(RAK3172_SendCmd("AT+TXP=13", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
          //*****************************************************************
          case TXP14:
            if(RAK3172_SendCmd("AT+TXP=14", "OK", 1000))
            {
              LORAWAN = BANDA;
            }
            else
            {
              Serial.println("Potencia no configurada.");
              Serial.println("Potencia no soportada para la frecuencia configurada!");
              while(1);
            }
          break;
        }
      break;
      //**************************************************
      case BANDA:
      EmmptyBuffer();
      switch(_banda)
      {
        case EU433:
        if(RAK3172_SendCmd("AT+BAND=0", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case CN470:
        if(RAK3172_SendCmd("AT+BAND=1", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case RU864:
        if(RAK3172_SendCmd("AT+BAND=2", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case IN865:
        if(RAK3172_SendCmd("AT+BAND=3", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case EU868:
        if(RAK3172_SendCmd("AT+BAND=4", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case US915:
        if(RAK3172_SendCmd("AT+BAND=5", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case AU915:
        if(RAK3172_SendCmd("AT+BAND=6", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case KR920:
        if(RAK3172_SendCmd("AT+BAND=7", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case AS923_1:
        if(RAK3172_SendCmd("AT+BAND=8", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case AS923_2:
        if(RAK3172_SendCmd("AT+BAND=9", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case AS923_3:
        if(RAK3172_SendCmd("AT+BAND=10", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
        case AS923_4:
        if(RAK3172_SendCmd("AT+BAND=11", "OK", 1000))
        {
          LORAWAN = ACTIVATE_SEND_CONFIRMATION;
        }
        break;
      }
      break;
      //****************
      case ACTIVATE_SEND_CONFIRMATION:
        if(RAK3172_SendCmd("AT+CFM=1", "OK", 1000))
        {
          flag = true;
        }
      break;
    }
  }
  return true;
}
// Funcion para solicitar unirse al gateway 
bool Smelpro_RAK3172::RAK3172_LoraWanJoin(int16_t timeout, uint16_t attemps) //segundos, intentos
{
  sprintf(textCMD, "AT+JOIN=1:0:%d:%d", timeout, attemps);
  if(RAK3172_SendCmd(textCMD, "+EVT:JOINED", ((timeout*attemps*1000)+30))) //milisegundos
  {
    Serial.print("ID de red: ");
    RAK3172_LoraWanGetId("AT+NETID=?", &valor);
    Serial.println(valor);
    return true;
  }
  return false;
}
// Funcion para enviar un dato al gateway en el modo LoRaWAN
bool Smelpro_RAK3172::RAK3172_LoraWanSend(uint16_t port,  char* stringHex, uint8_t attemps, uint64_t timeout) //milisegundos
{ 
  for(uint8_t i = 0; i<attemps; i++)
  {
    sprintf(textCMD, "AT+SEND=%d:%s", port, stringHex);
    if(RAK3172_SendCmd(textCMD, "SEND_CONFIRMED_OK", timeout)) //milisegundos
    {
      return true;
    }
  }
  return false;
}
// Funcion para enviar un dato mas grande (Long Package) al gateway en el modo LoRaWAN
bool Smelpro_RAK3172::RAK3172_LoraWanLPSend(uint16_t port, uint8_t ack, char* stringHex, uint8_t attemps, uint64_t timeout) //milisegundos
{
  for(uint8_t i = 0; i<attemps; i++)
  {
    if(ack == 0)
    {
      sprintf(textCMD, "AT+LPSEND=%d:%d:%s", port, ack, stringHex);
      if(RAK3172_SendCmd(textCMD, "TX_DONE", timeout)) //milisegundos
      {
        return true;
      }
    }
    else if(ack == 1)
    {
      sprintf(textCMD, "AT+LPSEND=%d:%d:%s", port, ack, stringHex);
      if(RAK3172_SendCmd(textCMD, "SEND_CONFIRMED_OK", timeout)) //milisegundos
      {
        return true;
      }
    }
    else
    {
      Serial.println("Valor no valido para ack!!");
      while(1);
    }
  }
  return false;
}
// Funcion para leer un dato desde el gateway
bool Smelpro_RAK3172::RAK3172_LoraWanRead(int* size_receive, uint8_t *array_bytes, uint8_t* canal)
{
  char data[200];
  uint16_t dataindex = 0;
  char* canal_gateway;
  char* dato_gateway;
  typedef enum
  {
    Send_Lorawan = 0,
    Emmpty_Buffer,
    Send_Comand,
    Read_data_gateway
  }state_t;
  state_t state_machine = Emmpty_Buffer;
  bool flag_while = false;
  bool flag_return = false;
  while(flag_while == false)
  {
    switch(state_machine)
    {
      case Send_Lorawan:
      // Se envia un dato arbitrario al gateway para luego solicitar informacion
      delay(1000);
      if(RAK3172_LoraWanSend(2, "BB", 3, 2000))
      {
        Serial.println("Esperando dato!\r\n");
        delay(200);
        state_machine = Emmpty_Buffer;
      }
      break;
      //***************************
      case Emmpty_Buffer:
      // Una vez enviado el dato se procede a limpiar el buffer de recepcion
      memset(data,0x00,sizeof(data));
      state_machine = Send_Comand;
      break;
      //*********************
      case Send_Comand:
      // Espero la confirmacion del RAK3172 por si ha recibido un dato desde el gateway
      EmmptyBuffer();
      Serial2.println("AT+RECV=?");
      EmmptyBuffer();
      delay(200);
      state_machine = Read_data_gateway;
      break;
      //**********************
      case Read_data_gateway:
      // Lectura de datos enviados desde el gateway
      if(Serial2.available())
      {
        char character = Serial2.read();
        if (character != '\n')
        {
          data[dataindex] = character;
          dataindex++;
        }
        else
        {
          // Aqui se desentrama la información del Gateway
          // ejemplo: AT+RECV=5:11223344
          canal_gateway = strtok(data, "=");
          canal_gateway = strtok(NULL, ":");
          dato_gateway = strtok(NULL, "\n");
          //Serial.print("Canal: ");
          //Serial.println(canal_gateway);
          //Serial.print("Dato: ");
          //Serial.println(dato_gateway);
          *canal = atoi(canal_gateway);
          *size_receive = strlen(dato_gateway);
          if(*size_receive > 1)
          {
            // Se convieten los datos de ASCCI a un array de bytes
            ConvertCharToHex(dato_gateway, array_bytes, strlen(dato_gateway));
            memset(dato_gateway,0,sizeof(dato_gateway));
            memset(canal_gateway,0,sizeof(canal_gateway));
            EmmptyBuffer();
            flag_return = true;
            flag_while = true;
            dataindex = 0;
          }
          else
          {
            // No se recibieron datos
            flag_return = false;
            flag_while = true;
            dataindex = 0;
          }
        }
      }
      break;
      //**********************+
    }
  }
  return flag_return;
}
// Funcion para obtener el ID de red del Gateway
void Smelpro_RAK3172::RAK3172_LoraWanGetId(const char* cmd, int* data_receive)
{
  char Buf[50];
  Serial2.println(cmd);
  char* data_R;
  int dato1 = 0;
  String data = Serial2.readStringUntil('\n');
  data = Serial2.readStringUntil('\n');
  data.toCharArray(Buf, 50);
  data_R = strtok(Buf, "=");
  data_R = strtok(NULL, "\0");
  //Serial.println(data_R);
  dato1 = atoi(data_R);
  *data_receive = dato1;
  delay(50);
}
//************************************* Funciones para trabajar el transceptor RAK3172 en modo P2P ***************************//
// Funcion para configurar los parametros P2P
bool Smelpro_RAK3172::RAK3172_P2PSetConfig(const char* _pfreq, uint8_t _spreading_factor, Bandwidth_t _Bandwidth, PCR_t _Code_Rate, uint16_t _Preamble_Length, uint8_t _Tx_Power)
{
  char text[100];
  bool flag = false;
  typedef enum
  {
    AT = 0,
    VERIFY_MODE,
    SET_MODE,
    PFREQ,
    PSF,
    BANDWIDTH,
    PCR,
    PPL,
    PTP
  }state_t;
  state_t P2P = AT;
  while(flag == false)
  {
    switch(P2P)
    {
      case AT:
      if(RAK3172_SendCmd("AT", "OK", 1000))
      {
        P2P = VERIFY_MODE;
      }
      break;
      //****************
      case VERIFY_MODE:
      // Verificar si esta en modo LoRaWAN(1) o P2P(0)
      if(RAK3172_SendCmd("AT+NWM=?", "AT+NWM=0", 1000))
      {
        P2P = PFREQ;
      }
      else if(RAK3172_SendCmd("AT+NWM=?", "AT+NWM=1", 1000))
      {
        Serial.println("RAK3172 en modo LoRaWAN, cambiando a P2P.....");
        P2P = SET_MODE;
      }
      break;
      //***************
      case SET_MODE:
      // Si estuvo de manera inicial en modo LoRaWAN, seteamos a P2P
      if(RAK3172_SendCmd("AT+NWM=0", "P2P.", 1000))
      {
        Serial.println("RAK3172 en modo P2P.....");
        P2P = PFREQ;
      }
      break;
      //***************
      case PFREQ:
      sprintf(text,"AT+PFREQ=%s",_pfreq);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        P2P = PSF;
      }
      break;
      //*************** 
      case PSF:
      sprintf(text,"AT+PSF=%d",_spreading_factor);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        P2P = BANDWIDTH;
      }
      break;
      //****************
      case BANDWIDTH:
      sprintf(text,"AT+PBW=%d",_Bandwidth);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        P2P = PCR;
      }
      break;
      //****************
      case PCR:
      sprintf(text,"AT+PCR=%d",_Code_Rate);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        P2P = PPL;
      }
      break;
      //****************
      case PPL:
      sprintf(text,"AT+PPL=%d",_Preamble_Length);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        P2P = PTP;
      }
      break;
      //****************
      case PTP:
      sprintf(text,"AT+PTP=%d",_Tx_Power);
      if(RAK3172_SendCmd(text, "OK", 1000))
      {
        flag = true;
      }
      break;
      //****************
    }
  }
  return true;
}
// Funcion para enviar un dato en modo P2P
bool Smelpro_RAK3172::RAK3172_P2PSend(char* stringHex, uint64_t timeout) //milisegundos
{
  char text[100];
  sprintf(text,"AT+PSEND=%s",stringHex);
  if(RAK3172_SendCmd(text, "OK", timeout))
  {
    return true;
  }
  else
  {
    return false;
  }
}
// Funcion para leer un dato en el modo P2P
bool Smelpro_RAK3172::RAK3172_P2PRead(int* size_receive, uint8_t *array_bytes, uint64_t _timeout)
{
  char data[200];
  char data_aux[100];
  uint16_t dataindex = 0;
  char* dato_P2P;
  typedef enum
  {
    Emmpty_Buffer = 0,
    Send_Comand,
    Read_data_P2P
  }state_t;
  state_t state_machine = Emmpty_Buffer;
  bool flag_while = false;
  bool flag_return = false;
  while(flag_while == false)
  {
    switch(state_machine)
    {
      case Emmpty_Buffer:
      // Una vez enviado el dato se procede a limpiar el buffer de recepcion
      memset(data,0x00,sizeof(data));
      state_machine = Send_Comand;
      break;
      //*********************
      case Send_Comand:
      // Espero la confirmacion del RAK3172 por si ha recibido un dato desde el gateway
      EmmptyBuffer();
      sprintf(data_aux,"AT+PRECV=%d",_timeout);
      if(RAK3172_SendCmd(data_aux, "OK", 2000))
      {
        delay(10);
        EmmptyBuffer();
        state_machine = Read_data_P2P;
      }
      break;
      //**********************
      case Read_data_P2P:
      // Lectura de datos enviados desde el gateway
      if(Serial2.available())
      {
        char character = Serial2.read();
        if (character != '\n')
        {
          data[dataindex] = character;
          dataindex++;
        }
        else
        {
          if(strstr(data,"+EVT:RXP2P:")!=0)
          {
            Serial.println("Data recibida");
            // Aqui se desentrama la información en modo P2P
            // ejemplo: +EVT:RXP2P:-31:12:0BCF03DE083213CA
            dato_P2P = strtok(data, ":");
            dato_P2P = strtok(NULL, ":");
            dato_P2P = strtok(NULL, ":");
            dato_P2P = strtok(NULL, ":");
            dato_P2P = strtok(NULL, "\n");
            //Serial.print("Dato: ");
            //Serial.println(dato_P2P);
            *size_receive = strlen(dato_P2P);
            // Se convieten los datos de ASCCI a un array de bytes
            ConvertCharToHex(dato_P2P, array_bytes, strlen(dato_P2P));
            memset(dato_P2P,0,sizeof(dato_P2P));
            EmmptyBuffer();
            flag_return = true;
            flag_while = true;
            dataindex = 0;
          }
          else if(strstr(data,"RECEIVE TIMEOUT")!=0)
          {
            Serial.println("Tiempo exedido, no se recibio datos");
            // No se recibieron datos
            flag_return = false;
            flag_while = true;
            dataindex = 0;
          }
        }
      }
      break;
      //**********************+
    }
  }
  return flag_return;
}
//****************************** Funcion para inicializar el tranceptor RAK3172 en modo Sigfox *********************************//
bool Smelpro_RAK3172::RAK3172_SigfoxInit(uint64_t baudRate, uint8_t pinRx, uint8_t pinTx)
{
  bool flag = false;
  uint8_t contador = 0;
  typedef enum
  {
    Config_Serial = 0,
    AT
  }state_t;
  state_t SIGFOX = Config_Serial;
  while(flag == false)
  {
    switch(SIGFOX)
    {
      case Config_Serial:
      // Inicializacion de la comunicacion Serial con el transceptor RAK3172
      Serial2.begin(baudRate, SERIAL_8N1, pinRx, pinTx);
      SIGFOX = AT;
      break;
      //******************************
      case AT:
      if(RAK3172_SendCmd("AT", "OK", 1000))
      {
        flag = true;
      }
      else
      {
        contador++;
        if(contador == 4)
        {
          Serial.println("parametros de comunicacion serial incorrectos");
          return false;
        }
      }
      break;
    }
  }
  return true;
}
// Funcion para obtener el ID en modo Sigfox
void Smelpro_RAK3172::RAK3172_SigfoxGetId(void)
{
  char data[100];
  uint16_t dataindex = 0;
  char* dato_P2P;
  typedef enum
  {
    Get_Id = 0,
    Read_Id,
    EmptyBuffer
  }state_t;
  state_t state_machine = Get_Id;
  bool flag_while = false;
  while(flag_while == false)
  {
    switch(state_machine)
    {
      case Get_Id:
      EmmptyBuffer();
      Serial2.println("AT$ID=?");
      state_machine = Read_Id;
      break;
      //****************
      case Read_Id:
      if(Serial2.available())
      {
        char character = Serial2.read();
        if (character != '\n')
        {
          data[dataindex] = character;
          dataindex++;
        }
        else
        {
          dataindex = 0;
          Serial.print("ID: ");
          Serial.println(data);
          state_machine = EmptyBuffer;
        }
      }
      break;
      //****************
      case EmptyBuffer:
      EmmptyBuffer();
      flag_while = true;
      break;
    }
  }
}
// Funcion para obtener el PAC en modo Sigfox
void Smelpro_RAK3172::RAK3172_SigfoxGetPac(void)
{
  char data[100];
  uint16_t dataindex = 0;
  char* dato_P2P;
  typedef enum
  {
    Get_PAC = 0,
    Read_PAC,
    EmptyBuffer
  }state_t;
  state_t state_machine = Get_PAC;
  bool flag_while = false;
  while(flag_while == false)
  {
    switch(state_machine)
    {
      case Get_PAC:
      EmmptyBuffer();
      Serial2.println("AT$PAC=?");
      state_machine = Read_PAC;
      break;
      //****************
      case Read_PAC:
      if(Serial2.available())
      {
        char character = Serial2.read();
        if (character != '\n')
        {
          data[dataindex] = character;
          dataindex++;
        }
        else
        {
          dataindex = 0;
          Serial.print("PAC: ");
          Serial.println(data);
          state_machine = EmptyBuffer;
        }
      }
      break;
      //****************
      case EmptyBuffer:
      EmmptyBuffer();
      flag_while = true;
      break;
    }
  }
}
// Funcion de configuracion de parametros en modo SIGFOX
bool Smelpro_RAK3172::RAK3172_SigfoxSetConfig(uint8_t _rc, uint8_t _tx_power, bool encryption_key, bool payload_encryption)
{
  char text[100];
  bool flag = false;
  typedef enum
  {
    AT = 0,
    verify_region,
    config_region,
    config_tx_power,
    set_encryption_key,
    set_payload_encryption
  }state_t;
  state_t SIGFOX= AT;
  while(flag == false)
  {
    switch(SIGFOX)
    {
      case AT:
      if(RAK3172_SendCmd("AT", "OK", 1000))
      {
        SIGFOX = verify_region;
      }
      break;
      //****************
      case verify_region:
      // Verificar en que region esta configurado por defecto
      if(RAK3172_SendCmd("AT$RC=?", "1", 1000))
      {
        if(_rc == 1)
        {
          Serial.println("RAK3172 configurado en la region RC1");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC1");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      else if(RAK3172_SendCmd("AT$RC=?", "2", 1000))
      {
        if(_rc == 2)
        {
          Serial.println("RAK3172 configurado en la region RC2");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC2");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      else if(RAK3172_SendCmd("AT$RC=?", "3", 1000))
      {
        if(_rc == 3)
        {
          Serial.println("RAK3172 configurado en la region RC3");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC3");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      else if(RAK3172_SendCmd("AT$RC=?", "4", 1000))
      {
        if(_rc == 4)
        {
          Serial.println("RAK3172 configurado en la region RC4");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC4");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      else if(RAK3172_SendCmd("AT$RC=?", "5", 1000))
      {
        if(_rc == 5)
        {
          Serial.println("RAK3172 configurado en la region RC5");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC5");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      else if(RAK3172_SendCmd("AT$RC=?", "6", 1000))
      {
        if(_rc == 6)
        {
          Serial.println("RAK3172 configurado en la region RC6");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC6");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      else if(RAK3172_SendCmd("AT$RC=?", "7", 1000))
      {
        if(_rc == 7)
        {
          Serial.println("RAK3172 configurado en la region RC7");
          SIGFOX = config_tx_power;
        }
        else
        {
          Serial.println("RAK3172 configurado en la region RC7");
          Serial.printf("Cambiando a la region RC%d\r\n",_rc);
          SIGFOX = config_region;
        }
      }
      break;
      //***************
      case config_region:
      char text_[20];
      sprintf(text_,"AT$RC=%d",_rc);
      if(RAK3172_SendCmd(text_, "OK", 1000))
      {
        Serial.printf("RAK3172 configurado en la region RC%d\r\n",_rc);
        SIGFOX = config_tx_power;
      }
      break;
      //**************
      case config_tx_power:
      char text2_[20];
      sprintf(text2_,"ATS302=%d",_tx_power);
      if(RAK3172_SendCmd(text2_, "OK", 1000))
      {
        Serial.printf("RAK3172 configurado en la potencia %d dbm\r\n",_tx_power);
        SIGFOX = set_encryption_key;
      }
      break;
      //*******************
      case set_encryption_key:
      //ATS410=?
      if(encryption_key == true)
      {
        if(RAK3172_SendCmd("ATS410=1", "OK", 1000))
        {
          Serial.println("RAK3172 configurado para usar la llave publica.");
          SIGFOX = set_payload_encryption;
        }
      }
      else
      {
        if(RAK3172_SendCmd("ATS410=0", "OK", 1000))
        {
          Serial.println("RAK3172 configurado para usar la llave privada.");
          SIGFOX = set_payload_encryption;
        }
      }
      break;
      //****************************
      case set_payload_encryption:
      //ATS411=?
      if(payload_encryption == true)
      {
        if(RAK3172_SendCmd("ATS411=1", "OK", 1000))
        {
          Serial.println("payload encryption ON");
          flag = true;
        }
      }
      else
      {
        if(RAK3172_SendCmd("ATS411=0", "OK", 1000))
        {
          Serial.println("payload encryption OFF");
          flag = true;
        }
      }
      break;
    } 
  }
  return true;
}
// Funcion para enviar un dato de prueba (0 ó 1) al backend de SIGFOX
bool Smelpro_RAK3172::RAK3172_SigfoxSendBitStatus(uint8_t bit, uint8_t attemps) //0 or 1, intentos
{
  sprintf(textCMD, "AT$SB=%d", bit);
  for(uint8_t i = 0; i<attemps; i++)
  {
    sprintf(textCMD, "AT$SB=%d", bit);
    if(RAK3172_SendCmd(textCMD, "OK", 30000)) //milisegundos
    {
      EmmptyBuffer();
      return true;
    }
  }
  EmmptyBuffer();
  return false;
}
// Funcion para enviar una trama de datos ASCII en formato de bytes
bool Smelpro_RAK3172::RAK3172_SigfoxSendFrame(char* dato, uint8_t attemps) 
{
  for(uint8_t i = 0; i<attemps; i++)
  {
    sprintf(textCMD, "AT$SF=%s", dato);
    if(RAK3172_SendCmd(textCMD, "OK", 30000)) //milisegundos
    {
      EmmptyBuffer();
      return true;
    }
  }
  EmmptyBuffer();
  return false;
}
// Funcion para enviar datos en formato ASCCI
bool Smelpro_RAK3172::RAK3172_SigfoxSendASCII(char* dato, uint8_t attemps) 
{
  uint8_t len = strlen(dato);
  Serial.printf("Tamaño del dato a enviar: %d\r\n",len);
  if(len<=12)
  {
    for(uint8_t i = 0; i<attemps; i++)
    {
      if(len<=9)
      {
        sprintf(textCMD, "AT$SH=%d,%s",len, dato);
      }
      else
      {
        sprintf(textCMD, "AT$SH=%d%s",len, dato);
      }
      if(RAK3172_SendCmd(textCMD, "OK", 30000)) //milisegundos
      {
        EmmptyBuffer();
        return true;
      }
    }
    EmmptyBuffer();
    return false;
  }
  else
  {
    Serial.println("Limite 12 sobrepasado");
    delay(5000);
    return false;
  }
}
