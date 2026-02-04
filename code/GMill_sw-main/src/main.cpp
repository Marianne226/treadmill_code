#include <Arduino.h>
#include <math.h>

#include "parameters.h"
#include "treadmills.h"
#include "manualencoder.h"
#include "weightsupports.h"
#include "motorencoders.h"
#include "endswitches.h"
#include "bwsencoders.h"
#include "communication.h"


void setup() {
  Serial.begin(115200);

  set_pins_bws_motor_encoders();
  setup_interrupts_bws_motor_encoders();

  setup_BWS_endswitches();

  set_pins_bws_encoders();
  setup_interrupts_bws_encoders();
}

void loop() {

  // Check the endswitches
  check_BWS_endswitches();
  C_weightsupports::GetInstance().BWS_check_endswitches();

  // Control the treadmill 
  C_parameters::GetInstance().Update_time_us();
  C_treadmills::GetInstance().Check_time_marker_overflows();
  C_treadmills::GetInstance().Enable_motor_as_species();
  C_treadmills::GetInstance().Progressive_acceleration();
  C_treadmills::GetInstance().Calc_time_delay();
  C_treadmills::GetInstance().TM_control();
  C_treadmills::GetInstance().TM_clear_step_pin();

  // Control the body weight support
  C_weightsupports::GetInstance().BWS_enable_motor_as_species();
  C_weightsupports::GetInstance().BWS_control_motor_position();
  C_weightsupports::GetInstance().BWS_run_motor();
  C_weightsupports::GetInstance().BWS_clear_step_pin();

  // Check the manual encoder
  C_manualencoder::GetInstance().Time_diff_rot_enc();

  communicate();
}
