#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "Arduino.h"

#define FORWARD 0
#define BACKWARD 1

#define FSM_Idle 0
#define FSM_moving_error 1
#define FSM_off_error 2
#define FSM_Compensate 3
#define FSM_off 4
#define FSM_calibration 5
#define FSM_base_pos 6
#define FSM_Moving 7
#define FSM_add_mouse 8


class C_parameters
{
    public:
        static C_parameters& GetInstance() {
            static C_parameters _instance;
            return _instance;
        };

        // Treadmill speed 
        void Set_TM_speed(double TM_speed);
        double Get_TM_speed(){ return c_TM_speed; };
        void Add_to_TM_speed(double TM_speed_add_on);

        void Set_TM_speed_actual(double TM_speed_actual){c_TM_speed_actual = TM_speed_actual;};
        double Get_TM_speed_actual(){ return c_TM_speed_actual;};

        void Set_TM_on_off(unsigned int TM_on_off){ c_TM_on_off = TM_on_off; };
        unsigned int Get_TM_on_off(){ return c_TM_on_off; };

        unsigned int Get_TM_direction(){ return c_TM_direction; };

        // Species
        void Set_species(unsigned int species){ c_species = species; };
        unsigned int Get_species(){ return c_species; };

        // Species weight
        void Set_weight(unsigned int weight){ c_weight = weight; };
        unsigned int Get_weight(){ return c_weight; };
        bool Set_weight_change_state(unsigned int weight); 

        // BWS for compensation
        void Set_bws(unsigned int bws){ c_bws = bws; };
        bool Set_bws_change_state(unsigned int bws);
        unsigned int Get_bws(){ return c_bws; };

        // Time
        void Update_time_us(){ c_time_us = micros(); };
        unsigned long Get_time_us(){ return c_time_us; };

        // Changes register
        void Set_changes_register(int bit ){ c_changes_register[bit] = '1'; };
        void Reset_changes_register(){ c_changes_register = "000000"; };
        String Get_changes_register(){ return c_changes_register;};

        // FSM states
        void Set_FSM_on_off_state_seperate();
        void Set_FSM_State_seperate(unsigned int state){c_FSM_state_seperate[Get_species()-1] = state; };
        unsigned int Get_FSM_state_seperate(){ return c_FSM_state_seperate[Get_species()-1]; };

        //hardware constants getters
        double Get_R_pulley() { return c_R_pulley; };
        double Get_signals_per_turn_pulleyy() { return c_signals_per_turn_pulley; };
        double Get_signals_per_turn_motor() { return c_signals_per_turn_motor; };
        double Get_p_length_per_step() { return c_p_length_per_step; };
        double Get_R_rat_spring_cst() { return c_R_rat_spring_cst; };
        double Get_inverse_R_spring_cst() { return c_inverse_R_spring_cst[Get_species()-1]; };

        double Get_ratio_pulley_signals_per_mm() { return c_ratio_pulley_signals_per_mm; };
        double Get_ratio_motor_signals_per_mm() { return c_ratio_motor_signals_per_mm; };
        double Get_inverse_ratio_pulley_signals_per_mm() { return c_inverse_ratio_pulley_signals_per_mm; };
        double Get_inverse_ratio_motor_signals_per_mm() { return c_inverse_ratio_motor_signals_per_mm; };

        double Get_g_constant() { return c_g_constant; };
        double Get_inverse_g_constant() { return c_inverse_g_constant; };

        double Get_weight_support(){return c_weight_support[Get_species()-1];}; //support weight
        double Get_spring_tension(){return c_spring_tension[Get_species()-1];};

        // Base position of the BWS when calibrating
        double Get_base_position(){return c_base_position[Get_species()-1];};

        void Set_move_up_down(int move_up_down){c_move_up_down = move_up_down;}; // adjust the base position when adding rodent
        int Get_move_up_down(){return c_move_up_down;};

        // Rodent addition parameters
        void Set_start_adding_rodent(unsigned int start){c_start_adding_rodent=start;};
        unsigned int Get_start_adding_rodent(){return c_start_adding_rodent;};

        // Target elongation to achieve the BWS
        void Set_target_elongation(int target_elongation){c_target_elongation=target_elongation;};
        double Get_target_elongation(){return c_target_elongation;};

        // Value to know if we already started the rodent adding process
        void Set_initialised_value(bool initialised){c_initialised_value = initialised;};
        bool Get_initialised_value(){return c_initialised_value;};

        // Save the state before turning off to be able to come back to it when turn back on
        unsigned int Get_state_before_off(){return c_state_before_off[Get_species()-1];};
        void Set_state_before_off(unsigned int state_before){c_state_before_off[Get_species()-1] = state_before;};

    private:
        //Singleton specific
        C_parameters() = default;
        C_parameters(const C_parameters&) = delete;
        C_parameters& operator=(const C_parameters&) = delete;

        //constants
        const unsigned long time_max = 4294967295; //4,294,967,295 about 71 min

        // Treadmill speeds
        double c_TM_speed = 0;  // Desired treadmill speed
        double c_TM_speed_actual = 0; // Actual treadmill speed due to progressive acceleration
        unsigned int c_TM_direction = FORWARD;

        unsigned int c_species = 1; //1 = mice, 2 = rats

        // BWS parameters
        unsigned int c_weight = 0;
        unsigned int c_bws = 0;
        unsigned int c_TM_on_off = 0;
        
        //Time variable for control
        unsigned long c_time_us = micros();

        // changes register to indicate to raspberry pi
        String c_changes_register = "000000";

        //FSM state
        unsigned int c_FSM_state_seperate[2] = {FSM_off, FSM_off};
        
        // Base position of the BWS when calibrating
        double c_base_position[2] = {37.5*500, 43*500}; 
        // adjust the base position when adding rodent
        int c_move_up_down;

        // Rodent addition parameters
        unsigned int c_start_adding_rodent = 0;
        
        // Value to know if we already started the rodent adding process
        bool c_initialised_value = false;

        //hardware constants
        double c_R_pulley = 245/2; // mm, 245mm is the diameter of the pulley with the encoder
        double c_signals_per_turn_pulley = 1000;
        double c_signals_per_turn_motor = 500;
        double c_p_length_per_step = 2; // 2mm/turn
        double c_R_rat_spring_cst = 0.02; // N/mm = kg.m.s⁻2/mm
        double c_R_mouse_spring_cst = 0.0033;
        double c_inverse_R_spring_cst[2] = {1/c_R_mouse_spring_cst,1/c_R_rat_spring_cst}; // mm/N

        double c_ratio_pulley_signals_per_mm = c_signals_per_turn_pulley/(2*3.14*c_R_pulley);
        double c_ratio_motor_signals_per_mm = c_signals_per_turn_motor/c_p_length_per_step;
        double c_inverse_ratio_pulley_signals_per_mm = 1/c_ratio_pulley_signals_per_mm;  //  = 1/c_ratio_pulley_signals_per_mm
        double c_inverse_ratio_motor_signals_per_mm = 1/c_ratio_motor_signals_per_mm;  //  = 1/c_ratio_motor_signals_per_mm
        
        double c_g_constant = 9.81;  // m/s⁻2
        double c_inverse_g_constant = 1/9.81;

        double c_weight_support[2] = {13.2, 46.2};
        double c_spring_tension[2] = {0.05,0.14}; 

        double c_target_elongation = 0;

        // Value to know if we already started the rodent adding process
        unsigned int c_state_before_off[2] = {FSM_calibration, FSM_calibration};
};  

#endif