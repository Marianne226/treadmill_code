/*
    Controlling multiple steppers with the AccelStepper and MultiStepper library

     by Dejan, https://howtomechatronics.com
*/

//#include <Serial.h>
//#include <AccelStepper.h>
//#include <MultiStepper.h>
#include <math.h>

//#define STEP_PIN 2
//#define DIR_PIN 3
#define ENC_1_PIN_A 22
#define ENC_1_PIN_B 23
#define ENC_2_PIN_A 24
#define ENC_2_PIN_B 25

#define ENC_ROT_PIN_A 30
#define ENC_ROT_PIN_B 31
#define PUSH_BUTTON_ENC_ROT 33

#define END_SWITCH_1_1 38
#define END_SWITCH_1_2 39
#define END_SWITCH_2_1 36
#define END_SWITCH_2_2 37

#define CLK_WISE 1
#define NO_DIRECTION 0
#define CCLK_WISE -1


//for testing
#define BWS_nb_max_pos 360

//constants
const unsigned long time_max = 4294967295; //4,294,967,295 about 71 min


//User entries
double TM_speed = 0;  // in ms (mili seconds)
unsigned int species = 0;


//Deduced control parameters
unsigned int TM_control_interval = 0; //0 means infinite time
unsigned int TM_direction = 1;

unsigned int BWS_control_interval = 2000; //in us, 0  means infinite time

unsigned int enc_rot_control_interval = 0; //in us, 0  means infinite time

int long BWS_mot_dir_1 = 0;
int long BWS_calc_pos_1 = 0;
int long BWS_des_pos_1 = BWS_nb_max_pos;
int long BWS_mes_pos_1 = 0;
int short BWS_enc_dir_1 = 0;

int long BWS_mot_dir_2 = 0;
int long BWS_calc_pos_2 = 0;
int long BWS_des_pos_2 = BWS_nb_max_pos;
int long BWS_mes_pos_2 = 0;
int short BWS_enc_dir_2 = 0;

//end switches
int switchState_1_1 = 0;
int switchState_1_2 = 0;
int switchState_2_1 = 0;
int switchState_2_2 = 0;

//Rotary encoder with push button
int short hysteresis_enc_rot = 0;
int short button_enc_rot = 0;
int short change_enc_rot = 0;
//int short inc_enc_rot = 0;
//int short dec_enc_rot = 0;
double crement_enc_rot = 1; //0.5;


int short switch_to_direction = 0;


//Time variable for control
unsigned long last_time_marker = micros();

unsigned long last_time_marker_BWS = micros();

unsigned long last_time_marker_enc_rot = micros();

unsigned long time_diff_us = 0;

unsigned long time_us = 0;

unsigned prev_time_rot_enc = 0;
unsigned delta_time_rot_enc = 500;  //us

unsigned long last_time_marker_push_button_rot_enc = micros();;
unsigned long control_interval_push_button_rot_enc = 5000; //in us

void button_pushed_enc_rot_isr(){
  if ((time_us - last_time_marker_push_button_rot_enc) > control_interval_push_button_rot_enc) {
    if (species == 1){
      species = 2;
    } else if (species == 2 || species == 0 ){
      species = 1;
    } else {
      species = 0;
    }
  }
}

void regulate_TM_speed(){
  //check for speed range
  if (TM_speed > 80){
    TM_speed = 80;
  } else if (TM_speed < -5){
    TM_speed = -5;
  }
}

void time_diff_rot_enc(){
  noInterrupts();
  time_diff_us = time_us - last_time_marker_enc_rot;
  if(change_enc_rot == 0 && time_diff_us > enc_rot_control_interval){
    last_time_marker_enc_rot = time_us;
    change_enc_rot = 1;
    if (hysteresis_enc_rot > 0){
      TM_speed += crement_enc_rot * (hysteresis_enc_rot);
    } else if (hysteresis_enc_rot < 0){
      TM_speed += crement_enc_rot * (hysteresis_enc_rot);
    }
    hysteresis_enc_rot = 0;
  }
  interrupts();
}

void encoder_rot_isr() {
  unsigned time = micros();
  if (time - prev_time_rot_enc > delta_time_rot_enc){
  //if (change_enc_rot == 1){
    change_enc_rot = 0;
    if (digitalRead(ENC_ROT_PIN_B) == HIGH && hysteresis_enc_rot >= 0) {
      // clockwise rotation
      //TM_speed += crement_enc_rot;
      hysteresis_enc_rot += 1;
      //Serial.print("enc clockwise");
    } else if (digitalRead(ENC_ROT_PIN_B) == LOW && hysteresis_enc_rot <= 0) {
      //counter-clockwise rotation
      //TM_speed -= crement_enc_rot;
      hysteresis_enc_rot -= 1;
      //Serial.print("enc counter-clockwise");
    } 
  //}
    prev_time_rot_enc = time;
  }
}

