#ifndef _ENDSWITCHES_H_
#define _ENDSWITCHES_H_



#include "Arduino.h"
#include "weightsupports.h"
#include "parameters.h"

//BWS endswitches (SS)
//inetnal (of the lead screw)
//switches BWS 1
#define BWS_SS_1_UP 34
#define BWS_SS_1_DOWN 35
//switches BWS 2
#define BWS_SS_2_UP 36
#define BWS_SS_2_DOWN 37

//external
//switches BWS 3
#define BWS_SS_3_UP 38
#define BWS_SS_3_DOWN 39
//switches BWS 4
#define BWS_SS_4_UP 40
#define BWS_SS_4_DOWN 41


void setup_BWS_endswitches();

void check_BWS_endswitches();

#endif