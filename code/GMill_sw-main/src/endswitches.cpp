#include "endswitches.h"


void setup_BWS_endswitches(){
    //internal SS
    //TM switches 1
    pinMode(BWS_SS_1_UP, INPUT_PULLUP);
    pinMode(BWS_SS_1_DOWN, INPUT_PULLUP);
    //TM switches 2
    pinMode(BWS_SS_2_UP, INPUT_PULLUP);
    pinMode(BWS_SS_2_DOWN, INPUT_PULLUP);

    //external SS
    //BWS switches 3
    pinMode(BWS_SS_3_UP, INPUT_PULLUP);
    pinMode(BWS_SS_3_DOWN, INPUT_PULLUP);
    //BWS switches 4
    pinMode(BWS_SS_4_UP, INPUT_PULLUP);
    pinMode(BWS_SS_4_DOWN, INPUT_PULLUP);
}


void check_BWS_endswitches(){
    //internal SS
    if (digitalRead(BWS_SS_1_UP) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_internal_up(true, 0);
        unsigned int state = C_parameters::GetInstance().Get_FSM_state_seperate();
        if (state != FSM_off && state != FSM_off_error && state != FSM_moving_error) { // if the state is already off, don't change it again
        C_parameters::GetInstance().Set_TM_on_off(0);
        C_parameters::GetInstance().Set_state_before_off(FSM_calibration); // save the state before turning off
        C_parameters::GetInstance().Set_FSM_State_seperate(FSM_off_error);
        //C_parameters::GetInstance().Set_FSM_on_off_state_seperate();
        C_parameters::GetInstance().Set_changes_register(3);
        C_parameters::GetInstance().Set_changes_register(4);
        }
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_internal_up(false, 0);
    }
    if (digitalRead(BWS_SS_1_DOWN) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_internal_down(true, 0);
        unsigned int state = C_parameters::GetInstance().Get_FSM_state_seperate();
        if (state != FSM_off && state != FSM_off_error && state != FSM_moving_error) { // if the state is already off, don't change it again
        C_parameters::GetInstance().Set_TM_on_off(0);
        C_parameters::GetInstance().Set_state_before_off(FSM_calibration); // save the state before turning off
        C_parameters::GetInstance().Set_FSM_State_seperate(FSM_off_error);
        //C_parameters::GetInstance().Set_FSM_on_off_state_seperate();
        C_parameters::GetInstance().Set_changes_register(3);
        C_parameters::GetInstance().Set_changes_register(5);
        }
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_internal_down(false, 0);
    }

    if (digitalRead(BWS_SS_2_UP) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_internal_up(true, 1);
        unsigned int state = C_parameters::GetInstance().Get_FSM_state_seperate();
        if (state != FSM_off && state != FSM_off_error && state != FSM_moving_error) { // if the state is already off, don't change it again
        C_parameters::GetInstance().Set_TM_on_off(0);
        C_parameters::GetInstance().Set_state_before_off(FSM_calibration); // save the state before turning off
        C_parameters::GetInstance().Set_FSM_State_seperate(FSM_off_error);
        //C_parameters::GetInstance().Set_FSM_on_off_state_seperate();
        C_parameters::GetInstance().Set_changes_register(3);
        C_parameters::GetInstance().Set_changes_register(4);
        }
        
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_internal_up(false, 1);
    }
    if (digitalRead(BWS_SS_2_DOWN) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_internal_down(true, 1);
        
        unsigned int state = C_parameters::GetInstance().Get_FSM_state_seperate();
        if (state != FSM_off && state != FSM_off_error && state != FSM_moving_error) { // if the state is already off, don't change it again
        C_parameters::GetInstance().Set_TM_on_off(0);
        C_parameters::GetInstance().Set_state_before_off(FSM_calibration); // save the state before turning off
        C_parameters::GetInstance().Set_FSM_State_seperate(FSM_off_error);
         //C_parameters::GetInstance().Set_FSM_on_off_state_seperate();
        C_parameters::GetInstance().Set_changes_register(3);
        C_parameters::GetInstance().Set_changes_register(5);
        }
       
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_internal_down(false, 1);
    }
    

    //external SS
    if (digitalRead(BWS_SS_3_UP) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_external_up(true, 0);
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_external_up(false, 0);
    }
    if (digitalRead(BWS_SS_3_DOWN) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_external_down(true, 0);
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_external_down(false, 0);
    }

    if (digitalRead(BWS_SS_4_UP) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_external_up(true, 1);
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_external_up(false, 1);
    }
    if (digitalRead(BWS_SS_4_DOWN) == LOW){
        C_weightsupports::GetInstance().Set_bws_ss_external_down(true, 1);
    } else {
        C_weightsupports::GetInstance().Set_bws_ss_external_down(false, 1);
    }
}