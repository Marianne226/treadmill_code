#ifndef _MANUALENCODER_H_
#define _MANUALENCODER_H_

#include "Arduino.h"
#include "parameters.h"

#define ENC_ROT_PIN_A 30
#define ENC_ROT_PIN_B 31
#define PUSH_BUTTON_ENC_ROT 33

#define FORWARD 0
#define BACKWARD 1

class C_manualencoder
{
    public:
        static C_manualencoder& GetInstance() {
            static C_manualencoder _instance;
            return _instance;
        };

        // Public methods of the classs        
        static void Time_diff_rot_enc();
        
        static void Manualencoder_isr();

        static void Button_pushed_Manualencoder_isr();


    private:
        //Singleton specific
        C_manualencoder();
        C_manualencoder(const C_manualencoder&) = delete;
        C_manualencoder& operator=(const C_manualencoder&) = delete;

        //variables

        //Deduced control parameters
        static int short c_hysteresis;
        static int short c_change;
        static double c_crement; //0.5;

        //Time variable for control
        static unsigned long c_last_time_marker_enc;

        static unsigned long c_time_diff_us_ENC;

        static unsigned c_prev_time;
        static unsigned c_delta_time;  //us

        static unsigned long c_last_time_marker_push_button;
        static unsigned long c_control_interval_push_button; //in us

};

#endif