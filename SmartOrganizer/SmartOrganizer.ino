
/*
	Smart pill organizer project for ECE 484
	Created by: Allison Jacob, Samuel Kelsch, Kieran Thompson
*/


#include <NoDelay.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "src/libs/RTClib.h"
RTC_DS1307 RTC;

/*
	Project varriables
*/
bool alarms_on = 1;	// Change to 0 to disable alarms
// Create an LCD object. Parameters: (RS, E, Data Lines)
LiquidCrystal lcd = LiquidCrystal(2,3,4,5,6,7);

int stClk = 8;  // RCLK (12) primary register
int srClk = 12;  // SRCLK (11) primary register
int dataPin = 11;  // SER (14) primary register
int dataPin2 = 10;  // SER (14) secondary register

///////////////////////////////////////////////////Remove PB?
int button1 = 9;  // push button
int button1State = 0;
////////////////////////////////////////////////////

char output[100];
int LED = 0;  // Which LED should be illuminated

// List of all alarms and their values. 14 total possible alarms, where the index
// of the value in the array represents the alarm number.
uint8_t weekDay[14];    // (0-6) (Sun - Sat)
uint8_t hour[14];       // (0-23)
uint8_t min[14];        // (0-59)
bool alarm_on[14];      // (0-1)

int melody[] = {1000,1500,2000,2500,3000,2000,1000};  // An array of frequencies


/*
	Setup method to initialize components
*/
void setup(void) {
  Serial.begin(9600);
  Serial.println("Setup Start");
  lcd.begin(16,2);                // Initializes LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(stClk, OUTPUT);
  pinMode(srClk, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(button1, INPUT);
  digitalWrite(stClk, LOW);  // Sets register to recieve data
  shiftOut(dataPin, srClk, LSBFIRST, 0);
  shiftOut(dataPin2, srClk, LSBFIRST, 0);
  digitalWrite(stClk, HIGH);  // Ends communications with register
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  RTC.adjust(DateTime(__DATE__, __TIME__));
  Serial.println("Setup END");

  /////////////////////////////////////////////////////
  //Test Times
  set_alarm(0, 1, 0, 0);
  set_DateTime(0, 12, 24, 23, 59, 57);
  //////////////////////////////////////////////////////
}


/*
  Method to check if current time is the same as an alarm time that is also turned ON
*/
bool isTimeAlarm() {
  DateTime now = RTC.now(); 
  for (int i=0; i<14; i++) {
    if (alarm_on[i] == 1) { // only check time against set alarm time if alarm is ON (1)
     if ((weekDay[i]) == (now.dayOfWeek())) { // check if day of week, hour, and min = an alarm time
      if (hour[i] == now.hour()) { 
        if (min[i] == now.minute()) { 
          return true;
        }
       }
      }
    }
  }
  return false;
}


/*
  Function to print date, time, and day of the week to serial
*/
void printTime () {
    DateTime now = RTC.now();       // Gets current time
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
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
  Function to alert user an alarm is triggered
*/
void notify(){
  send_SMS();

  if (LED < 256){                                       // Uses first register
    digitalWrite(stClk, LOW);                           // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, LED);
    digitalWrite(stClk, HIGH);                          // Ends communications with register
  }
  else if(LED >= 256){                                  // Uses second register
    digitalWrite(stClk, LOW);                           // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, 0);
    shiftOut(dataPin2, srClk, LSBFIRST, (LED >> 8));    // Shifts data by 8 bits to other register
    digitalWrite(stClk, HIGH);                          // Ends communications with register
  }

  // Sounds alarm
  for (int thisNote = 0; thisNote < (sizeof(melody) / sizeof(melody[0])) ; thisNote++){  // Loops for each note in melody[]
      tone(13, melody[thisNote], 500);    // Plays the note at the position in melody[] for 500ms
      delay(500);                         // Waits for 500ms for the note to finish
    }
}


/*
  Function to take user input and set time of RTC
*/
void set_DateTime(int newYear, int newMon, int newDay, int newHour, int newMin, int newSec) {
  RTC.adjust(DateTime(newYear, newMon, newDay , newHour, newMin, newSec));
}


/*
  Function to take user input and set time of an alarm
*/
void set_alarm(int alarmNum, int newWeekDay, int newHour, int newMin) {
  alarm_on[alarmNum] = 1;  
  weekDay[alarmNum] = newWeekDay;
  hour[alarmNum] = newHour;
  min[alarmNum] = newMin;
}

/*
  Function to send the user an SMS message
*/
void send_SMS(){



}


/*
	Main method that loops forever
*/
void loop() { 
  DateTime now = RTC.now(); 
  printTime();

  if ((isTimeAlarm() == true) && (alarms_on == 1) && (now.second() == 0)){  // Alarm only triggers if: current time = time of an alarm; 
                                                                            // alarms are turned on; it is the turn of a minute
    notify();     
  } else {
    delay(300);
  }
  // if keypad_flag == 1
    // if key == "set alarm key"
      // set_alarm();
    // if key == "set time key"
      // set_time();
    // keypad_flag = 0;
}