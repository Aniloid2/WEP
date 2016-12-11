/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
#include "MicroBit.h"
#include <stdio.h>

#if MICROBIT_BLE_ENABLED 
 #ifdef YOTTA_CFG
  #error "This example needs BLE to be disabled. Use the yotta config.json in the proximit-heart directory to do this"
 #else
  #error "This example needs BLE to be disabled in the microbit config file in the microbit-dal: MicroBitConfig.h"
 #endif
#endif

// Initialize the board
MicroBit    uBit;

// Haven't seen any one before
uint8_t friend_seen = 0;

// Initialize broadcasting to 1
uint8_t broadcast = 1;

// If any micro:bit is in range and has the same group name others will see it.
const char group_name[] = "shit";
 
// Initialize global variables 
int begin=1;
int to_base =0;
int toggle = 0;
int button = 0;
int served = 0;
int prev_button = 0;
int vib2 = 1;


// Define arrays for a simple animation
const uint8_t one_arr[] {
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0,
                          0, 0, 1, 0, 0,
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, };

const uint8_t two_arr[] {
                          0, 0, 0, 0, 0,
                          0, 1, 1, 1, 0,
                          0, 1, 1, 1, 0,
                          0, 1, 1, 1, 0,
                          0, 0, 0, 0, 0, }; 

const uint8_t three_arr[] {
                          1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, };

const uint8_t twoE_arr[] {
                          0, 0, 0, 0, 0,
                          0, 1, 1, 1, 0,
                          0, 1, 0, 1, 0,
                          0, 1, 1, 1, 0,
                          0, 0, 0, 0, 0, }; 

const uint8_t threeE_arr[] {
                          1, 1, 1, 1, 1,
                          1, 0, 0, 0, 1,
                          1, 0, 0, 0, 1,
                          1, 0, 0, 0, 1,
                          1, 1, 1, 1, 1, };

MicroBitImage one(5,5,one_arr);
MicroBitImage two(5,5,two_arr);
MicroBitImage three(5,5,three_arr);
MicroBitImage twoE(5,5,twoE_arr);
MicroBitImage threeE(5,5,threeE_arr);


MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ANALOG);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_DIGITAL);
MicroBitPin P16(MICROBIT_ID_IO_P16, MICROBIT_PIN_P16, PIN_CAPABILITY_DIGITAL);

void onButtonA(MicroBitEvent)
{
    // Send '1' through radio when button A is pressed
    uBit.radio.datagram.send("1");    
}

void onButtonB(MicroBitEvent)
{
    // Send '2' through radio when button A is pressed
    uBit.radio.datagram.send("2");    
}


void onButtonAB(MicroBitEvent)
{
    // Toggle broadcasting if both buttons are pressed together 
    broadcast = !broadcast;
    uBit.display.print("!");
}


