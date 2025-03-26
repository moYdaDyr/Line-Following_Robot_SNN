#include "Ports.h"
#include "Camera.h"
#include "FRAM.h"
#include "SNN.h"

#define MAX_ENGINE_POWER (255)

#define POT_MIN (0)
#define POT_MAX (255)
#define POT_RANGE (POT_MAX-POT_MIN)

#define STOP 0xF0
#define MOVE 0xFF

void Transform_answer(ANSWER_TYPE answer, unsigned char &l_power, unsigned char &r_power ){
  l_power = ((-answer+1.0f)/2.0f)*MAX_ENGINE_POWER;
  r_power = ((answer+1.0f)/2.0f)*MAX_ENGINE_POWER;
}

void Blink(unsigned char led, unsigned char delay_ms){
  analogWrite(led, 255);
  delay(delay_ms);
  analogWrite(led, 0);
  delay(delay_ms);
}

void SleepMode(){
  while(!digitalRead(BUTTON_PIN)){
    Blink(LED_1_PIN, 100);
  }
}

void setup() {
  randomSeed(analogRead(0));
  pinMode(CAMERA_D0_PIN, INPUT);
  pinMode(CAMERA_D1_PIN, INPUT);
  pinMode(CAMERA_D2_PIN, INPUT);
  pinMode(CAMERA_D3_PIN, INPUT);
  pinMode(CAMERA_D4_PIN, INPUT);
  pinMode(CAMERA_D5_PIN, INPUT);
  pinMode(CAMERA_D6_PIN, INPUT);
  pinMode(CAMERA_D7_PIN, INPUT);
  pinMode(CAMERA_D7_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(POT_PIN, INPUT);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

  pinMode(L_ENGINE_IN1_PIN, OUTPUT);
  pinMode(L_ENGINE_IN2_PIN, OUTPUT);
  pinMode(L_ENGINE_POWER_PIN, OUTPUT);

  pinMode(R_ENGINE_IN1_PIN, OUTPUT);
  pinMode(R_ENGINE_IN2_PIN, OUTPUT);
  pinMode(R_ENGINE_POWER_PIN, OUTPUT);

  setResolution(QQVGA);
	setColorSpace(YUV422);
	wrReg(REG_CLKRC,4);
  camInit();
	
  Serial.begin(9600);
  Serial1.begin(9600);
  //pinMode(, OUTPUT);

  Load_weights();
  SleepMode();
}

void loop() {

  if(Serial1.available()){
    unsigned char command = Serial1.read();
    if(command==STOP){
      while (true){
        if(Serial1.available()){
          command = Serial1.read();
          if (command==MOVE) break;
        }
        Blink(LED_2_PIN, 100);
      }
    }
  }

  if (digitalRead(BUTTON_PIN)){
    SleepMode();
  }

  captureImg(160*2,120);
  
  unsigned char l_power, r_power;

  SNN_process();

  float answer = Transform_spikes_to_answer();

  Transform_answer(answer, l_power, r_power);

  analogWrite(LED_1_PIN, l_power);
  analogWrite(LED_2_PIN, r_power);

  digitalWrite(L_ENGINE_IN1_PIN, HIGH);
  digitalWrite(L_ENGINE_IN2_PIN, LOW);
  analogWrite(L_ENGINE_POWER_PIN, l_power);

  digitalWrite(R_ENGINE_IN1_PIN, HIGH);
  digitalWrite(R_ENGINE_IN2_PIN, LOW);
  analogWrite(R_ENGINE_POWER_PIN, r_power);
}
