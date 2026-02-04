# python3 /home/.G-Mill/Desktop/Project/.G-Mill.py

import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
import matplotlib
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import time
import math

import threading
import serial
import time
from bitarray import bitarray

matplotlib.use('TkAgg')

from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)



changed_values_send = bitarray(8)
changed_values_send.setall(0)
changed_values_receive = bitarray(6)
changed_values_receive.setall(0)

bws_value = int
tm_speed_value = int
tm_on_off_value = int
species_value = int

weight_value = 0 #self.weight_value
bws_value = 0 #self.bws_value
tm_speed_value = 0 #self.tm_speed_value
tm_on_off_value = 0 #self.tm_on_off_value
species_value = 1 #self.species_value #2 #rat

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
# Get rid of garbage/incomplete data
ser.flush()


def weight_value_changed(var, index, mode):
    changed_values_send[0] = 1

def bws_value_changed(var=None, index=None, mode=None):
    changed_values_send[1] = 1

def tm_speed_value_changed(var, index, mode):
    changed_values_send[2] = 1

def rodent_added_changed():
    changed_values_send[5] = 1

def move_up_down_value_changed(var,index,mode):
    changed_values_send[6] = 1


def Communication(self):
    print('communication round')
    #SEND to arduino
    weight_value = self.weight_value.get()
    bws_value = self.bws_value.get()
    tm_speed_value = self.tm_speed_value.get()
    tm_on_off_value = self.tm_on_off_value.get()
    species_value = self.species_value.get() #2 #rat
    rodent_added_value = self.start_adding_rodent.get()
    move_up_down_value = self.move_up_down_value.get()

    if species_value == 'Rats':
        species_value = 2
    else :
        species_value = 1
    print('species value: ', species_value)

    string_weight = str(weight_value)
    while (len(string_weight)<3):
        string_weight = "0"+string_weight
    
    string_bws = str(bws_value)
    while (len(string_bws)<3):
        string_bws = "0"+string_bws

    string_tm_speed_value = str(tm_speed_value)
    while (len(string_tm_speed_value)<2):
        string_tm_speed_value = "0"+string_tm_speed_value
    
    string_tm_on_off_value = str(tm_on_off_value)
    while (len(string_tm_on_off_value)<1):
        string_tm_on_off_value = "0"+string_tm_on_off_value
    print("TM value: ", tm_on_off_value)

    string_species_value = str(species_value)
    while (len(string_species_value)<1):
        string_species_value = "0"+string_species_value
    
    string_rodent_added = str(rodent_added_value)
    while (len(string_rodent_added)<1):
        string_rodent_added = "0"+string_rodent_added

    string_move_up_down = str(move_up_down_value)
    while (len(string_move_up_down)<2):
        string_move_up_down = "0"+string_move_up_down


    send_string = ("<"+ changed_values_send.to01() +"|"+ string_weight +"|"+ string_bws
                    +"|"+ string_tm_speed_value +"|"+ string_tm_on_off_value
                    +"|"+ string_species_value +"|"+ string_rodent_added + "|" + string_move_up_down + ">\n")
    
    print(send_string.encode('utf-8'))
    # Send the string
    ser.write(send_string.encode('utf-8'))

    #reset move_up_down to zero
    self.move_up_down_value.set(0)
    
    # Do nothing for 500 milliseconds (0.5 seconds)
    #time.sleep(0.5)

    time.sleep(0.1)
    
    # RECEIVE data from the Arduino
    receive_string = ser.readline().decode('utf-8').rstrip()

    if (len(receive_string)> 15 and receive_string[0] == '<'):
        for i in range(6):
            changed_values_receive[i] = int(receive_string[i+1])

        if (changed_values_receive[0] == 1):
            self.tm_speed_value.set(int(receive_string[8:10:1]))

        if (changed_values_receive[1] == 1):
            species = int(receive_string[11:12:1])
            if species == 2:
                self.species_value.set('Rats')
            elif species == 1:
                self.species_value.set('Mice')

        if (changed_values_receive[2] == 1):
            self.start_adding_rodent.set(int(receive_string[13:14:1]))
            print('add rodent on')
        
        
        if (changed_values_receive[4] == 1 ):
            #the end switch has been pressed --> show an error message
            self.switch_error = 1
            if self.end_switch_error == False: 
                self.initialise_buttons = True
                self.end_switch_error = True
                self.tm_on_off_value.set(0)
                if self.tm_on_off_button['text'] == "TM is ON":
                    self.tm_on_off_button.configure(text="TM is OFF") 
                show_error_message('Error', 'The Body Weight Support system has reached its limit. Please adjust its position with the arrows to resolve the blockage')
        if self.end_switch_error == True and changed_values_receive[4] == 0 :
            self.end_switch_error = False

        if (changed_values_receive[5] == 1):
            #the end switch has been pressed --> show an error message
            self.switch_error = 2
            if self.end_switch_error == False: 
                self.initialise_buttons = True
                self.end_switch_error = True
                self.tm_on_off_value.set(0)
                if self.tm_on_off_button['text'] == "TM is ON":
                    self.tm_on_off_button.configure(text="TM is OFF") 
                show_error_message('Error', 'The Body Weight Support system has reached its limit. Please adjust its position with the arrows to resolve the blockage')
        if self.end_switch_error == True and changed_values_receive[5] == 0 :
            self.end_switch_error = False
        
        if (changed_values_receive[3] == 1):
            state = int(receive_string[15:16:1])
            self.fsm_state.set(state)
            if state == 4 : #FSM_off
                print('click encoder')
                self.button_encoder = True
                self.tm_on_off_value.set(0)
                if self.tm_on_off_button['text'] == "TM is ON":
                    self.tm_on_off_button.configure(text="TM is OFF") 
        


        print('previous state : ', receive_string[17:18:1])
        split_data = receive_string.split('|')
        print(receive_string)
        #print(changed_values_receive)
        current_pos_BWSE = int(split_data[6])
        current_pos_BWS = int(split_data[7].rstrip('>'))
        #print('the current measured position BWSE:', current_pos_BWSE)
        #print('the current measured position BWS:', current_pos_BWS)
        
    changed_values_send.setall(0)
    changed_values_receive.setall(0)

