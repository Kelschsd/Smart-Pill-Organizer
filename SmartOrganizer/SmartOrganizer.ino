
/*
	Smart pill organizer project for ECE 484
	Created by: Allison Jacob, Samuel Kelsch, Kieran Thompson
*/



#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

/*
	Protoype Functions
*/
void notify();	  	// Main function that implements all notifications
void get_time();  	// Reads time from RTC and stores readings to global varriables
void print_time();	// Prints a time to LCD
void light_LED(); 	// Writes LED address to registers to light, will keep LEDs on if not answered
void send_SMS();  	// Send SMS from notify()
void get_key();	  	// Read value on analog pin and translate to what button pushed
void set_time();  	// Used to set time
void set_alarm(); 	// Used to set alarm time

/*
	Project varriables
*/

bool alarms_on = 1;	// Change to 0 to disable alarms

struct alarms
{
	int weekDay[13];  // (0-6) (Sun - Sat)
	int hour[13];     // (0-23)
	int min[13];      // (0-59)
	bool alarm_on[13];    // (0-1)
};

alarms alarm = {};

/*
	Setup method to initialize components
*/

void setup(void) {
  Serial.begin(9600);
  Serial.println("Setup Start");
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  Serial.println("Setup END");
}

/*
  Method to check if current time is the same as an alarm time that is turned ON
*/
bool isTimeAlarm(DateTime now) {
  for (int i=0; i<14; i++) {
    if (alarm.alarm_on[i] == 1) { // only check time against set alarm time if alarm is ON (1)
     if (alarm.weekDay[i] == now.dayOfWeek()) { 
      if (alarm.hour[i] == now.hour()) { 
        if (alarm.min[i] == now.minute()) { 
          return true;
        }
       }
      }
    }
  }
  return false;
}

/*
  Function to print time to serial
*/
void printTime () {
    DateTime now = RTC.now(); 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(' ');
    Serial.print(now.dayOfWeek(), DEC);
    Serial.println(); 
}

/*
	Main method that loops forever
*/
void loop() {
/* Test Code  
  Serial.println("DEBUG 1");
  DateTime now = RTC.now(); 
  Serial.println("DEBUG 2");
  alarm.alarm_on[0] = 1;  
  alarm.weekDay[1] = 1;
  alarm.hour[1] = 1;
  alarm.min[36] = 1;
  Serial.println("DEBUG 3");
  */
  Serial.println("Is alarm time?: "+ isTimeAlarm(now));
  printTime();
  if (isTimeAlarm(now) == true){
    // notify();      
  }
  // If time == alarm && alarm_on
    //notify();
  // if keypad_flag == 1
    // if key == "set alarm key"
      // set_alarm();
    // if key == "set time key"
      // set_time();
    // keypad_flag = 0;
}