#ifndef _TREADMILLS_H_
#define _TREADMILLS_H_

#include "Arduino.h"
#include "parameters.h"
#include <math.h>

//motor 1
#define TM1_STEP 2
#define TM1_DIR 52
#define TM1_EN 53
//motor2
#define TM2_STEP 3
#define TM2_DIR 50
#define TM2_EN 51

#define FORWARD 0
#define BACKWARD 1

class C_treadmills
{
    public:
        static C_treadmills& GetInstance(){
            static C_treadmills _instance;
            return _instance;
        };
        
        // Public methods of the class
        unsigned long Get_time_max(){ return c_time_max; };

        void Set_TM_control_interval(unsigned int TM_control_interval){ c_TM_control_interval = c_TM_control_interval; };
        unsigned int Get_TM_control_interval(){ return c_TM_control_interval; };

        void Udpade_last_time_marker(){ c_last_time_marker = C_parameters::GetInstance().Get_time_us(); };
        unsigned long Get_last_time_marker(){ return c_last_time_marker; };
        void Check_time_marker_overflows();

        double Calc_time_delay();
        void TM_clear_step_pin();
        
        void TM_control();
        void Enable_motor_as_species();
        void Progressive_acceleration();



    private:
        //Singleton specific
        C_treadmills();
        C_treadmills(const C_treadmills&) = delete;
        C_treadmills& operator=(const C_treadmills&) = delete;

        //constants
        const unsigned long c_time_max = 4294967295; //4,294,967,295 about 71 min
        
        //Deduced control parameters
        unsigned int c_TM_control_interval = 0; //0 means infinite time

        //Time variable for control
        unsigned long c_last_time_marker = micros();

};

#endif