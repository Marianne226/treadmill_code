#include <weightsupports.h>

C_weightsupports::C_weightsupports(){
    //TM motor 1
  pinMode(BWS1_STEP, OUTPUT); //step pin
  pinMode(BWS1_DIR, OUTPUT); //dir pin
  pinMode(BWS1_EN, OUTPUT); //enable pin
  // TM motor 2
  pinMode(BWS2_STEP, OUTPUT); //step pin
  pinMode(BWS2_DIR, OUTPUT); //dir pin
  pinMode(BWS2_EN, OUTPUT); //enable pin
}

void C_weightsupports::BWS_clear_step_pin(){
  //BWS
  PIOC->PIO_CODR = PIO_PC26; //pin 4 STEP
  PIOC->PIO_CODR = PIO_PC25; //pin 5 STEP
}

void C_weightsupports::BWS_enable_motor_as_species(){
  /**
   * @brief Enables the Body Weight Support (BWS) motors based on the selected species and FSM state.
   * 
   * This function activates or disables the BWS motors depending on the selected species (1 for mice, 2 for rats) 
   * and the current FSM state. It ensures appropriate motor control for weight support systems.
   * 
   */

  unsigned int species = C_parameters::GetInstance().Get_species();
  unsigned int c_FSM_state = C_parameters::GetInstance().Get_FSM_state_seperate();

  //1 for mice, 2 for rats
  if (species == 1){ //clear bit to enable, set bit to disable
    //BWS motors
    PIOC->PIO_CODR = PIO_PC14; 
    PIOC->PIO_SODR = PIO_PC16;
  } else if (species == 2){
    //BWS motors
    PIOC->PIO_SODR = PIO_PC14; 
    PIOC->PIO_CODR = PIO_PC16;
  } else if (species != 1 || species != 2 || c_FSM_state == FSM_off) { 
    //BWS motors
    PIOC->PIO_SODR = PIO_PC14; 
    PIOC->PIO_SODR = PIO_PC16;
  }
}

void C_weightsupports::BWS_run_motor(){
  /**
   * @brief Controls the operation of the Body Weight Support (BWS) motor based on the current FSM state and species.
   * 
   * This function runs the BWS motor by generating control signals at intervals defined by the 
   * control period (`c_BWS_control_interval`). The motor direction and step signals are determined 
   * by the species and direction configuration (`c_switch_to_direction`).
   * 
   * @note The motor operates only in specific FSM states:
   *       - `FSM_base_pos`
   *       - `FSM_Moving`
   *       - `FSM_calibration`
   */

  unsigned int c_FSM_state = C_parameters::GetInstance().Get_FSM_state_seperate();
  unsigned int species = C_parameters::GetInstance().Get_species() - 1;

  switch (c_FSM_state){
    case FSM_base_pos:
    case FSM_Moving: 
    case FSM_moving_error:
    case FSM_calibration: {
      unsigned long time_us = C_parameters::GetInstance().Get_time_us();
      c_time_diff_us_BWS = time_us - c_last_time_marker_BWS;

      if (c_time_diff_us_BWS >= c_BWS_control_interval){
          c_last_time_marker_BWS = time_us;
          c_BWS_control_interval = BWS_MOTOR_CONTROL_INTERVAL;

          if (c_switch_to_direction[species] == CLK_WISE) {
            if (species == 0) {
              PIOC->PIO_CODR = PIO_PC15; //set direction
              PIOC->PIO_SODR = PIO_PC26; //make step
            } else {
              PIOC->PIO_CODR = PIO_PC17; //set direction
              PIOC->PIO_SODR = PIO_PC25; //make step
            }
          } else {
            if (species == 0) {
              PIOC->PIO_SODR = PIO_PC15; //set direction
              PIOC->PIO_SODR = PIO_PC26; //make step
            } else {
              PIOC->PIO_SODR = PIO_PC17; //set direction
              PIOC->PIO_SODR = PIO_PC25; //make step
            }
          }
        }
      break;
    }
    case FSM_Compensate:
    case FSM_add_mouse:
    case FSM_Idle:
    case FSM_off: {
      // don't do anything
      break;
    }
  }
}


