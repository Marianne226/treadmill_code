#include "manualencoder.h"



//Deduced control parameters
int short C_manualencoder::c_hysteresis = 0;
int short C_manualencoder::c_change = 0;
double C_manualencoder::c_crement = 1; //0.5;

//Time variable for control
unsigned long C_manualencoder::c_last_time_marker_enc = micros();

unsigned long C_manualencoder::c_time_diff_us_ENC= 0;

unsigned C_manualencoder::c_prev_time = 0;
unsigned C_manualencoder::c_delta_time = 500;  //us

unsigned long C_manualencoder::c_last_time_marker_push_button = micros();
unsigned long C_manualencoder::c_control_interval_push_button = 5000; //in us


C_manualencoder::C_manualencoder(){
    //rotary encoder
    //set interrupt pins
    pinMode(ENC_ROT_PIN_A, INPUT_PULLUP);
    //set line B
    pinMode(ENC_ROT_PIN_B, INPUT_PULLUP);
    //button
    pinMode(PUSH_BUTTON_ENC_ROT, INPUT_PULLUP);

    //set interrupt
    attachInterrupt(digitalPinToInterrupt(ENC_ROT_PIN_A), Manualencoder_isr, RISING);
    attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_ENC_ROT), Button_pushed_Manualencoder_isr, RISING);
}


void C_manualencoder::Time_diff_rot_enc(){
  /**
   * @brief Computes the time difference between successive encoder events and updates treadmill speed if conditions are met.
   * 
   * This function calculates the time elapsed since the last encoder marker. If the FSM
   * state is not `FSM_off` and sufficient time has passed, the encoder's state is updated, and the treadmill 
   * speed is adjusted based on the hysteresis value and speed increment.
   * 
   * @note If the time difference exceeds a predefined control interval and there is no ongoing change (`c_change == 0`), 
   *       the treadmill speed is adjusted using the hysteresis value.
   */

  unsigned int c_FSM_state = C_parameters::GetInstance().Get_FSM_state_seperate();
  if (c_FSM_state != FSM_off) {
    unsigned long time_us = C_parameters::GetInstance().Get_time_us();
    c_time_diff_us_ENC = time_us - c_last_time_marker_enc;

    if(c_change == 0 && c_time_diff_us_ENC > c_control_interval_push_button){
      c_last_time_marker_enc = time_us;
      c_change = 1;
      if (c_hysteresis != 0) {
        C_parameters::GetInstance().Add_to_TM_speed(c_crement * (c_hysteresis));
      }
      c_hysteresis = 0;
    }
  }
}

void C_manualencoder::Manualencoder_isr() {
  /**
   * @brief Interrupt Service Routine for handling manual encoder rotation events --> will control the TM speed
   * 
   * This ISR detects changes in the encoder's rotation direction and updates the hysteresis value accordingly.
   * It ensures a minimum time interval (`c_delta_time`) between consecutive updates to prevent rapid oscillations.
   * 
   */
    unsigned time = micros();
    if (time - c_prev_time > c_delta_time){
        c_change = 0;
        if (digitalRead(ENC_ROT_PIN_B) == HIGH && c_hysteresis >= 0) {
            // counter-clockwise rotation
            c_hysteresis -= 1;
        } else if (digitalRead(ENC_ROT_PIN_B) == LOW && c_hysteresis <= 0) {
            // clockwise rotation
            c_hysteresis += 1;
        }
        c_prev_time = time;
    }
}

void C_manualencoder::Button_pushed_Manualencoder_isr(){
  /**
   * @brief Interrupt Service Routine triggered when the manual encoder button is pushed --> turn TM off
   * 
   * This ISR handles the logic for switching the treadmill off when the manual encoder button is pressed. 
   * It ensures that actions are only taken if sufficient time has elapsed since the last button press, 
   * avoiding repeated triggers.
   * 
   * @note The function performs the following actions if the FSM state is not already `FSM_off`:
   *       - Updates the last button press timestamp (`c_last_time_marker_push_button`).
   *       - Sets the treadmill state to off (`FSM_off`).
   *       - Saves the current FSM state as the state before the off state.
   *       - Turns the treadmill off by setting its speed to 0.
   *       - Updates the changes register to reflect modifications.
   */
  unsigned long time_us = C_parameters::GetInstance().Get_time_us();
    if ((time_us - c_last_time_marker_push_button) > c_control_interval_push_button) {
      unsigned int state_before = C_parameters::GetInstance().Get_FSM_state_seperate();

      if (state_before != 4) { //check that the treadmill is not already off
        c_last_time_marker_push_button = time_us;
        C_parameters::GetInstance().Set_TM_on_off(0);
        C_parameters::GetInstance().Set_state_before_off(state_before);
        C_parameters::GetInstance().Set_FSM_State_seperate(FSM_off);
        C_parameters::GetInstance().Set_changes_register(3);
        C_parameters::GetInstance().Set_TM_speed(0);
        C_parameters::GetInstance().Set_changes_register(0);
      }
    }
}