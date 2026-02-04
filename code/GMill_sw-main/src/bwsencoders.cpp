#include "bwsencoders.h"


void set_pins_bws_encoders(){
  //encoders
  //set interrupt pins
  pinMode(BWSE_ENC_1_PIN_A, INPUT);
  pinMode(BWSE_ENC_2_PIN_A, INPUT);
  //set line B
  pinMode(BWSE_ENC_1_PIN_B, INPUT);
  pinMode(BWSE_ENC_2_PIN_B, INPUT);
}

void setup_interrupts_bws_encoders(){
  // set interrupt for encoders 
  attachInterrupt(digitalPinToInterrupt(BWSE_ENC_1_PIN_A), encoder_bws_1_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(BWSE_ENC_2_PIN_A), encoder_bws_2_isr, RISING);
}

void encoder_bws_1_isr(){
  if (digitalRead(BWSE_ENC_1_PIN_B) == HIGH) {
    // clockwise rotation
    C_weightsupports::GetInstance().Set_BWSE_enc_dir_CCLK_WISE(1);
    C_weightsupports::GetInstance().Increment_BWSE_mes_pos(1);
  } else {
    //counter-clockwise rotation
    C_weightsupports::GetInstance().Set_BWSE_enc_dir_CLK_WISE(1);
    C_weightsupports::GetInstance().Decrement_BWSE_mes_pos(1);
  } 
}

void encoder_bws_2_isr(){
  if (digitalRead(BWSE_ENC_2_PIN_B) == HIGH) {
    // clockwise rotation
    C_weightsupports::GetInstance().Set_BWSE_enc_dir_CCLK_WISE(0);
    C_weightsupports::GetInstance().Decrement_BWSE_mes_pos(0);
  } else {
    //counter-clockwise rotation
    C_weightsupports::GetInstance().Set_BWSE_enc_dir_CLK_WISE(0);
    C_weightsupports::GetInstance().Increment_BWSE_mes_pos(0);
  } 
}

