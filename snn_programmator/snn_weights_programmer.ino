#include <EEPROM.h>
#define DATA_X (8)
#define DATA_Y (6)
#define DATA_SIZE (DATA_X*DATA_Y)

#define NEURON_S_ZONE_X (SCREEN_X/DATA_X)
#define NEURON_S_ZONE_Y (SCREEN_Y/DATA_Y)

#define R_NEURON_NUMBER (2)
#define S_NEURON_NUMBER DATA_SIZE
#define SYNAPSES_NUMBER (R_NEURON_NUMBER * S_NEURON_NUMBER)

#define EEPROM_WEIGHTS_ADR 0x01

union bytes_to_float{
  float f;
  unsigned int arr[sizeof(float)];
};

union bytes_to_float transformer;

unsigned short weights_counter = SYNAPSES_NUMBER;
unsigned short weights_adr = EEPROM_WEIGHTS_ADR;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

unsigned char byte_counter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()>0){
    transformer.arr[byte_counter] = Serial.read();
    byte_counter++;
  }
  if (byte_counter >= sizeof(float)){
    byte_counter=0;
    EEPROM.update(weights_adr,transformer.f);
    weights_counter--;
    weights_adr+=sizeof(float);
  }
  if (!weights_counter){
    while (1);
  }
}