void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();
    int rssi = uBit.radio.getRSSI();

    if (s == "1"){
        // Display one of the animation arrays when '1' was received
        uBit.display.print(threeE);
	}
    if (s == "2"){
        // Display one of the animation arrays when '2' was received
        uBit.display.print(twoE);
    }
    if(s == "3"){
    	// Toggle begin mode and turn off to_the_base mode
    	begin ^= 1;
    	to_base = 0;
    }
    else if(s== "A")
    {
    	to_base ^= 1;
    	begin = 0;
    	served = 0;
    	P1.setDigitalValue(0);
    }
    else if(s=="tilted"){
        // Turn on the vibration generator when 'tilted' label was received and the vibration mode is ON
        if(vib2)
        	P1.setDigitalValue(1);
        else
        	P1.setDigitalValue(0);
    }
    else if(s=="balanced"){
    	// Turn off the vibration generator when 'balanced' label was received
    	P1.setDigitalValue(0);
    }
    	
	// In order to detect another board it has to send the same group name
    if (s == group_name && rssi < 70) {
        // We can make this larger to allow more missed packets
        friend_seen = 3;
    }
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Setup some button handlers to allow direct heartbeat control with buttons
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, onButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_EVT_ANY, onButtonAB);

    // Setup a hander to run when data is received
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
	
	// Initialize radio
    uBit.radio.enable();
    
	// Initialize local variables
    int value;
    char str[100],str_serv[10];
  	int avg = 0;
  	int count = 0;
  	double pitch, roll;
  	double accelx = 0, accely = 0, accelx_avg, accely_avg, accelx_old, accely_old;
  	int gesture = 5, vib=0;
  	double velx = 0;
	double vely = 0;
	double a = 0.5, dt = 0.8, g=9.87*0.001;
	
   // Print everything upside down
	uBit.display.rotateTo(MICROBIT_DISPLAY_ROTATION_180); 

    while(1) {
        // If other board has been detected
        if (friend_seen) {
	        sprintf(str_serv,"%d",served);
            uBit.display.print(str_serv); 
            friend_seen -= 1;
        } 
        else {
            uBit.display.print(one);
        }
        // If we broadcasting
        if (broadcast)
           uBit.radio.datagram.send(group_name);
           
        // Avoid debouncing
        prev_button = button;
        button ^= P16.getDigitalValue();

        // If external button pressed, increment the number of customers served
        if(button&&!prev_button){
        	uBit.display.print(one);
        	served ++;
        	}
		// Print the animation (very badly implemented)
		if(count%1&&(begin ||to_base)){
			uBit.display.print(one); 
		}
		if(count%2&&(begin||to_base)){
			uBit.display.print(two); 
		}
		if(count%3&&(begin||to_base)){
			uBit.display.print(three); 
		}
		// Toggle the vibration generator when to_the_base mode is ON
		if(to_base&&!begin&&(count%100))
		{	
			toggle^=1;
			P1.setDigitalValue(toggle);
		}
		
		if(!begin)
		{
			//Read the value from the heart rate sensor (ADC) and accelometer
	        value = P0.getAnalogValue(); // P0 is a value in the range of 0 - 1024
	        pitch = uBit.accelerometer.getPitchRadians();
	        roll = uBit.accelerometer.getRollRadians();
	        accelx_old = accelx;
	        accely_old = accely;
	        accelx = (a*uBit.accelerometer.getX() + (1-a)*accelx_old)*g; //attempt to implement complementary filter
	        accely = (a*uBit.accelerometer.getY() + (1-a)*accely_old)*g;
	        velx += accelx*dt;
	        vely += accely*dt;
         	
         	// Detect gestures
	        gesture = uBit.accelerometer.getGesture();
	        if(gesture==4)
	        {
	        	vib^=1;
	        	if(vib==1)
	        		uBit.display.print("N"); 
	        	else
	        		uBit.display.print("F"); 
	        }
	        if(gesture==3)
	        {
	        	vib2^=1;
	        	if(vib2==1)
	        		uBit.display.print("N"); 
	        	else
	        		uBit.display.print("F"); 
	        }
	        //UART for debugging and data from the accelerometer send to other boards
	        sprintf(str,"1,%.2f,%.2f,%.2f,%.2f,%d", pitch , roll,accelx_old,accely_old,served);
	        uBit.serial.send(str);
	        uBit.serial.send("\n\r");
	        uBit.radio.datagram.send(str);
	        
	        if(pitch>0.2 ||pitch <-0.2||roll>0.2 ||roll <-0.2){
	        	// Turn on the vibration generator if vibration mode is ON
	        	if(vib)
	        		P1.setDigitalValue(1);
	        	// Send the label that the board is 'tilted' to other mirco:bits
	        	uBit.radio.datagram.send("tilted");   
	        }
	        else{
	          	if(vib)
	        		P1.setDigitalValue(0);
	        	// Send the label that the board is 'balaced' to other mirco:bits
	        	uBit.radio.datagram.send("balanced"); 
	        }
	    }
	    //delay 300ms
		uBit.sleep(300);
		//increment the counter
		count++;
    }
}
