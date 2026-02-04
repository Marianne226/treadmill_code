#ifndef _BWSENCODERS_H_
#define _BWSENCODERS_H_



#include "Arduino.h"
#include "weightsupports.h"

//BWS encoders

#define FORWARD 0
#define BACKWARD 1

#define BWSE_ENC_1_PIN_A 42
#define BWSE_ENC_1_PIN_B 43

#define BWSE_ENC_2_PIN_A 44
#define BWSE_ENC_2_PIN_B 45

#define CLK_WISE 1
#define NO_DIRECTION 0
#define CCLK_WISE -1

#define ENCODER_TURN_RESOLUTION_MICE 5120
#define ENCODER_TURN_RESOLUTION_RATS 1000

void set_pins_bws_encoders();

void setup_interrupts_bws_encoders();

void encoder_bws_1_isr();

void encoder_bws_2_isr();


#endif