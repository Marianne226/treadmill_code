#include "treadmills.h"



C_treadmills::C_treadmills(){
    //TM motor 1
  pinMode(TM1_STEP, OUTPUT); //step pin
  pinMode(TM1_DIR, OUTPUT); //dir pin
  pinMode(TM1_EN, OUTPUT); //enable pin
  //TM motor 2
  pinMode(TM2_STEP, OUTPUT); //step pin
  pinMode(TM2_DIR, OUTPUT); //dir pin
  pinMode(TM2_EN, OUTPUT); //enable pin
}

void C_treadmills::Check_time_marker_overflows(){
  if (c_last_time_marker > c_time_max) {
    c_last_time_marker = 0;
  }
}

double C_treadmills::Calc_time_delay() {
  /**
  @brief Calculates the time delay between each motor step based on the treadmill's actual speed.
  * 
  * This function computes the time interval between motor control signals required to maintain 
  * the treadmill's current speed. 
  * 
  * @return The time delay in microseconds (us) between each motor step.
  */

  double TM_speed_actual = C_parameters::GetInstance().Get_TM_speed_actual();

  //Robot's constants
  double TM_radius = 2.5; //in cm
  unsigned int TM_motor_steps = 200;
  unsigned int nb_coils = 2;
  unsigned int TM_step_division_factor = 2; //full step(1/1) = 1, half step(1/2) = 2 , quarter(1/4) = 4, an eight(1/8) = 8, an sixtheen(1/16) = 16 

  //Robot's calculated constants
  long double TM_perimeter = 2*M_PI*TM_radius; //in cm
  long double TM_omega = abs(TM_speed_actual)/TM_perimeter; //in Hz
  long double TM_signals_per_turn = TM_motor_steps*TM_step_division_factor; //in signals
  long double TM_signals_per_seconds = TM_omega*TM_signals_per_turn*nb_coils; //in signals/s

  return c_TM_control_interval = 1/TM_signals_per_seconds * 1000000; //in us (micro seconds)
}

void C_treadmills::TM_clear_step_pin(){
  PIOB->PIO_CODR = PIO_PB25; //pin 2 STEP, Motor 1
  PIOC->PIO_CODR = PIO_PC28; //pin 3 STEP, Motor 2
}

void C_treadmills::TM_control(){
  /**
   * @brief Controls the treadmill motors by generating step signals based on the current speed.
   * 
   * This function handles the motor direction and step signal generation for the treadmill. 
   * It ensures that step signals are sent at the appropriate intervals, depending on the 
   * treadmill's current speed. The direction is set based on the sign of the speed, with 
   * specific motor pins activated accordingly.
   * 
   * @note The following motor control pins are used:
   *       - Direction pins:
   *         - `PIO_PB21` and `PIO_PC13` for setting motor orientation.
   *       - Step pins:
   *         - `PIO_PB25` and `PIO_PC28` for generating step signals.
   */

  double TM_speed_actual = C_parameters::GetInstance().Get_TM_speed_actual();
  unsigned long time_us = C_parameters::GetInstance().Get_time_us();
  unsigned long time_diff_us = time_us - c_last_time_marker;
  if (TM_speed_actual != 0){
    if (TM_speed_actual > 0){ //motors oriented diferently, so symetric of each other
      PIOB->PIO_CODR = PIO_PB21;
      PIOC->PIO_SODR = PIO_PC13;
    } else {
      PIOB->PIO_SODR = PIO_PB21;
      PIOC->PIO_CODR = PIO_PC13;
    }
    
    if (time_diff_us >= c_TM_control_interval){
      c_last_time_marker = C_parameters::GetInstance().Get_time_us();
      PIOB->PIO_SODR = PIO_PB25; //pin 2 STEP
      PIOC->PIO_SODR = PIO_PC28; //pin 3 STEP
    }
  } else {
    c_last_time_marker = time_us;
  }
}


void C_treadmills::Progressive_acceleration() {
  /**
   * @brief Gradually adjusts the treadmill's actual speed towards the target speed.
   * 
   * This function implements progressive acceleration to minimize abrupt changes 
   * in speed. It calculates the difference between the target speed (`TM_speed`) 
   * and the actual speed (`TM_speed_actual`) and adjusts the actual speed incrementally 
   * based on the magnitude of the difference.
   * 
   * @note The following adjustments are made:
   *       - If the speed difference is greater than 1 or less than -1, the actual speed is 
   *         incremented by a small value calculated as `1 / (1 + 0.25 * difference)`.
   *       - If the difference is between -1 and 1, the actual speed is set directly to the target speed.
   */

  double TM_speed = C_parameters::GetInstance().Get_TM_speed();
  double TM_speed_actual = C_parameters::GetInstance().Get_TM_speed_actual();
  double difference = TM_speed - TM_speed_actual;

  if (difference > 1){
    C_parameters::GetInstance().Set_TM_speed_actual(TM_speed_actual + 0.5);
  } else if (difference < -1){
    C_parameters::GetInstance().Set_TM_speed_actual(TM_speed_actual - 0.5);
  } else if (difference != 0 && (difference <= 1 || difference >= -1)){
    C_parameters::GetInstance().Set_TM_speed_actual(TM_speed); // go to desired speed directly
  }

}


//enables the pair of TM + BWS motors depending on teh selected specie
void C_treadmills::Enable_motor_as_species(){ 
  /**
   * @brief Enables the treadmill and BWS motors based on the selected species and current speed.
   * 
   * This function activates the appropriate pair of motors depending on the selected species 
   * (1 for mice, 2 for rats) and the current treadmill speed. It ensures motors are enabled 
   * or disabled based on speed thresholds to prevent unnecessary activation.
   */

  //select TM for species 
  unsigned int species = C_parameters::GetInstance().Get_species();
  double TM_speed = C_parameters::GetInstance().Get_TM_speed();

  //1 for mice, 2 for rats
  if (species == 1){ //clear bit to enable, set bit to disable
    //TM motors
    if (TM_speed > 0.1 or TM_speed < -0.1){
    PIOB->PIO_CODR = PIO_PB14;
    PIOC->PIO_SODR = PIO_PC12;
    }
  } else if (species == 2){ //clear bit to enable, set bit to disable
    //TM motors
    if (TM_speed > 0.1 or TM_speed < -0.1){
    PIOB->PIO_SODR = PIO_PB14;
    PIOC->PIO_CODR = PIO_PC12;
    }
  } else { //clear bit to enable, set bit to disable
    //TM motors
    if (TM_speed > 0.1 or TM_speed < -0.1){
    PIOB->PIO_SODR = PIO_PB14;
    PIOC->PIO_SODR = PIO_PC12;
    }
  }

  //disable motors, if zero speed
  if (TM_speed < 0.1 && TM_speed > -0.1){
    PIOB->PIO_SODR = PIO_PB14;
    PIOC->PIO_SODR = PIO_PC12;
  }
}


