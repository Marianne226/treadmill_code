#include "motorencoders.h"


void set_pins_bws_motor_encoders(){
  //encoders
  //set interrupt pins
  pinMode(BWS_ENC_1_PIN_A, INPUT);
  pinMode(BWS_ENC_2_PIN_A, INPUT);
  //set line B
  pinMode(BWS_ENC_1_PIN_B, INPUT);
  pinMode(BWS_ENC_2_PIN_B, INPUT);

}

void setup_interrupts_bws_motor_encoders(){
  // set interrupt for encoders 
  attachInterrupt(digitalPinToInterrupt(BWS_ENC_1_PIN_A), encoder_bws_motor_1_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(BWS_ENC_2_PIN_A), encoder_bws_motor_2_isr, RISING);
}

void encoder_bws_motor_1_isr() {
  
  if (digitalRead(BWS_ENC_1_PIN_B) == HIGH) {
    // clockwise rotation
    C_weightsupports::GetInstance().Decrement_BWS_mes_pos(0);
  } else {
    //counter-clockwise rotation
    C_weightsupports::GetInstance().Increment_BWS_mes_pos(0);
  } 
}

void encoder_bws_motor_2_isr() {
  
  if (digitalRead(BWS_ENC_2_PIN_B) == HIGH) {
    C_weightsupports::GetInstance().Decrement_BWS_mes_pos(1);
  } else {
    C_weightsupports::GetInstance().Increment_BWS_mes_pos(1);
  } 
}




