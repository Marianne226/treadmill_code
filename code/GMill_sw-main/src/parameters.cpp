#include "parameters.h"


void C_parameters::Set_TM_speed(double TM_speed){
  /**
   * @brief Sets the speed and direction of the Treadmill.
   * 
   * This function sets the treadmill speed and updates its direction based on 
   * the provided speed value. If the speed is positive, the direction is set 
   * to FORWARD; otherwise, it is set to BACKWARD. Additionally, it updates 
   * the changes register to indicate a modification.
   * 
   * @param TM_speed The desired treadmill speed. 
   */

  c_TM_speed = TM_speed; // set the speed in the parameters
  C_parameters::GetInstance().Set_changes_register(0); //set the changes register to send info to the Raspberry pi

  if (TM_speed > 0.0){
      c_TM_direction = FORWARD;
  } else {
      c_TM_direction = BACKWARD;
  }

};

void C_parameters::Add_to_TM_speed(double TM_speed_add_on){
  /**
   * @brief Adds a specified value to the current treadmill speed.
   * 
   * This function increments the current treadmill speed by a given value while ensuring
   * that the resultant speed remains within acceptable limits based on the species type.
   * The adjusted speed is then applied, and the changes register is updated if modifications are made.
   * 
   * @param TM_speed_add_on The value to be added to the current treadmill speed. 
   * 
   * @note The speed limits are species-dependent:
   *       - For species 1 = mice: The speed is clamped between -5 and 15.
   *       - For other species = rats: The speed is clamped between -5 and 25.
   */

  double c_TM_speed_before = C_parameters::GetInstance().Get_TM_speed();
  double c_TM_speed_after = c_TM_speed_before + TM_speed_add_on;

  unsigned int species = C_parameters::GetInstance().Get_species();

  // Check if the speed is within the acceptable limits
  if (species == 1){ // mice
    if (c_TM_speed_after > 15){ //can go up to 80 but here we max it to 15 due to high power and noise
      c_TM_speed_after = 15;
    } else if (c_TM_speed_after < -5){
      c_TM_speed_after = -5;
    }
  } else { // rats
    if (c_TM_speed_after > 25){ //can go up to 80 but here we max it to 25 due to high power and noise
      c_TM_speed_after = 25;
    } else if (c_TM_speed_after < -5){
      c_TM_speed_after = -5;
    }
  } 
  
  // Adjust the correct speed if needed
  if (c_TM_speed_after != c_TM_speed_before){
    C_parameters::Set_TM_speed(c_TM_speed_after);
    C_parameters::GetInstance().Set_changes_register(0); //indicate the speed change to the raspberry pi
  }
};


void C_parameters::Set_FSM_on_off_state_seperate(){
  /**
   * @brief Updates the FSM state based on the treadmill's on/off status.
   * 
   * This function checks the current treadmill on/off state and adjusts the FSM state accordingly:
   * - If the treadmill is off, the current FSM state is saved, and the FSM is set to the `FSM_off` state.
   * - If the treadmill is on, the FSM state is restored to its previous state before it was turned off.
   * The changes register is updated when transitioning back to the previous state.
   * 
   * @note The `FSM_off` state is set only if the treadmill is off and the FSM is not already in `FSM_off`.
   */

  bool tm_on_off = C_parameters::GetInstance().Get_TM_on_off();
  if (tm_on_off == false) { //treadmill is off
    unsigned int state = C_parameters::GetInstance().Get_FSM_state_seperate();
    if (state != FSM_off) { // if the state is already off, don't change it again
      C_parameters::GetInstance().Set_state_before_off(state); // save the state before turning off
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_off);
    }
  } else { // treadmill is on
    unsigned int state_before = C_parameters::GetInstance().Get_state_before_off();
    C_parameters::GetInstance().Set_FSM_State_seperate(state_before);
    C_parameters::GetInstance().Set_changes_register(3); //Indicate to the raspberry pi that the state has changed
  }
};

bool C_parameters::Set_bws_change_state(unsigned int bws){ 
  /**
   * @brief Updates the BWS (Body Weight Support) value and FSM state if applicable.
   * 
   * This function sets the BWS parameter and modifies the FSM state to `FSM_Compensate` if the current FSM 
   * state is `FSM_Idle`, `FSM_Moving`, or `FSM_Compensate` for the specified species. A flag is returned 
   * to indicate whether a modification was made to the FSM state.
   * 
   * @param bws The new BWS value to set.
   * 
   * @return `true` if the FSM state was modified to `FSM_Compensate`; `false` otherwise.
   * 
   * @note The FSM state is modified only if it is in one of the following states: `FSM_Idle`, 
   *       `FSM_Moving`, or `FSM_Compensate`.
   */

  bool modifiy = false; // parameter to determine if we need to modify the bws position
  c_bws = bws; // Set the new bws 
  if (c_FSM_state_seperate[c_species-1] == FSM_Idle || c_FSM_state_seperate[c_species-1] == FSM_Moving || c_FSM_state_seperate[c_species-1] == FSM_Compensate){
    c_FSM_state_seperate[c_species-1] = FSM_Compensate;
    modifiy = true;
  }
  return modifiy;
};

bool C_parameters::Set_weight_change_state(unsigned int weight){ 
  /**
   * @brief Updates the weight value and FSM state if applicable.
   * 
   * This function sets the weight parameter and modifies the FSM state to `FSM_Compensate` if the current FSM 
   * state is `FSM_Idle`, `FSM_Moving`, or `FSM_Compensate` for the specified species. A flag is returned 
   * to indicate whether a modification was made to the FSM state.
   * 
   * @param weight The new weight value to set.
   * 
   * @return `true` if the FSM state was modified to `FSM_Compensate`; `false` otherwise.
   * 
   * @note The FSM state is modified only if it is in one of the following states: `FSM_Idle`, 
   *       `FSM_Moving`, or `FSM_Compensate`.
   */

  bool modify = false;
  c_weight = weight; // set the new weight
  if (c_FSM_state_seperate[c_species-1] == FSM_Idle || c_FSM_state_seperate[c_species-1] == FSM_Moving || c_FSM_state_seperate[c_species-1] == FSM_Compensate){
    c_FSM_state_seperate[c_species-1] = FSM_Compensate;
    modify = true;
  }
  return modify;
};
