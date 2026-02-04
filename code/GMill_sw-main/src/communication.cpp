#include <math.h>
#include <Arduino.h>
//#include <string.h>
#include "communication.h"


void setup_communication(){
    Serial.begin(115200);
}


void communicate(){
    String changes_receive = "00000000";
    String data_send = "";
    String changes_send = "00000";

    if (Serial.available() > 0) {

        // RECEIVE data
        String data = Serial.readStringUntil('\n');
    
        //parse data
        if (data[0] == '<'){
            for (unsigned int k = 0; k < 8; k++){
                changes_receive[k] = data[k+1];
            }
            String weight = String(data[10]) + String(data[11]) + String(data[12]);
            int weight_i = weight.toInt();

            String bws = String(data[14]) + String(data[15]) + String(data[16]);
            int bws_i = bws.toInt();

            String tm_speed = String(data[18]) + String(data[19]);
            int tm_speed_i = tm_speed.toInt();

            String tm_on_off = String(data[21]);
            int tm_on_off_i = tm_on_off.toInt();

            String species = String(data[23]);
            int species_i = species.toInt();

            String added_rodent = String(data[25]);
            int added_rodent_i = added_rodent.toInt();

            String move_up_down = String(data[27]) + String(data[28]);
            int move_up_down_i = move_up_down.toInt();

            unsigned int k = 0;
            if (changes_receive[k++] == '1'){   
                C_weightsupports::GetInstance().weight_changed(weight_i);
            }
            if (changes_receive[k++] == '1'){
                C_weightsupports::GetInstance().bws_changed(bws_i);
            }
            if (changes_receive[k++] == '1'){
                C_parameters::GetInstance().Set_TM_speed(tm_speed_i);
            }
            if (changes_receive[k++] == '1'){
                C_parameters::GetInstance().Set_TM_on_off(tm_on_off_i);
                C_parameters::GetInstance().Set_FSM_on_off_state_seperate();
            }
            if (changes_receive[k++] == '1'){
                C_parameters::GetInstance().Set_species(species_i);
            }
            if (changes_receive[k++] == '1'){
                C_parameters::GetInstance().Set_start_adding_rodent(added_rodent_i);
            }
            if (changes_receive[k++] == '1'){
                C_parameters::GetInstance().Set_move_up_down(move_up_down_i);
            }
            if (changes_receive[k++] == '1'){
                C_parameters::GetInstance().Set_FSM_State_seperate(FSM_calibration);
                C_parameters::GetInstance().Set_changes_register(3);
                C_parameters::GetInstance().Set_initialised_value(false);
            }

            // SEND data
            changes_send = C_parameters::GetInstance().Get_changes_register();

            int TM_speed = int (C_parameters::GetInstance().Get_TM_speed());
            String string_TM_speed = String(TM_speed);

            while (string_TM_speed.length()<2) {
                string_TM_speed = "0" + string_TM_speed;
            }

            int species_send = int (C_parameters::GetInstance().Get_species());
            int c_BWSE_mes_pos =  int(C_weightsupports::GetInstance().Get_BWSE_mes_pos(species_send-1));
            int c_BWS_mes_pos = int(C_weightsupports::GetInstance().Get_BWS_mes_pos(species_send-1));
            unsigned int c_start_adding_rodent = C_parameters::GetInstance().Get_start_adding_rodent();
            unsigned int c_fsm_state = C_parameters::GetInstance().Get_FSM_state_seperate();
            unsigned int previous_fsm_state = C_parameters::GetInstance().Get_state_before_off();

            String string_species_send = String(species_send);
            String string_c_BWSE_mes_pos = String(c_BWSE_mes_pos);
            String string_c_BWS_mes_pos = String(c_BWS_mes_pos);
            String string_c_start_adding_rodent = String(c_start_adding_rodent);
            String string_c_fsm_state = String(c_fsm_state);
            String string_previous_fsm_state = String(previous_fsm_state);

            data_send = "<" + changes_send +  "|" + string_TM_speed + "|" + string_species_send + "|" + string_c_start_adding_rodent + "|" + string_c_fsm_state + "|" +  string_previous_fsm_state + "|" +  string_c_BWSE_mes_pos + "|" +string_c_BWS_mes_pos+ ">";
            Serial.println(data_send); // Sends the data to the raspberry pi
            C_parameters::GetInstance().Reset_changes_register();
        }
    }
}