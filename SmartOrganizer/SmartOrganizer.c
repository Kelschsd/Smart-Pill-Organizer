/*
	Smart pill organizer project for ECE 484


	Created by: Allison Jacob, Samuel Kelsch, Kieran Thompson
*/


#include "libs/"		//Designates file to store libraries used

/*
	Protoype Functions
*/
void notify();		// Main function that implements all notifications
void get_time();	// Reads time from RTC and stores readings to global varriables
void print_time();	// Prints a time to LCD
void light_LED();	// Writes LED address to registers to light, will keep LEDs on if not answered
void send_SMS();	// Send SMS from notify()
void get_key();		// Read value on analog pin and translate to what button pushed
void set_time();	// Used to set time
void set_alarm();	// Used to set alarm time

/*
	Project varriables
*/

bool alarm_on = 1;	// Change to 0 to disable alarms

typedef struct
{
	uint8_t weekDay[13];
	uint8_t hour[13];
	uint8_t min[13];
	bool alarm_on[13];
}alarms;

typedef struct		// Structure to store time
{
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t weekDay;
  uint8_t date;
  uint8_t month;
  uint8_t year;  
}rtc_t;



/*
	Setup method to initialize components
*/

void setup(void) {
  
}


/*
	Main method that loops forever
*/
int main() {
	setup();	// Calls setup
	while(1){
		
		// get_tim();
		// Check if time == alarm && alarm_on
		// If time == alarm && alarm_on
			//notify();
		// if keypad_flag == 1
			// if key == "set alarm key"
				// set_alarm();
			// if key == "set time key"
				// set_time();
			// keypad_flag = 0;
			
		
	}
	return 0;
}

notify