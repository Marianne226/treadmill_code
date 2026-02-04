#ifndef _MOTORENCODERs_H_
#define _MOTORENCODERs_H_

#include "Arduino.h"
#include "weightsupports.h"

#define FORWARD 0
#define BACKWARD 1

#define BWS_ENC_1_PIN_A 22
#define BWS_ENC_1_PIN_B 23

#define BWS_ENC_2_PIN_A 24
#define BWS_ENC_2_PIN_B 25

#define CLK_WISE 1
#define NO_DIRECTION 0
#define CCLK_WISE -1

void set_pins_bws_motor_encoders();

void setup_interrupts_bws_motor_encoders();

void encoder_bws_motor_1_isr();

void encoder_bws_motor_2_isr();




#endif