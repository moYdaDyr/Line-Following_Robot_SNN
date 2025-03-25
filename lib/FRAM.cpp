#include "Ports.h"
#include "FRAM.h"
#include <SPI.h>

void Init_fram()
{
  pinMode(FRAM_PIN_SO, INPUT);
  pinMode(FRAM_PIN_SI, OUTPUT);
  pinMode(FRAM_PIN_CS, OUTPUT);
  pinMode(FRAM_PIN_SCK, OUTPUT);

    SPI.begin();

    pinMode(FRAM_PIN_CS, OUTPUT);

    digitalWrite(FRAM_PIN_CS,HIGH);

    SPI.beginTransaction(SPISettings (40000000, MSBFIRST, SPI_MODE0));
}

inline unsigned char Read_fram(unsigned short adr){
  unsigned char data;

  digitalWrite(FRAM_PIN_CS,LOW);
  SPI.transfer(READ);
  SPI.transfer(MSB_ADDR_BYTE(adr));
  SPI.transfer(LSB_ADDR_BYTE(adr));
  data = SPI.transfer(0xFF);
  digitalWrite(FRAM_PIN_CS,HIGH);

  return data;
}

inline void Write_fram(unsigned short adr, unsigned char data){
  digitalWrite(FRAM_PIN_CS,LOW);
  SPI.transfer(WREN);
  digitalWrite(FRAM_PIN_CS,HIGH);

  digitalWrite(FRAM_PIN_CS,LOW);
  SPI.transfer(WRITE);
  SPI.transfer(MSB_ADDR_BYTE(adr));
  SPI.transfer(LSB_ADDR_BYTE(adr));
  SPI.transfer(data);
  digitalWrite(FRAM_PIN_CS,HIGH);

  digitalWrite(FRAM_PIN_CS,LOW);
  SPI.transfer(WRDI);
  digitalWrite(FRAM_PIN_CS,HIGH);
}