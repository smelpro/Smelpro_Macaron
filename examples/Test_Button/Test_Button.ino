/*
 *  Test_Button.ino
 *
 *  Creado: Nov 1, 2024
 *      Autor: Smelpro
 *
 *  web: www.smelpro.com
 */

#include "Smelpro_Macaron.h"

int led = PIN_LED_BLINK;         // GPIO 2
int boton = PIN_BTN_USER	;      // GPIO 48

void setup()
{
  pinMode(led, OUTPUT);     // Pin como salida
  pinMode(boton, INPUT);    // Pin como entrada
} 

void loop()
{
  if(digitalRead(boton)==1)       // Verifica si se presiono el pulsador
    {digitalWrite(led, LOW);}     // Enciende el led
  else
    {digitalWrite(led, HIGH);}    // Si no se presiona apaga el led  
}