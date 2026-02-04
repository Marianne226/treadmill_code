#ifndef _WEIGHTSUPPORTS_H_
#define _WEIGHTSUPPORTS_H_

#include "Arduino.h"
#include "parameters.h"
#include "bwsencoders.h"

//motor 1
#define BWS1_STEP 4
#define BWS1_DIR 48
#define BWS1_EN 49
//motor2
#define BWS2_STEP 5
#define BWS2_DIR 46
#define BWS2_EN 47

//rotation direction
#define CLK_WISE 1
#define NO_DIRECTION 0
#define CCLK_WISE -1

#define BWS_MAX_POS 500000  //500

//time constant in us (micro seconds)
#define BWS_MOTOR_CONTROL_INTERVAL 800 //originally: 1000 //2000
#define DELTA_MARGIN_MOTOR_DIRECTION_CHANGE 500000 //1000000

#define SET_BOTTOM 1

class C_weightsupports
{
    public:
        static C_weightsupports& GetInstance(){
            static C_weightsupports _instance;
            return _instance;
        };
        
        // Public methods of the class
        void BWS_clear_step_pin();
        void BWS_enable_motor_as_species();
        void BWS_run_motor();
        void BWS_control_motor_position();
        void BWS_check_endswitches();

        void Control_bws_compensation();
        void Calibrate_motor();

        void Go_to_desired_position(unsigned int c_FSM_state);

        void Increment_BWS_mes_pos(unsigned int species){ c_BWS_mes_pos[species]++; };
        void Decrement_BWS_mes_pos(unsigned int species){ c_BWS_mes_pos[species]--; };
        int  Get_BWS_mes_pos(unsigned int species){ return c_BWS_mes_pos[species]; };
        void reset_BWS_mes_pos(unsigned int species) {c_BWS_mes_pos[species] = 0; };

        bool Get_bws_ss_internal_up(unsigned int species) { return c_bws_ss_internal_up[species]; };
        void Set_bws_ss_internal_up( bool bws_ss_internal_up, unsigned int species) { c_bws_ss_internal_up[species] = bws_ss_internal_up; };
        bool Get_bws_ss_internal_down(unsigned int species) { return c_bws_ss_internal_down[species]; };
        void Set_bws_ss_internal_down( bool bws_ss_internal_down, unsigned int species) { c_bws_ss_internal_down[species] = bws_ss_internal_down; };

        bool Get_bws_ss_external_up(unsigned int species) { return c_bws_ss_external_up[species]; };
        void Set_bws_ss_external_up( bool bws_ss_external_up, unsigned int species) { c_bws_ss_external_up[species] = bws_ss_external_up; };
        bool Get_bws_ss_external_down(unsigned int species) { return c_bws_ss_external_down[species]; };
        void Set_bws_ss_external_down( bool bws_ss_external_down, unsigned int species) { c_bws_ss_external_down[species] = bws_ss_external_down; };

        void Increment_BWSE_mes_pos(unsigned int species){ c_BWSE_mes_pos[species]++; };
        void Decrement_BWSE_mes_pos(unsigned int species){ c_BWSE_mes_pos[species]--; };
        int  Get_BWSE_mes_pos(unsigned int species){ return c_BWSE_mes_pos[species]; };
        void reset_BWSE_mes_pos(unsigned int species);

        void Set_BWSE_enc_dir_CLK_WISE(unsigned int species) { c_BWSE_enc_dir[species] = CLK_WISE; };
        void Set_BWSE_enc_dir_CCLK_WISE(unsigned int species) { c_BWSE_enc_dir[species] = CCLK_WISE; };

        void initialise_adding_rodent();
        void check_finished_adding();
        void check_BWSE_and_compensate();
        double spring_deformation();

        void bws_changed(unsigned int bws);
        void weight_changed(unsigned int weight);

        void check_up_down();

    private:
        //Singleton specific
        C_weightsupports();
        C_weightsupports(const C_weightsupports&) = delete;
        C_weightsupports& operator=(const C_weightsupports&) = delete;

        int c_BWS_mes_pos[2] = {0, 0};
        int c_BWS_des_pos[2] = {0, 0};

        int short c_switch_to_direction[2] = {0, 0};

        unsigned short c_behaviour_bws_ss_internal_up[2] = {FALLING, FALLING};
        unsigned short c_behaviour_bws_ss_internal_down[2] = {FALLING, FALLING};
        unsigned short c_behaviour_bws_ss_external_up[2] = {FALLING, FALLING};
        unsigned short c_behaviour_bws_ss_external_down[2] = {FALLING, FALLING};

        bool c_bws_ss_internal_up[2] = {false, false};
        bool c_bws_ss_internal_down[2] = {false, false};
        bool c_bws_ss_external_up[2] = {false, false};
        bool c_bws_ss_external_down[2] = {false, false};

        //variables of BWS external encoder

        int c_BWSE_mes_pos[2] = {0, 0};
        int c_BWSE_enc_dir[2] = {0, 0};
        
        //Deduced control parameters
        unsigned int c_BWS_control_interval = BWS_MOTOR_CONTROL_INTERVAL; 

        //Time variable for control
        unsigned long c_time_diff_us_BWS = 0;
        
        unsigned long c_last_time_marker_BWS = micros();

        unsigned int c_calibration_state = SET_BOTTOM;

};

#endif