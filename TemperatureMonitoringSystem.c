/*

    Program: HSI255 Lab Project
    Author: Frances Resma
    Date: December 6, 2024
    Purpose: This program interfaces with a temperature sensor, buzzer, and LED.
	The PWM is set up in the beginning of the code with setting FIO4 as the 
	channel to send the PWM signal to, with divisor as 48MHz/DIV, with divisor of 47, 
	1 as number of time enabled, and 8-bit timer mode. The program starts off with LED 
	off by DAC0 sending a high signal. Then, the user is asked for the temperature threshold 
	in degrees Celsius. the user is asked for the temperature threshold in degrees Celsius. 
	Then, they are asked to choose between two exit strategies; how long they want the program 
	to run for (in seconds) or how many buzzer activations before the program ends. The temperature 
	sensor (AIN0) will read the voltage output continuously compare it with the threshold set 
	by the user to decide whether to activate the LED or the buzzer. If the user chooses to run the 
	program based on the time they provide, and if the temperature is below the threshold, the 
	DAC0 channel which is connected to the LED will send a low signal for the LED to remain on and 
	the program constantly prints the current temperature. If the temperature is above the threshold, the timer 
	value is set to 50% duty cycle to turn the buzzer on and DAC0 will send a high signal to turn the LED off. 
	The program will constantly print that the temperature is high and the current temperature. 
	If the user chooses to end the program after a certain amount of buzzer activation, again, the program will 
	constantly print the current temperature and turn the LED on if the temperature is below the threshold. 
	If the temperature is above the threshold, the timer value will be set to 50% duty cycle to turn the buzzer on. 
	The program will stop printing the current temperature and lets the user know how many times the buzzer has been activated, 
	compared to the number of buzzer activations they specified. The buzzer will turn off and 
	the program will continue printing the current temperature again and turns the LED on again. 
	This loop will go on until the number of buzzer activations the user specified has been reached. 
	The user also has the option to exit the program in the beginning of the program if they choose not to continue. 
	Before the program ends, the LED and buzzer turn off and all pins will reset. The program will wait for user input
	to exit the program.

*/


#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "C:/Program Files (x86)/LabJack/Drivers/LabJackUD.h"