void C_weightsupports::BWS_control_motor_position(){
  /**
   * @brief Controls the position of the Body Weight Support (BWS) motor based on the current FSM state.
   * 
   * This function manages the BWS motor position by invoking the appropriate control method 
   * for the current FSM state. Each state corresponds to a specific motor action or behavior.
   */

  unsigned int FSM_State = C_parameters::GetInstance().Get_FSM_state_seperate();
  switch (FSM_State) {
    case FSM_moving_error:{
      //Move the BWS
      Go_to_desired_position(FSM_moving_error); 
      break;
    }

    case FSM_Moving: {
      //Move the BWS
      Go_to_desired_position(FSM_Moving); 
      break;
    }

    case FSM_Compensate: {
      // Compute the needed position to compensate for bws
      Control_bws_compensation();
      break;
    }

    case FSM_calibration: {
      // Calibrate the motor
      Calibrate_motor();
      break;
    }

    case FSM_base_pos:{
      // Go to the base position when finished calibrating
      Go_to_desired_position(FSM_base_pos);
      break;
    }

    case FSM_add_mouse:{
      // When adding rodent, check if it is done
      check_finished_adding();
      break;
    }

    case FSM_off_error:
    case FSM_Idle:{
      // Check if we need to move the platform up or down
      check_up_down();
      break;
    }
  }
}

void C_weightsupports::BWS_check_endswitches(){
  /**
   * @brief Checks the states of the internal and external end switches for the Body Weight Support (BWS) system.
   * 
   * This function evaluates the current states of the up and down end switches (both internal and external) 
   * for the specified species. It updates the corresponding behavior flags to indicate whether the switch 
   * state is transitioning from falling to rising or remains falling.
   * 
   * @note Behavior of end switches:
   *       - If the switch is active (`true`), the behavior is set to `RISING` if it was previously `FALLING`.
   *       - If the switch is inactive (`false`), the behavior is set to `FALLING`.
   */

  unsigned int species = C_parameters::GetInstance().Get_species() - 1;

  // Check the internal switches (the ones inside the robot)
  if (c_bws_ss_internal_up[species] == true) {
    if (c_behaviour_bws_ss_internal_up[species] == FALLING) {
      c_behaviour_bws_ss_internal_up[species] = RISING;
    }
  } else {
    c_behaviour_bws_ss_internal_up[species] = FALLING;
  }

  if (c_bws_ss_internal_down[species] == true) {
    if (c_behaviour_bws_ss_internal_down[species] == FALLING) {
      c_behaviour_bws_ss_internal_down[species] = RISING;
    }
  } else {
    c_behaviour_bws_ss_internal_down[species] = FALLING;
  }

  // Check the external switches (the ones outside the robot)
  if (c_bws_ss_external_up[species] == true) {
    if (c_behaviour_bws_ss_external_up[species] == FALLING) {
      c_behaviour_bws_ss_external_up[species] = RISING;
    } 
  } else {
    c_behaviour_bws_ss_external_up[species] = FALLING;
  }

  if (c_bws_ss_external_down[species] == true) {
    if (c_behaviour_bws_ss_external_down[species] == FALLING) {
      c_behaviour_bws_ss_external_down[species] = RISING;
    }
  } else {
    c_behaviour_bws_ss_external_down[species] = FALLING;
  }
}


void C_weightsupports::Control_bws_compensation(){
  /**
   * @brief Controls the Body Weight Support (BWS) compensation system to maintain the desired force balance.
   * 
   * This function calculates the necessary displacement for the BWS system to achieve the target elongation 
   * based on the weight support requirements and the current state of the spring tension. It adjusts the 
   * desired position of the BWS motor to compensate for the difference between the target and current elongation.
   * 
   * @note The compensation process involves:
   *       - Calculating the force to compensate for using the weight, body weight support percentage (`bws_to_compensate`), 
   *         and the spring constant.
   *       - Determining the target elongation of the spring in millimeters based on the force difference.
   *       - Calculating the required motor displacement signals to achieve the target elongation.
   * 
   * @note Constraints:
   *       - The desired position is limited to stay within `0` and `BWS_MAX_POS`.
   *       - Adjustments include a buffer of `5` units when the limits are exceeded.
   *       - An additional offset of `10 mm` is added to the target elongation if the BWS compensation is at 100%.
   */

  unsigned int species = C_parameters::GetInstance().Get_species() - 1;
  unsigned int bws_to_compensate = C_parameters::GetInstance().Get_bws();

  // Compute the total force (rodent weight + BWS weight + BWS compensation)
  double force_to_compensate = 0.001 * (C_parameters::GetInstance().Get_weight()*bws_to_compensate*0.01 + C_parameters::GetInstance().Get_weight_support())
                              * C_parameters::GetInstance().Get_g_constant(); // result in Newton : N = kg.m.s⁻2

  // Compute the needed spring elongation to compensate for the force
  double target_elongation_mm = (force_to_compensate - C_parameters::GetInstance().Get_spring_tension()) * C_parameters::GetInstance().Get_inverse_R_spring_cst();  // result in mm

  // Make sure that the rodent flies when at 100%
  if (bws_to_compensate == 100){
    target_elongation_mm += 10;
  }

  //C_parameters::GetInstance().Set_target_elongation(target_elongation_mm); --> used in the check_BWSE_and_compensate that is not used

  // Find the displacement of the motors to reach that elongation
  double current_elongation_mm = C_weightsupports::spring_deformation();
  double needed_displacement_mm = target_elongation_mm - current_elongation_mm;
  double needed_displacement_in_motor_signals = needed_displacement_mm*250;

  // Set the destination position of the BWS motor
  if ((c_BWS_mes_pos[species] + needed_displacement_in_motor_signals < BWS_MAX_POS)&&(c_BWS_mes_pos[species] + needed_displacement_in_motor_signals > 0)){
      c_BWS_des_pos[species] = c_BWS_mes_pos[species] + needed_displacement_in_motor_signals;
    } else if (c_BWS_mes_pos[species] + needed_displacement_in_motor_signals > BWS_MAX_POS){
      c_BWS_des_pos[species] = BWS_MAX_POS-5;
    } else if (c_BWS_mes_pos[species] + needed_displacement_in_motor_signals < 0) {
      c_BWS_des_pos[species] = 5;
    }
    C_parameters::GetInstance().Set_FSM_State_seperate(FSM_Moving);
    C_parameters::GetInstance().Set_changes_register(3);
};