def show_error_message(title, message):
    messagebox.showerror(title, message)

def SignalSource(amplitude=1, frequency=1):
    """Simulate some signal source that returns an xy data point"""
    start_time = time.time()
    twopi = 2 * math.pi
    pi_2 = math.pi * 3/4
    period = 1 / frequency
    yield (0, math.sin(0) * amplitude + amplitude, math.sin(0 - pi_2) * amplitude + amplitude)
    while True:
        x = time.time() - start_time
        phase = x / period * twopi
        yield (x, math.sin(phase) * amplitude + amplitude, math.sin(phase - pi_2) * amplitude + amplitude)

class App(tk.Tk):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        
        self.geometry("800x480")
        self.title('.G-Mill')
        self.resizable(1, 1)
        self.attributes('-zoomed', True)
        

class GuiUserFrame(ttk.Frame):
    def __init__(self, container, *args, **kwargs):
        super().__init__(container, *args, **kwargs)      
        
        self.signal = SignalSource(frequency=0.5)
        self.xdata = []
        self.yrdata = []
        self.yldata = []

        self.weight_value = tk.IntVar(value = 0) #10)
        self.bws_value = tk.IntVar(value = 0)
        self.tm_speed_value = tk.IntVar(value = 0) #5)
        self.tm_on_off_value = tk.IntVar(value = 0)
        self.species_value = tk.StringVar(value = "Mice") #ATS"Mice")
        self.log_on_off_value = ""
        self.leg_log_left_value = tk.IntVar(value = 1)
        self.leg_log_right_value = tk.IntVar(value = 1)
        self.start_adding_rodent = tk.IntVar(value = 0)
        self.move_up_down_value = tk.IntVar(value = 0)
        self.fsm_state = tk.IntVar(value = 4) #off

        self.button_encoder = False
        self.end_switch_error = False
        self.initialise_buttons = False
        self.switch_error = 1
        self.widgets_to_destroy = [] 


        # configure the grid
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)
        self.columnconfigure(3, weight=20)

        self.plot_on = False

        self.create_widgets()
        
        #self.show_add_rodent_popup()

        # add padding to the frame and show it
        self.grid(padx=10, pady=0, sticky=tk.NSEW)

        self.update_gui()
        
    def update_gui(self):
        Communication(self)
        if self.plot_on == True:
            self.update_plot()
        self.after(1, self.update_gui)
        

    def create_widgets(self):
        self.create_top()
        self.create_bottom()
        
        
    def create_top(self):     #top for the user entries
        ########################################################################
        # Create a parent frame to hold both Weight and BWS sections
        settings_frame = ttk.Frame(self)
        settings_frame.grid(column=0, row=1, columnspan=3, padx=10, pady=5, sticky="nsew")

        # Center the parent frame
        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)

        # Weight section
        weight_frame = ttk.LabelFrame(settings_frame, text="", padding=(10, 5))
        weight_frame.grid(column=0, row=0, padx=10, pady=5, sticky="nsew")

        # Add widgets inside the weight_frame
        weight_label = ttk.Label(weight_frame, text="Weight (in g) :")
        weight_label.grid(column=0, row=0, sticky=tk.W, padx=5, pady=1)

        weight_spinbox = ttk.Spinbox(weight_frame,
                                    from_=0, 
                                    to=500,
                                    textvariable=self.weight_value,
                                    wrap=True,
                                    width=3)
        weight_spinbox.grid(column=1, row=0, sticky=tk.E, padx=5, pady=1)

        weight_p1_button = ttk.Button(weight_frame, text="+1g", command=lambda:self.check_weight_add(1))
        weight_p1_button.grid(column=2, row=0, sticky=tk.N, ipadx=5, ipady=10) #test bigger button

        weight_m1_button = ttk.Button(weight_frame, text="-1g", command=lambda:self.check_weight_add(-1))
        weight_m1_button.grid(column=2, row=1, sticky=tk.N, ipadx=5, ipady=10)

        weight_p100_button = ttk.Button(weight_frame, text="+100g", command=lambda:self.check_weight_add(100))
        weight_p100_button.grid(column=0, row=1, sticky=tk.N, ipadx=5, ipady=10)

        weight_p10_button = ttk.Button(weight_frame, text="+10g", command=lambda:self.check_weight_add(10))
        weight_p10_button.grid(column=1, row=1, sticky=tk.N, ipadx=5, ipady=10)

        # BWS section
        bws_frame = ttk.LabelFrame(settings_frame, text="", padding=(10, 5))
        bws_frame.grid(column=1, row=0, padx=10, pady=5, sticky="nsew")

        # Add widgets inside the bws_frame
        bws_label = ttk.Label(bws_frame, text="BWS (in %) :")
        bws_label.grid(column=0, row=0, sticky=tk.W, padx=5, pady=1)

        bws_spinbox = ttk.Spinbox(bws_frame,
                                from_=0, 
                                to=100,
                                textvariable=self.bws_value,
                                wrap=True,
                                width=3)
        bws_spinbox.grid(column=1, row=0, sticky=tk.E, padx=5, pady=1)

        bws_p1_button = ttk.Button(bws_frame, text="+1%", command=lambda:self.check_bws_add(1))
        bws_p1_button.grid(column=2, row=0, ipadx=5, ipady=10)

        bws_m1_button = ttk.Button(bws_frame, text="-1%", command=lambda:self.check_bws_add(-1))
        bws_m1_button.grid(column=2, row=1, ipadx=5, ipady=10)

        bws_p10_button = ttk.Button(bws_frame, text="+10%", command=lambda:self.check_bws_add(10))
        bws_p10_button.grid(column=0, row=1, ipadx=5, ipady=10)

        bws_m10_button = ttk.Button(bws_frame, text="-10%", command=lambda:self.check_bws_add(-10))
        bws_m10_button.grid(column=1, row=1, ipadx=5, ipady=10)

        bws_check_button = ttk.Button(bws_frame, text="Re-compute", command=lambda:bws_value_changed())
        bws_check_button.grid(column=3, row=0, ipadx=5, ipady=10)

        # Create a parent frame to hold TM Speed, and another for Species and Treadmill On/Off
        controls_frame = ttk.Frame(self)
        controls_frame.grid(column=0, row=0, columnspan=3, padx=10, pady=1, sticky="nsew")

        # Center the parent frame
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)

        # TM Speed section
        tm_speed_frame = ttk.LabelFrame(controls_frame, text="", padding=(10, 1))
        tm_speed_frame.grid(column=0, row=0, padx=10, pady=1, sticky="nsew")

        # Add widgets inside the tm_speed_frame
        tm_speed_label = ttk.Label(tm_speed_frame, text="TM speed (in cm/s) :")
        tm_speed_label.grid(column=0, row=0, sticky=tk.W, padx=5, pady=1)

        tm_speed_spinbox = ttk.Spinbox(tm_speed_frame,
                                    from_=-5, 
                                    to=15,
                                    textvariable=self.tm_speed_value,
                                    wrap=True,
                                    width=3)
        tm_speed_spinbox.grid(column=1, row=0, sticky=tk.E, padx=5, pady=1)


        tm_speed_p1_button = ttk.Button(tm_speed_frame, text="+1cm/s", command=lambda:self.check_speed(1))
        tm_speed_p1_button.grid(column=2, row=0, ipadx=5, ipady=10)

        tm_speed_m1_button = ttk.Button(tm_speed_frame, text="-1cm/s", command=lambda:self.check_speed(-1))
        tm_speed_m1_button.grid(column=2, row=1, ipadx=5, ipady=10)

        tm_speed_p10_button = ttk.Button(tm_speed_frame, text="+10cm/s", command=lambda:self.check_speed(10))
        tm_speed_p10_button.grid(column=0, row=1, ipadx=5, ipady=10)

        tm_speed_m10_button = ttk.Button(tm_speed_frame, text="-10cm/s", command=lambda:self.check_speed(-10))
        tm_speed_m10_button.grid(column=1, row=1, ipadx=5, ipady=10)

        # Create a parent frame for Species and Treadmill On/Off sections
        species_tm_on_off_frame = ttk.Frame(controls_frame)
        species_tm_on_off_frame.grid(column=1, row=0, padx=10, pady=5, sticky="nsew")

        # Treadmill On/Off section
        buttons_frame = ttk.LabelFrame(species_tm_on_off_frame, text="", padding=(10, 5))
        buttons_frame.grid(column=0, row=0, padx=10, pady=(0, 5), sticky="nsew")

        # Add widgets inside the tm_on_off_frame
        def switch_TM_state():
            if self.tm_on_off_button['text'] == "TM is ON":
                self.tm_on_off_button.configure(text="TM is OFF")    
                self.tm_on_off_value.set(0)
                self.fsm_state.set(4)
                self.tm_speed_value.set(0)  # Added: When the treadmill is off, it should not run
            else:
                self.tm_on_off_button.configure(text="TM is ON")
                self.tm_on_off_value.set(1)

        self.tm_on_off_button = ttk.Button(buttons_frame, text="TM is OFF", command=switch_TM_state)
        self.tm_on_off_button.grid(column=1, row=0, sticky=tk.W, padx=5, ipady=10)

        restart_calibration_button = ttk.Button(buttons_frame, text="Restart Calibration", command=self.restart_calibration)
        restart_calibration_button.grid(column=2, row=0, sticky=tk.W, padx=5, ipady=10)

        # Species Selection section
        species_frame = ttk.LabelFrame(species_tm_on_off_frame, text="", padding=(10, 5))
        species_frame.grid(column=0, row=1, padx=10, pady=(5, 0), sticky="nsew")

        # Add widgets inside the species_frame
        species_label = ttk.Label(species_frame, text="Species :")
        species_label.grid(column=0, row=0, sticky=tk.W, padx=5, pady=1)

        species_rats_radiobutton = ttk.Radiobutton(species_frame,
                                                text="Rats",
                                                value="Rats",
                                                variable=self.species_value)
        species_rats_radiobutton.grid(column=2, row=0, sticky=tk.W, padx=5, pady=1)

        species_mice_radiobutton = ttk.Radiobutton(species_frame,
                                                text="Mice",
                                                value="Mice",
                                                variable=self.species_value)
        species_mice_radiobutton.grid(column=1, row=0, sticky=tk.W, padx=5, pady=1)


        #calling function when value changes
        self.weight_value.trace_add('write', weight_value_changed)
        self.bws_value.trace_add('write', bws_value_changed)
        self.tm_speed_value.trace_add('write', tm_speed_value_changed)
        self.tm_on_off_value.trace_add('write', self.tm_on_off_value_changed)
        self.species_value.trace_add('write', self.species_value_changed)

        # print("changed_values_send\n")
        # print(changed_values_send)
        # print("\n")

    def check_speed(self, add):
        if self.tm_on_off_value.get() != 0:
            if self.species_value.get() == 'Rats' :
                if self.tm_speed_value.get() + add < 25:
                        if self.tm_speed_value.get() + add > -5:
                            self.tm_speed_value.set(self.tm_speed_value.get() + add)
                        elif self.tm_speed_value.get() != -5:
                            self.tm_speed_value.set(-5)
                elif self.tm_speed_value.get() != 25 :
                    self.tm_speed_value.set(25)
            else :
                if self.tm_speed_value.get() + add < 15:
                        if self.tm_speed_value.get() + add > -5:
                            self.tm_speed_value.set(self.tm_speed_value.get() + add)
                        elif self.tm_speed_value.get() != -5:
                            self.tm_speed_value.set(-5)
                elif self.tm_speed_value.get() != 15:
                    self.tm_speed_value.set(15)


    def check_bws_add(self, add):
        if (add == 1 or add == 10) and (self.bws_value.get() != 100) :
            if self.bws_value.get() + add < 100 :
                self.bws_value.set(self.bws_value.get() + add)
            else :
                self.bws_value.set(100)
        elif (add == -1 or add == -10) and (self.bws_value.get() != 0):
            if self.bws_value.get() + add > 0 :
                self.bws_value.set(self.bws_value.get() + add)
            else :
                self.bws_value.set(0)

    def check_weight_add(self, add):
            if (add == 1 or add == 10 or add == 100) and (self.weight_value.get() != 500) :
                if self.weight_value.get() + add < 500 :
                    self.weight_value.set(self.weight_value.get() + add)
                else :
                    self.weight_value.set(500)
            elif add == -1 and (self.weight_value.get() != 0):
                if self.weight_value.get() + add > 0 :
                    self.weight_value.set(self.weight_value.get() + add)
                else :
                    self.weight_value.set(0)


    def species_value_changed(self, var, index, mode):
        changed_values_send[4] = 1
        #self.show_add_rodent_popup()
    
    def tm_on_off_value_changed(self, var, index, mode):
        print('click gui')
        if self.button_encoder == False :
            print('inside')
            changed_values_send[3] = 1
        else:
            self.button_encoder = False

    def restart_calibration(self):
        self.start_adding_rodent.set(0) 
        if self.tm_on_off_value.get() == 0:
            self.tm_on_off_value.set(True)
            self.tm_on_off_button.configure(text="TM is ON") 
        changed_values_send[7] = 1 
        self.destroy_dynamic_widgets()
        self.label.config(text="Please wait, calibration...")
        self.start_adding_rodent.trace_add('write', self.update_bottom_rodent_adding)

    def create_bottom(self):
        # Create a container frame for the bottom half
        self.bottom_frame = tk.Frame(self)
        self.bottom_frame.grid(row=3, column=0, rowspan=10, columnspan=3, sticky="nsew")
        self.columnconfigure(3, weight=2)

        self.bottom_frame.columnconfigure(0, weight=1)
        self.bottom_frame.columnconfigure(1, weight=1)

        self.label = tk.Label(self.bottom_frame, text="Treadmill is off")
        self.label.grid(row=0, column=0, columnspan=3, pady=10, sticky="n")

        self.fsm_state.trace_add('write', self.update_bottom)
        self.move_up_down_value.trace_add('write', move_up_down_value_changed)

    def update_bottom(self, var=None, index=None, mode=None):
        if self.fsm_state.get() == 4 : #TM_off
            self.destroy_dynamic_widgets()
            self.label.config(text="Treadmill is off")
        elif self.fsm_state.get() == 5 or self.fsm_state.get() == 8 or self.fsm_state == 6: # Calibration or base_pos or add_mouse
            if self.start_adding_rodent.get() == 0:
                self.label.config(text="Please wait, calibration...")
                self.start_adding_rodent.trace_add('write', self.update_bottom_rodent_adding)
            else: # adding rodent
                self.update_bottom_rodent_adding()
        elif self.fsm_state.get() == 0 or self.fsm_state.get() == 3 or self.fsm_state == 7: # idle or compensation or moving
            self.destroy_dynamic_widgets()
            self.label.config(text="Treadmill and body weight support are on")
            arrow_button_frame = tk.Frame(self.bottom_frame)
            arrow_button_frame.grid(row=1, column=1, sticky="w", padx=5, pady=10)
            self.widgets_to_destroy.append(arrow_button_frame)

            # Up and down arrow buttons within arrow_button_frame
            up_arrow_button = tk.Button(arrow_button_frame, text="↑", command=self.adjust_position_up)
            up_arrow_button.pack(side="top", padx=5, pady=5)
            self.widgets_to_destroy.append(up_arrow_button)

            down_arrow_button = tk.Button(arrow_button_frame, text="↓", command=self.adjust_position_down)
            down_arrow_button.pack(side="bottom", padx=5, pady=5)
            self.widgets_to_destroy.append(down_arrow_button)
        elif (self.fsm_state.get() == 2 or self.fsm_state.get() == 1) and self.initialise_buttons == True:
            #in the error state
            self.destroy_dynamic_widgets()
            self.label.config(text="Please use arrows to adjust the position and fix error")
            arrow_button_frame = tk.Frame(self.bottom_frame)
            arrow_button_frame.grid(row=1, column=1, sticky="w", padx=5, pady=10)
            self.widgets_to_destroy.append(arrow_button_frame)

            # Up and down arrow buttons within arrow_button_frame
            if (self.switch_error == 1):
                up_arrow_button = tk.Button(arrow_button_frame, text="↑", command=self.adjust_position_up)
                up_arrow_button.pack(side="top", padx=5, pady=5)
                self.widgets_to_destroy.append(up_arrow_button)
            else :
                down_arrow_button = tk.Button(arrow_button_frame, text="↓", command=self.adjust_position_down)
                down_arrow_button.pack(side="bottom", padx=5, pady=5)
                self.widgets_to_destroy.append(down_arrow_button)
            self.initialise_buttons = False

        

    def update_bottom_rodent_adding(self, var=None, index=None, mode=None):
        # This function will update the right half based on the rodent addition state
        if self.start_adding_rodent.get() == 1:
            # Change the label text to "Start adding rodent"
            self.label.config(text="Start adding rodent")

            # "Adjust position" label on the left
            adjust_label = tk.Label(self.bottom_frame, text="Adjust position")
            adjust_label.grid(row=1, column=0, sticky="e", padx=5, pady=10)
            self.widgets_to_destroy.append(adjust_label)

            arrow_button_frame = tk.Frame(self.bottom_frame)
            arrow_button_frame.grid(row=1, column=1, sticky="w", padx=5, pady=10)
            self.widgets_to_destroy.append(arrow_button_frame)

            # Up and down arrow buttons within arrow_button_frame
            up_arrow_button = tk.Button(arrow_button_frame, text="↑", command=self.adjust_position_up)
            up_arrow_button.pack(side="top", padx=5, pady=5)
            self.widgets_to_destroy.append(up_arrow_button)

            down_arrow_button = tk.Button(arrow_button_frame, text="↓", command=self.adjust_position_down)
            down_arrow_button.pack(side="bottom", padx=5, pady=5)
            self.widgets_to_destroy.append(down_arrow_button)

            # Frame for "Done" button at the bottom
            button_frame = tk.Frame(self.bottom_frame)
            button_frame.grid(row=1, column=1, columnspan=1, pady=10)
            self.widgets_to_destroy.append(button_frame)

            # Add the "Done" button inside button_frame
            done_button = tk.Button(button_frame, text="Done", command=self.done_action)
            #done_button.pack(expand=True, padx=10)
            done_button.pack(expand=True, ipadx=5, ipady=10)
            self.widgets_to_destroy.append(done_button)

            
    def destroy_dynamic_widgets(self):
        # Destroy all widgets tracked in the list and clear the list
        for widget in self.widgets_to_destroy:
            widget.destroy()
        self.widgets_to_destroy.clear()

    def done_action(self):
        rodent_added_changed()  
        self.start_adding_rodent.set(0)
        self.label.config(text="Treadmill and body weight support are on")
        self.destroy_dynamic_widgets()


    # Define placeholder functions for the new up and down button commands
    def adjust_position_up(self):
        # Code to handle moving up, e.g., sending a command to Arduino
        self.move_up_down_value.set(self.move_up_down_value.get()+1)

    def adjust_position_down(self):
        # Code to handle moving down, e.g., sending a command to Arduino
        self.move_up_down_value.set(self.move_up_down_value.get()-1)

    def restore_plot(self):
        # Restore the original plot view in the right half
        for widget in self.right_half_frame.winfo_children():
            widget.destroy()  # Remove existing widgets (e.g., label and button)

        # Create the plot again
        self.figure = Figure(figsize=(4, 4))
        self.plt = self.figure.add_subplot(111)
        self.canvas = FigureCanvasTkAgg(self.figure, self.right_half_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().grid(row=0, column=3, rowspan=10)
        self.plot_on = True
        self.update_plot()


    def update_plot(self):
        """Get new signal data and update plot.  Called periodically"""
        plot_labels=['Right leg Forces', 'Left leg Forces']
        matplotlib.rcParams.update({'font.size': 6})
        x, yr, yl = next(self.signal)
        self.xdata.append(x)
        self.yrdata.append(yr)
        self.yldata.append(yl)
        if len(self.xdata) > 50:
            # Throw away old signal data
            self.xdata.pop(0)
            self.yrdata.pop(0)
            self.yldata.pop(0)
        # Refresh plot with new signal data.  Clear the plot so it will rescale to the new xy data.
        self.plt.clear()
        self.plt.margins(x=0)
        
        if (self.leg_log_right_value.get() == 1 and self.leg_log_left_value.get() == 1 ):
            self.plt.plot(self.xdata, self.yrdata, 'g')
            self.plt.plot(self.xdata, self.yldata, 'm')
        elif (self.leg_log_right_value.get() == 1 and self.leg_log_left_value.get() == 0 ):
            self.plt.plot(self.xdata, self.yrdata, 'g')
            plot_labels = [plot_labels.copy().pop(0)]
        elif (self.leg_log_right_value.get() == 0 and self.leg_log_left_value.get() == 1 ):
            self.plt.plot(self.xdata, self.yldata, 'm')
            plot_labels = [plot_labels.copy().pop(1)]
        else:
            x_len = len(self.xdata)
            y_zeros = np.zeros(x_len)
            self.plt.plot(self.xdata, y_zeros)
            
        self.plt.set_title("Exerced feet forces on the force sensors")
        self.plt.set_ylabel("Force [in N]")
        self.plt.set_xlabel("Time [in s]")
        self.plt.legend(loc="upper left", labels=plot_labels)
        self.figure.canvas.draw()
        
        #Communication(self)

if __name__ == "__main__":
    app = App()
    GuiUserFrame(app)
    app.mainloop()
    #x = threading.Thread(target=Communication, args=(1,))
    #x.start()