int main()
{
	
	LJ_ERROR lj_cue;
	LJ_HANDLE lj_handle = 0;

	// Variable declerations
	int counter = 0, sec = 0, activation = 0, buzzerTriggered = 0;
	double tsAIN0;
	double threshold = 0;
	int menuOpt = 1;

	// Open LabJack 
	lj_cue = OpenLabJack(LJ_dtU3, LJ_ctUSB, "1", 1, &lj_handle);
	lj_cue = ePut(lj_handle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);

	// Setup PWM 
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0, 0); // FIO4
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_tc48MHZ_DIV, 0, 0); // 48MHz with DIV
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, 47, 0, 0); // Divisor of 47
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 1, 0, 0); // Enable the timer
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_MODE, 0, LJ_tmPWM8, 0, 0); // Timer mode 8-bit
	lj_cue = Go();

	// Initially, LED is off so DAC0 = 5V
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 5, 0, 0);
	lj_cue = Go();

	
	// Welcome message and instructions
	printf("\t\t\t***** WELCOME *****\n\n");
	printf("This program monitors the temperature using a temperature sensor.\n");
	printf("The buzzer will activate if the temperature exceeds the threshold\n");
	printf("that you will set before the program starts! The blue LED will be\n");
	printf("ON when the temperature is BELOW the threshold (meaning it is cold!).\n");
	printf("The LED will turn off when the temperature is ABOVE the threshold\n");
	printf("(meaning it is hot!), and the buzzer will sound as an alert!\n\n");
	printf("\nYou will have two options to chooose how you want the program to end:\n");
	printf("You can choose how many seconds you want to run the program for, OR\n");
	printf("you can choose how many times the buzzer should activate before the program ends.\n");
	printf("\n\t\t\tLet's begin!\n\n");
	system("pause");	// Wait for user input to continue
	printf("\nPlease enter the threshold temperature in degrees Celsius for the buzzer to sound: ");
	scanf_s("%lf", &threshold);		// Where user enters the temperature value 
	
	while (menuOpt != 0)
	{
		// Exit strategy menu
		printf("\n\nExit strategy:");
		printf("\n\t1. Choose how long you want the program to run for.");
		printf("\n\t2. Choose the amount of buzzer activations you want to occur.");
		printf("\n\t3. Exit\n\n");
		printf("Enter option: ");
		scanf_s("%d", &menuOpt);	// Where user enters their option


		// If user chooses strategy 1
		if (menuOpt == 1)
		{
			printf("\n\nType how many seconds you want the program to run for: ");
			scanf_s("%d", &sec);	// User enters the amount of seconds

			// The program will run depending on how many seconds the user puts
			while (counter < sec * 5)
			{
				// Read AIN0 value (temperature)
				lj_cue = AddRequest(lj_handle, LJ_ioGET_AIN, 0, 0, 0, 0);
				lj_cue = Go();
				lj_cue = GetResult(lj_handle, LJ_ioGET_AIN, 0, &tsAIN0);

				// If AIN0 value is below threshold, turn on LED and buzzer is off,  multiplied by 100 to convert the voltage to degrees Celsius
				if (tsAIN0 * 100 < threshold)	
				{
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 0, 0, 0);	// Turn LED on by DAC0 sending low signal 
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 0, 0, 0);	// Turns off buzzer 
					lj_cue = Go();

					// Prints AIN0 value, multiplied by 100 to convert the voltage to degrees Celsius
					printf("The current temperature is %.2lf\n", tsAIN0 * 100);		
				}

				// If AIN0 value is above threshold, turn off LED and buzzer is on,  multiplied by 100 to convert the voltage to degrees Celsius
				else if (tsAIN0 * 100 > threshold)	
				{
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 5, 0, 0);	// Turns off LED by DAC0 sending high signal
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 32768, 0, 0);	// Turns on buzzer 
					lj_cue = Go();

					// Tells user the temperature is above the set threshold and prints AIN0 value, multiplied by 100 to convert the voltage to degrees Celsius
					printf("The temperature is high! Temperature: %.2lf\n", tsAIN0 * 100);	
				}

				counter++;
				Sleep(200);
			}

			// When done, break out of the loop and program ends
			break;
		}

		// If user choose 2
		else if (menuOpt == 2)
		{
			printf("\n\nHow many buzzer activations would you like to occur? ");
			scanf_s("%d", &activation);		// User enters how many buzzer activations and store in the activation value

			// Loop will end when buzzer activates depending on the number the user puts 
			while (counter < activation)
			{
				// Read AIN0 value (temperature)
				lj_cue = AddRequest(lj_handle, LJ_ioGET_AIN, 0, 0, 0, 0);
				lj_cue = Go();
				lj_cue = GetResult(lj_handle, LJ_ioGET_AIN, 0, &tsAIN0);

				// If temperature is above threshold and buzzer has not been triggered
				if (tsAIN0 * 100 > threshold && buzzerTriggered != 1)
				{
					counter++;	// Increase counter
					buzzerTriggered = 1;	// Mark buzzer as triggered

					// Prints how many times the buzzer has been activated 
					printf("The buzzer rang %d/%d times!\n", counter, activation);

					// Turns off LED by DAC0 sending high signal and set valur yo 32768 to turn on buzzer
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 5, 0, 0);
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 32768, 0, 0);
					lj_cue = Go();
				}

				// If temperature is below threshold, 
				else if (tsAIN0 * 100 < threshold)
				{
					buzzerTriggered = 0;	// Reset sticky key 

					// Turn on LED by DAC sending low signal and set timer value to 0 to turn off buzzer
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 0, 0, 0);
					lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 0, 0, 0);
					lj_cue = Go();

					// Keep printing the current temperature 
					printf("The current temperature is %.2lf\n", tsAIN0 * 100);
				}
				
				Sleep(200);
			}

			// When done, break out of the loop and program ends
			break;
		}

		// If user chose 3 (exit), break out of the loop and program ends
		else if (menuOpt == 3)
		{
			break;
		}

		break;
	}

	// Turn off LED and buzzer and reset all pins
	lj_cue = ePut(lj_handle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 5, 0, 0);
	lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 0, 0, 0);
	lj_cue = Go();

	printf("\n\nThank you for using my HSI Project program!\n");
	printf("Press any key to exit..\n\n");	// Wait for user input before terminating the program
	system("pause");

	// Close LabJack
	Close();	
	return 0;

}