void C_weightsupports::Calibrate_motor(){
  /**
   * @brief Calibrates the Body Weight Support (BWS) motor to determine the zero position.
   * 
   * This function performs motor calibration by moving the BWS system to the bottom external switch 
   * to set the zero position and then towards the base position
   * During calibration, the motor direction and measured positions are adjusted accordingly.
   * 
   */

  unsigned int species = C_parameters::GetInstance().Get_species() - 1;

  //Setting the measured pos at the bottom switch to zero
  if (c_calibration_state == SET_BOTTOM) {

    // if we did not press on the end switch yet 
    if (c_behaviour_bws_ss_external_down[species] == FALLING) {
      // keep moving up
      c_switch_to_direction[species] = CCLK_WISE;
    } else if (c_behaviour_bws_ss_external_down[species] == RISING) {
      //Set the measured position to zero and switch direction
      reset_BWS_mes_pos(species);
      C_weightsupports::reset_BWSE_mes_pos(species);
      c_switch_to_direction[species] = CLK_WISE; 
      c_BWS_des_pos[species] = C_parameters::GetInstance().Get_base_position();
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_base_pos);
      C_parameters::GetInstance().Set_changes_register(3);
    }
  }
}

void C_weightsupports::Go_to_desired_position(unsigned int c_FSM_state){
  /**
   * @brief Moves the Body Weight Support (BWS) motor to the desired position and then sets up the rodent addition if went to base position
   * 
   * This function adjusts the motor direction and control interval to move the BWS motor towards 
   * its desired position (`c_BWS_des_pos`). If it was going to the base position, it initialises the rodent adding code
   * 
   * @param c_FSM_state The current FSM state guiding the motor behavior.
   */

  unsigned int species = C_parameters::GetInstance().Get_species() - 1;

  // If we are too high
  if (c_BWS_mes_pos[species] > (c_BWS_des_pos[species] + 5) && c_switch_to_direction[species] == CLK_WISE) {
    c_switch_to_direction[species] = CCLK_WISE;
    c_BWS_control_interval = BWS_MOTOR_CONTROL_INTERVAL + DELTA_MARGIN_MOTOR_DIRECTION_CHANGE;
  
  // If we are too low
  } else if (c_BWS_mes_pos[species] < (c_BWS_des_pos[species] - 5) && c_switch_to_direction[species] == CCLK_WISE){
    c_switch_to_direction[species] = CLK_WISE;
    c_BWS_control_interval = BWS_MOTOR_CONTROL_INTERVAL + DELTA_MARGIN_MOTOR_DIRECTION_CHANGE;
  
  // If we are close enough
  } else if (c_BWS_mes_pos[species] < (c_BWS_des_pos[species] + 5) && c_BWS_mes_pos[species] > c_BWS_des_pos[species] - 5) {
    if (c_FSM_state == FSM_base_pos) {
      //communicate to raspberry pi that it is ready to add the mouse
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_add_mouse);
      C_parameters::GetInstance().Set_changes_register(3);
      if  (C_parameters::GetInstance().Get_initialised_value() == false){
        C_weightsupports::initialise_adding_rodent();
        C_parameters::GetInstance().Set_initialised_value(true);
      }
    } else if (c_FSM_state == FSM_moving_error){
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_calibration);
      C_parameters::GetInstance().Set_changes_register(3);
    } else {
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_Idle);
      C_parameters::GetInstance().Set_changes_register(3);
    }
  }
}

