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


void onButtonA(MicroBitEvent)
{
    // Send 'A' when button A is pressed and print '1'
    uBit.radio.datagram.send("A");   
    uBit.display.print("1"); 
}

void onButtonB(MicroBitEvent)
{
    // Send 'B' when button B is pressed and print '2'
    uBit.radio.datagram.send("B");  
    uBit.display.print("2");   
}


void onButtonAB(MicroBitEvent)
{
	//Send '3' label when both buttons are pressed
    uBit.radio.datagram.send("3");
    uBit.display.print(three);
}

void onData(MicroBitEvent)
{
    ManagedString s = uBit.radio.datagram.recv();
    int rssi = uBit.radio.getRSSI();
    
    if (s == "1")
    	// Display one of the animation arrays when '1' was received
        uBit.display.print(threeE);

    if (s == "2")
    	// Display one of the animation arrays when '2' was received
        uBit.display.print(twoE);
    //Do nothing when 'balanced' or 'tilted' or group name has been detected
    else if(s=="balanced")
    {}
    else if(s=="tilted")
    {}
    else if(s==group_name)
    {}
    else{
    	uBit.serial.send(s);
    	uBit.serial.send("\n\r");
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

    //Setup a hander to run when data is received
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);

	// Initialize radio
    uBit.radio.enable();
    
    //Initialize the counter
    int count = 0;
    

    while(1) {
        
        // If other board has been detected
        if (friend_seen) {
	    uBit.display.print(one); 
            friend_seen -= 1;
        } else {
            uBit.display.print(two);
        }
        // If we broadcasting
        if (broadcast)
           uBit.radio.datagram.send(group_name);
           
        //Print the animation
        if(count%1){
			uBit.display.print(one); 
		}
		if(count%2){
			uBit.display.print(twoE); 
		}
		if(count%3){
			uBit.display.print(threeE); 
		}
           
	    //delay 300ms
		uBit.sleep(300);
		//increment the counter
		count++;
    }
}