void read_end_swicthes(){
  switchState_1_1 = digitalRead(END_SWITCH_1_1);
  switchState_1_2 = digitalRead(END_SWITCH_1_2);
  switchState_2_1 = digitalRead(END_SWITCH_2_1);
  switchState_2_2 = digitalRead(END_SWITCH_2_2);
}

void logic_end_switch(){
  //switch_1_1
  if(switchState_1_1 == LOW) {
    // switch is closed
    switch_to_direction = CLK_WISE;
    //Serial.print("switch_to_direction = CLK_WISE");
  } else
  //switch_1_2
  if(switchState_1_2 == LOW) {
    // switch is closed
    switch_to_direction = CCLK_WISE;
    //Serial.print("switch_to_direction = CCLK_WISE");
  } else 
  //switch_1_1
  if(switchState_2_1 == LOW) {
    // switch is closed
    switch_to_direction = CLK_WISE;
    //Serial.print("switch_to_direction = CLK_WISE");
  } else
  //switch_2_2
  if(switchState_2_2 == LOW) {
    // switch is closed
    switch_to_direction = CCLK_WISE;
    //Serial.print("switch_to_direction = CCLK_WISE");
  } else {
    switch_to_direction = NO_DIRECTION;
    //Serial.print("switch_to_direction = NO_DIRECTION");
  }
}


void BWS_run_motor(){
  time_diff_us = time_us - last_time_marker_BWS;
  if (time_diff_us >= BWS_control_interval){
    last_time_marker_BWS = time_us;
    if (BWS_mot_dir_1==CLK_WISE){
      PIOC->PIO_SODR = PIO_PC15; //set direction
      PIOC->PIO_SODR = PIO_PC26; //make step
      //Serial.print("step clockwise");
    } else if (BWS_mot_dir_1==CCLK_WISE){
      PIOC->PIO_CODR = PIO_PC15; //set direction
      PIOC->PIO_SODR = PIO_PC26; //make step
      //Serial.print("step counter-clockwise");
    }
    if (BWS_mot_dir_2==CLK_WISE){
      PIOC->PIO_SODR = PIO_PC17; //set direction
      PIOC->PIO_SODR = PIO_PC25; //make step
      //Serial.print("step clockwise");
    } else if (BWS_mot_dir_2==CCLK_WISE){
      PIOC->PIO_CODR = PIO_PC17; //set direction
      PIOC->PIO_SODR = PIO_PC25; //make step
      //Serial.print("step counter-clockwise");
    }
  }
}

void encoder_1_isr() {
  
  if (digitalRead(ENC_1_PIN_B) == HIGH) {
    // clockwise rotation
    BWS_enc_dir_1=CLK_WISE;
    BWS_mes_pos_1++;
    //Serial.print("enc clockwise");
  } else {
    //counter-clockwise rotation
    BWS_enc_dir_1=CCLK_WISE;
    BWS_mes_pos_1--;    
    //Serial.print("enc counter-clockwise");
  } 
}

void encoder_2_isr() {
  
  if (digitalRead(ENC_2_PIN_B) == HIGH) {
    // clockwise rotation
    BWS_enc_dir_2=CLK_WISE;
    BWS_mes_pos_2++;
    //Serial.print("enc clockwise");
  } else {
    //counter-clockwise rotation
    BWS_enc_dir_2=CCLK_WISE;
    BWS_mes_pos_2--;    
    //Serial.print("enc counter-clockwise");
  } 
}

void BWS_check_positin(){
  if ((BWS_mes_pos_1 > BWS_des_pos_1 && BWS_enc_dir_1 == CLK_WISE) 
  || (switch_to_direction == CCLK_WISE)){
    BWS_des_pos_1 = -BWS_nb_max_pos;
    BWS_mot_dir_1 = CCLK_WISE;
    //Serial.print("change rotation counter clockwise");
  } else if ((BWS_mes_pos_1 < BWS_des_pos_1 && BWS_enc_dir_1 == CCLK_WISE) 
  || (switch_to_direction == CLK_WISE)){
    BWS_des_pos_1 = BWS_nb_max_pos;
    BWS_mot_dir_1 = CLK_WISE;
    //Serial.print("change rotation clockwise");
  }
  if ((BWS_mes_pos_2 > BWS_des_pos_2 && BWS_enc_dir_2 == CLK_WISE) 
  || (switch_to_direction == CCLK_WISE)){
    BWS_des_pos_2 = -BWS_nb_max_pos;
    BWS_mot_dir_2 = CCLK_WISE;
    //Serial.print("change rotation counter clockwise");
  } else if ((BWS_mes_pos_2 < BWS_des_pos_2 && BWS_enc_dir_2 == CCLK_WISE) 
  || (switch_to_direction == CLK_WISE)){
    BWS_des_pos_2 = BWS_nb_max_pos;
    BWS_mot_dir_2 = CLK_WISE;
    //Serial.print("change rotation clockwise");
  }
}