void C_weightsupports::reset_BWSE_mes_pos(unsigned int species){
  /**
   * @brief Resets the measured position of the Body Weight Support External (BWSE) system for the specified species when calibrating.
   * 
   * This function sets the initial measured position (`c_BWSE_mes_pos`) of the BWSE system.
   * 
   * @param species The species for which the BWSE position is being reset:
   *                - `0`: Mice
   *                - `1`: Rats
   */

  if (species == 0){// mice
    double init_spring_elongation_mm = -20; 
    double mm_to_BWSE = ENCODER_TURN_RESOLUTION_MICE*init_spring_elongation_mm/(3.14*24.5);
    c_BWSE_mes_pos[0] = mm_to_BWSE;

  }else{ // rats
    double init_spring_elongation_mm = -11;
    double mm_to_BWSE = ENCODER_TURN_RESOLUTION_RATS*init_spring_elongation_mm/(3.14*24.5);
    c_BWSE_mes_pos[1] = mm_to_BWSE;
  }
}

void C_weightsupports::initialise_adding_rodent(){
  C_parameters::GetInstance().Set_start_adding_rodent(1);
  C_parameters::GetInstance().Set_changes_register(2);
}

void C_weightsupports::check_finished_adding(){
  /**
   * @brief Checks the completion of the rodent addition process and adjusts the Body Weight Support (BWS) position if needed.
   * 
   * This function monitors the rodent addition status and determines whether the FSM state should transition 
   * to `FSM_Compensate`. It also adjusts the BWS position up or down based on the `move_up_down` parameter.
   */

  //check if the rodent has been added
  if (C_parameters::GetInstance().Get_start_adding_rodent() == 0) {
    C_parameters::GetInstance().Set_FSM_State_seperate(FSM_Compensate); //start compensating for the BWS
    C_parameters::GetInstance().Set_changes_register(3);
  }

  //check if need to adjust the BWS up or down 
  int move_up_down = C_parameters::GetInstance().Get_move_up_down();
  if (move_up_down != 0){
    double needed_displacement_mm = 10*move_up_down; //1 move_up_down = 1cm
    double needed_displacement_in_motor_signals = needed_displacement_mm*250;
    C_parameters::GetInstance().Set_move_up_down(0);

    unsigned int species = C_parameters::GetInstance().Get_species() - 1;
    if ((c_BWS_des_pos[species] + needed_displacement_in_motor_signals < BWS_MAX_POS)&&(c_BWS_des_pos[species] + needed_displacement_in_motor_signals > 0)){
      c_BWS_des_pos[species] += needed_displacement_in_motor_signals;
    } else if (c_BWS_des_pos[species] + needed_displacement_in_motor_signals > BWS_MAX_POS){
      c_BWS_des_pos[species] = BWS_MAX_POS-5;
    } else if (c_BWS_des_pos[species] + needed_displacement_in_motor_signals < 0){
      c_BWS_des_pos[species] = 5;
    }
    C_parameters::GetInstance().Set_FSM_State_seperate(FSM_base_pos);
    C_parameters::GetInstance().Set_changes_register(3);
  }
}

void C_weightsupports::check_BWSE_and_compensate(){
  /**
   * @brief Checks the Body Weight Support External (BWSE) spring deformation and adjusts motor position to compensate.
   * 
   * This function compares the current spring deformation with the target elongation and compensates by adjusting 
   * the motor's desired position (`c_BWS_des_pos`). Compensation is triggered if the current deformation deviates 
   * from the target by more than 20 mm.
   */


  //look if the actual spring deformation is bigger than the desired one --> closed loop control
  double target_elongation_mm = C_parameters::GetInstance().Get_target_elongation();
  double current_deformation_mm = C_weightsupports::spring_deformation();
  
  // Check if we are far from the target elongation
  if ((current_deformation_mm < target_elongation_mm - 20) || (current_deformation_mm > target_elongation_mm + 20)){
    double needed_displacement_mm = target_elongation_mm - current_deformation_mm;
    double needed_displacement_in_motor_signals = needed_displacement_mm*250; 
    unsigned int species = C_parameters::GetInstance().Get_species() - 1;

    // Set the new destination to reach the target elongatoin
     if ((c_BWS_mes_pos[species] + needed_displacement_in_motor_signals < BWS_MAX_POS)&&(c_BWS_mes_pos[species] + needed_displacement_in_motor_signals > 0)){
      c_BWS_des_pos[species] = c_BWS_mes_pos[species] + needed_displacement_in_motor_signals;
    } else if (c_BWS_mes_pos[species] + needed_displacement_in_motor_signals > BWS_MAX_POS){
      c_BWS_des_pos[species] = BWS_MAX_POS-5;
    } else if (c_BWS_mes_pos[species] + needed_displacement_in_motor_signals < 0) {
      c_BWS_des_pos[species] = 5;
    }
    C_parameters::GetInstance().Set_FSM_State_seperate(FSM_Moving);
    C_parameters::GetInstance().Set_changes_register(3);
  }
}

