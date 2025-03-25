#include "Ports.h"
#include "Camera.h"
#include "FRAM.h"
#include "SNN.h"

#define MAX_ENGINE_POWER (255)

#define POT_MIN (0)
#define POT_MAX (255)
#define POT_RANGE (POT_MAX-POT_MIN)

void Transform_answer(ANSWER_TYPE answer, unsigned char &l_power, unsigned char &r_power ){
  l_power = ((-answer+1.0f)/2.0f)*MAX_ENGINE_POWER;
  r_power = ((answer+1.0f)/2.0f)*MAX_ENGINE_POWER;
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
  //pinMode(, OUTPUT);
}

void loop() {

  captureImg(160*2,120);

  unsigned char pot = analogRead(POT_PIN);

  float answer = -(float)(pot*2)/POT_RANGE+1;
  
  unsigned char l_power, r_power;

  for (unsigned short i = 0;i<SCREEN_SIZE;i++){
    unsigned char pixel = Read_fram(FRAM_SCREEN_MATRIX_ADR+i);
    Serial.print(pixel);
    Serial.print(" ");
  }

  Serial.println(answer);

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