void setup_interrupts(){
  // set interrupt for encoders 
  attachInterrupt(digitalPinToInterrupt(ENC_1_PIN_A), encoder_1_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_2_PIN_A), encoder_2_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_ROT_PIN_A), encoder_rot_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_ENC_ROT), button_pushed_enc_rot_isr, RISING);
}

void set_direction(){ //for TM
  if (TM_direction == 0){
    PIOB->PIO_CODR = PIO_PB21;
    PIOC->PIO_CODR = PIO_PC13;
  } else {
    PIOB->PIO_SODR = PIO_PB21;
    PIOC->PIO_SODR = PIO_PC13;
  }
}

//enables the pair of TM + BWS motors depending on teh selected specie
void enable_motor_as_species(){ //1 for mice, 2 for rats
  //select TM for species 
  if (species == 1){ //clear bit to enable, set bit to disable
    //TM motors
    PIOB->PIO_CODR = PIO_PB14;
    PIOC->PIO_SODR = PIO_PC12;
    //BWS motors
    PIOC->PIO_CODR = PIO_PC14; 
    PIOC->PIO_SODR = PIO_PC16;
  } else if (species == 2){ //clear bit to enable, set bit to disable
    PIOB->PIO_SODR = PIO_PB14;
    PIOC->PIO_CODR = PIO_PC12;
    //BWS motors
    PIOC->PIO_SODR = PIO_PC14; 
    PIOC->PIO_CODR = PIO_PC16;
  } else { //clear bit to enable, set bit to disable
    PIOB->PIO_SODR = PIO_PB14;
    PIOC->PIO_SODR = PIO_PC12;
    //BWS motors
    PIOC->PIO_SODR = PIO_PC14; 
    PIOC->PIO_SODR = PIO_PC16;
  }

  //disable motors, if zero speed
  if (TM_speed < 0.1 && TM_speed > -0.1){
    //Serial.print("D disable motors D ");
    PIOB->PIO_SODR = PIO_PB14;
    PIOC->PIO_SODR = PIO_PC12;
  }
}

void update_time_us(){
  time_us = micros();
}

void check_time_marker_overflows(){
  if (last_time_marker > time_max) {
    last_time_marker = 0;
  }/* else if (time_us > time_max) {
    Serial.print("Errror with time_us too big");
  }*/
}


double calc_time_delay() { //calculates the time between each step
  //Robot's constants
  double TM_radius = 2.5; //in cm
  unsigned int TM_motor_steps = 200;
  unsigned int TM_step_division_factor = 2; //full step(1/1) = 1, half step(1/2) = 2 , quarter(1/4) = 4, an eight(1/8) = 8, an sixtheen(1/16) = 16 

  //Robot's calculated constants
  long double TM_perimeter = 2*M_PI*TM_radius; //in cm
  long double TM_omega = abs(TM_speed)/TM_perimeter; //in Hz
  long double TM_signals_per_turn = TM_motor_steps*TM_step_division_factor; //in signals
  long double TM_signals_per_seconds = TM_omega*TM_signals_per_turn; //in signals/s

  //Result
  //if (TM_speed == 0){
  //  TM_control_interval = 1 << 63;
  //} else {
  TM_control_interval = 1/TM_signals_per_seconds * 1000000; //in us (micro seconds)
  //}
}

void TM_clear_step_pin(){
  //TM
  PIOB->PIO_CODR = PIO_PB25; //pin 2 STEP
  PIOC->PIO_CODR = PIO_PC28; //pin 3 STEP
  //digitalWrite(STEP_PIN, LOW); //STEP
  //BWS
  PIOC->PIO_CODR = PIO_PC26; //pin 4 STEP
  PIOC->PIO_CODR = PIO_PC25; //pin 5 STEP
}