double C_weightsupports::spring_deformation(){
  /**
   * @brief Calculates the spring deformation in millimeters for the Body Weight Support (BWS) system.
   * 
   * This function computes the elongation of the spring as the difference between the measured positions 
   * of the BWS and BWSE systems, converted into millimeters. The calculation depends on the species (mice or rats).
   * 
   * @return The spring deformation in millimeters.
   */

  unsigned int species = C_parameters::GetInstance().Get_species() - 1;
  double BWSE_to_mm = 0;

  if (species == 0){
    BWSE_to_mm = (C_weightsupports::Get_BWSE_mes_pos(species)*3.14*24.5)/ENCODER_TURN_RESOLUTION_MICE;
  } else {
    BWSE_to_mm = (C_weightsupports::Get_BWSE_mes_pos(species)*3.14*24.5)/ENCODER_TURN_RESOLUTION_RATS;
  }

  double BWS_to_mm = C_weightsupports::Get_BWS_mes_pos(species)*2/500;
  double spring_elongation_mm = BWS_to_mm - BWSE_to_mm; 
  return spring_elongation_mm; 
}

void C_weightsupports::bws_changed(unsigned int bws){ 
  /**
   * @brief Calls the function to update the the bws value and triggers compensation if a state change occurs.
   * 
   * This function updates the BWS value by calling `Set_bws_change_state()` to check and set the new state. 
   * If the state is modified, the compensation control process is invoked to align the system with the updated BWS value.
   * 
   * @param bws The new BWS value to be set.
   */

  bool to_modify = C_parameters::GetInstance().Set_bws_change_state(bws);
  if (to_modify==true){
    C_weightsupports::Control_bws_compensation();
  }
};

void C_weightsupports::weight_changed(unsigned int weight){ 
  /**
   * @brief Calls the function to update the the weight value and triggers compensation if a state change occurs.
   * 
   * This function updates the weight value by calling `Set_weight_change_state()` to check and set the new state. 
   * If the state is modified, the compensation control process is invoked to align the system with the updated BWS value.
   * 
   * @param weight The new weight value to be set.
   */
  bool to_modify = C_parameters::GetInstance().Set_weight_change_state(weight);
  if (to_modify==true){
    C_weightsupports::Control_bws_compensation();
  }
};


void C_weightsupports::check_up_down(){
  /**
   * @brief Checks and adjusts the Body Weight Support (BWS) position based on the up or down movement command.
   * 
   * This function processes the `move_up_down` parameter to determine the required displacement in the BWS position. 
   * It calculates the corresponding motor signals for the desired displacement, updates the BWS of 1 cm for each 
   * time the button is pressed
   * 
   */

  int move_up_down = C_parameters::GetInstance().Get_move_up_down();
  if (move_up_down != 0){
    double needed_displacement_mm = 10*move_up_down; //1 move_up_down = 1cm
    double needed_displacement_in_motor_signals = needed_displacement_mm*250;
    C_parameters::GetInstance().Set_move_up_down(0);

    unsigned int species = C_parameters::GetInstance().Get_species() - 1;
    if ((c_BWS_des_pos[species] + needed_displacement_in_motor_signals < BWS_MAX_POS)&&(c_BWS_des_pos[species] + needed_displacement_in_motor_signals > 0)){
      c_BWS_des_pos[species] += needed_displacement_in_motor_signals;
    } else if (c_BWS_des_pos[species] + needed_displacement_in_motor_signals > BWS_MAX_POS){
      c_BWS_des_pos[species] = BWS_MAX_POS-5;
    } else if (c_BWS_des_pos[species] + needed_displacement_in_motor_signals < 0){
      c_BWS_des_pos[species] = 5;
    }
    if  (C_parameters::GetInstance().Get_FSM_state_seperate() == FSM_off_error) {
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_moving_error);
    } else {
      C_parameters::GetInstance().Set_FSM_State_seperate(FSM_Moving);
    }
    C_parameters::GetInstance().Set_changes_register(3);
  }
}