void TM_control(){
  time_diff_us = time_us - last_time_marker;
  if (TM_speed != 0){
    if (TM_speed > 0){
      PIOB->PIO_CODR = PIO_PB21;
      PIOC->PIO_CODR = PIO_PC13;
    } else {
      PIOB->PIO_SODR = PIO_PB21;
      PIOC->PIO_SODR = PIO_PC13;
    }
    
    if (time_diff_us >= TM_control_interval){
      last_time_marker = time_us;
      //digitalWrite(DIR_PIN, HIGH); //DIR
      //digitalWrite(STEP_PIN, HIGH); //STEP
      PIOB->PIO_SODR = PIO_PB25; //pin 2 STEP
      PIOC->PIO_SODR = PIO_PC28; //pin 3 STEP
      //Serial.print("!Step trig! ");
    }
  } else {
    last_time_marker = time_us;
  }
}

void set_pins(){
  //TM motor 1
  pinMode(2, OUTPUT); //step pin
  pinMode(52, OUTPUT); //dir pin
  pinMode(53, OUTPUT); //enable pin
  //TM motor 2
  pinMode(3, OUTPUT); //step pin
  pinMode(50, OUTPUT); //dir pin
  pinMode(51, OUTPUT); //enable pin
  //BWS motor 1
  pinMode(4, OUTPUT); //step pin
  pinMode(48, OUTPUT); //dir pin
  pinMode(49, OUTPUT); //enable pin
  //BWS motor 2
  pinMode(5, OUTPUT); //step pin
  pinMode(46, OUTPUT); //dir pin
  pinMode(47, OUTPUT); //enable pin

  //encoders
  //set interrupt pins
  pinMode(ENC_1_PIN_A, INPUT_PULLUP);
  pinMode(ENC_2_PIN_A, INPUT_PULLUP);
  //set line B
  pinMode(ENC_1_PIN_B, INPUT);
  pinMode(ENC_2_PIN_B, INPUT);

  //rotary encoder 
  //set interrupt pins
  pinMode(ENC_ROT_PIN_A, INPUT_PULLUP);
  //set line B
  pinMode(ENC_ROT_PIN_B, INPUT_PULLUP);
  //button
  pinMode(PUSH_BUTTON_ENC_ROT, INPUT_PULLUP);


  //switches
  pinMode(END_SWITCH_1_1, INPUT_PULLUP);
  pinMode(END_SWITCH_1_2, INPUT_PULLUP);
  pinMode(END_SWITCH_2_1, INPUT_PULLUP);
  pinMode(END_SWITCH_2_2, INPUT_PULLUP);


 //for tests
 
  //pinMode(26, OUTPUT); //enable pin
}

void setup() {
  Serial.begin(115200); //9600);

  set_pins();
  setup_interrupts();

  set_direction();
  enable_motor_as_species();

  calc_time_delay();

  /*stepper1.setMaxSpeed(2040); // Set maximum speed value for the stepper

  // Adding the 3 steppers to the steppersControl instance for multi stepper control
  steppersControl.addStepper(stepper1);

  gotoposition[0] = 0;
  */
}

void loop() {
  while(1){

    //for test
    //PIOD->PIO_SODR = PIO_PD1;
    //digitalWrite(26, HIGH);

    //Serial.print("TM_speed: ");
    //Serial.print(TM_speed);
    update_time_us();
    check_time_marker_overflows();
    enable_motor_as_species();
    /*Serial.print("last_time_marker:");
    Serial.print(last_time_marker);
    SeriSal.print(",");
    Serial.print("time_us:");
    Serial.println(time_us);
    */

    //TM_speed = 80;
    calc_time_delay();
    update_time_us();  //repeating, usefull?
    TM_control();
    //Serial.print("TM_control_interval:");
    //Serial.println(TM_control_interval);
    //delayMicroseconds(100);

    BWS_check_positin();
    BWS_run_motor();

    read_end_swicthes();
    logic_end_switch();

    time_diff_rot_enc();
    regulate_TM_speed();
    
    TM_clear_step_pin();


    /*
    unsigned long StartTime = millis();
    steppersControl.runSpeedToPosition();
    unsigned long CurrentTime = millis();
    unsigned long ElapsedTime = CurrentTime - StartTime;
    Serial.println("ElapsedTime : " + String(ElapsedTime) + "\n");
    */

    //delay(1000);
  //Serial.println("new_loop");
  //Serial.print(TM_control_interval);
  //Serial.print(" , ");
  //Serial.println(TM_speed);
  
    //for test
    //digitalWrite(26, LOW);

  }

}