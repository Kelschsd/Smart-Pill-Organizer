/*
  Smart pill organizer project for ECE 484
  Created by: Allison Jacob, Samuel Kelsch, Kieran Thompson
*/


#include <NoDelay.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "src/libs/RTClib.h"
RTC_DS1307 RTC;
#include <ezAnalogKeypad.h>

/*
  Project varriables
*/

unsigned char left = '<';;
unsigned char right = '>';
unsigned char zero = '0' ;
unsigned char ent = '=';
unsigned char seven = '7';
unsigned char  eight = '8';
unsigned char nine = '9';
unsigned char esc = 'x';
unsigned char four = '4';
unsigned char five = '5';
unsigned char six = '6';
unsigned char down = '.';
unsigned char one = '1';
unsigned char two = '2';
unsigned char three = '3';
unsigned char up = '^';
unsigned char F1 = 'f';
unsigned char F2 = 'f';
unsigned char sharp = '#';
unsigned char asterisk = '*';


ezAnalogKeypad keypad(A0);
bool alarms_on = 1; // Change to 0 to disable alarms
bool hasNotification = false;
// Create an LCD object. Parameters: (RS, E, Data Lines)
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);

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

int melody[] = {1000, 1500, 2000, 2500, 3000, 2000, 1000}; // An array of frequencies


/*
  Setup method to initialize components
*/
void setup(void) {
  Serial.begin(9600);
  Serial.println("Setup Start");
  lcd.begin(16, 2);               // Initializes LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(stClk, OUTPUT);
  pinMode(srClk, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(A3, OUTPUT);
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
keypad.setNoPressValue(0);
keypad.registerKey(left,962);
keypad.registerKey(right,962);
keypad.registerKey(zero,963);
keypad.registerKey(ent,965);
keypad.registerKey(seven,688);
keypad.registerKey(eight,724);
keypad.registerKey(nine,693);
keypad.registerKey(esc,815);
keypad.registerKey(four,532);
keypad.registerKey(five,608);
keypad.registerKey(six,545);
keypad.registerKey(down,768);
keypad.registerKey(one,195);
keypad.registerKey(two,394);
keypad.registerKey(three,226);
keypad.registerKey(up,704);
keypad.registerKey(F1,1005);
keypad.registerKey(F2,1005);
keypad.registerKey(sharp,1005);
keypad.registerKey(asterisk,1005);



  /////////////////////////////////////////////////////
  //Test Times
  set_alarm(3, 3, 18, 19);     // Alarm, weekday, hour, min
  //set_DateTime(1, 12, 25, 12, 59, 57); //year, month, day, hour, min, sec
  //////////////////////////////////////////////////////
}


/*
  Method to check if current time is the same as an alarm time that is also turned ON
*/
int isTimeAlarm() {
  DateTime now = RTC.now();
  for (int i = 0; i < 14; i++) {
    if (alarm_on[i] == 1) { // only check time against set alarm time if alarm is ON (1)
      if ((weekDay[i]) == (now.dayOfWeek())) { // check if day of week, hour, and min = an alarm time
        if (hour[i] == now.hour()) {
          if (min[i] == now.minute()) {
            return i + 1;
          }
        }
      }
    }
  }
  return 0;
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
   Function to determine which pill compartment to
   light up
*/
int lightBox(int alarmTriggered) {
  int box = 0;
  if (hour[alarmTriggered] < 12) {
    box = (1 << (weekDay[alarmTriggered] * 2)); /// AM boxes
  } else if (hour[alarmTriggered] >= 12) {
    box = (2 << (weekDay[alarmTriggered] * 2)); /// PM boxes
  }
  return box;
}

/*
  Function to alert user an alarm is triggered
*/
void notify(int alarmTriggered) {
  send_SMS();
  LED = lightBox(alarmTriggered);
  if (LED < 256) {                                      // Uses first register
    digitalWrite(stClk, LOW);                           // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, LED);
    digitalWrite(stClk, HIGH);                          // Ends communications with register
  }
  else if (LED >= 256) {                                // Uses second register
    digitalWrite(stClk, LOW);                           // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, 0);
    shiftOut(dataPin2, srClk, LSBFIRST, (LED >> 8));    // Shifts data by 8 bits to other register
    digitalWrite(stClk, HIGH);                          // Ends communications with register
  }

  // Sounds alarm
  for (int thisNote = 0; thisNote < (sizeof(melody) / sizeof(melody[0])) ; thisNote++) { // Loops for each note in melody[]
    tone(13, melody[thisNote], 500);    // Plays the note at the position in melody[] for 500ms
    delay(500);                         // Waits for 500ms for the note to finish
  }
  alarmTriggered = 0;   // Resets alarm status
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
void send_SMS() {
  digitalWrite(A3,HIGH);
  delay(200);
  digitalWrite(A3,LOW);
}

/*
   Prints the time to the LCD
*/
void dispTime() {
  delay(50);
  DateTime now = RTC.now();       // Gets current time
  lcd.setCursor(0, 0);
  sprintf(output, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.print(output);
  lcd.setCursor(0, 1);
  sprintf(output, "%02d/%02d/%02d day:%01d", now.month(), now.day(), now.year(), now.dayOfWeek());
  lcd.print(output);
}

/*
  Main method that loops forever
*/
void loop() {
  DateTime now = RTC.now();
 printTime();
 dispTime();
 unsigned char key = keypad.getKey();
 if (key && (key != 42) && (hasNotification == 0)) {
    Serial.println(key);
    delay(1000);
    keyMenu();
 } else if ( key && hasNotification == 1) {
   hasNotification = 0;                               // Clears Notification LED
   LED = 0;
   digitalWrite(stClk, LOW);                           // Sets register to recieve data
   shiftOut(dataPin, srClk, LSBFIRST, LED);
   digitalWrite(stClk, HIGH);                          // Ends communications with register
 }
 
  int alarmTriggered = isTimeAlarm();
  Serial.print(alarmTriggered);
  if ((alarmTriggered != 0) && (alarms_on == 1) && (now.second() == 0)) { // Alarm only triggers if: current time = time of an alarm;
    hasNotification = 1;
    // alarms are turned on; it is the turn of a minute
    notify(alarmTriggered - 1); // Changes alarm number back to array positions
  } else {
    delay(300);
 }
}

char keyboardNum(char keyVal) {

  // Map the analog value to the corresponding key
  
  if (keyVal == 962) {
    keyVal = '<';
  } else if (keyVal == 61) {
    keyVal = '0';
  } else if (keyVal == 965) {
    keyVal = '=';
  } else if (keyVal == 57) {
    keyVal = '7';
  } else if (keyVal == 56) {
    keyVal = '8';
  } else if (keyVal == 94) {
    keyVal = '9';
  } else if (keyVal == 815) {
    keyVal = '-';
  } else if (keyVal == 54) {
    keyVal = '4';
  } else if (keyVal == 53) {
    keyVal = '5';
  } else if (keyVal == 55) {
    keyVal = '6';
  } else if (keyVal == 768) {
    keyVal = '!';
  } else if (keyVal == 49) {
    keyVal = '1';
  }  else if (keyVal == 50) {
    keyVal = '2';
  }  else if (keyVal == 51) {
    keyVal = '3';
  } else if (keyVal == 704) {
    keyVal = '^';
  } else if (keyVal == 1005) {
    keyVal = '#';
  } 
  /*else {
    keyVal = 'x';  // No key pressed
  }
*/
  return keyVal;
}

void keyMenu () {
  lcd.clear();
  lcd.setCursor(0, 0);  // Displays secondary message
  lcd.print("Set Alarm: 01-14");
  //delay(2000);
  unsigned char alarmKey1 = keypad.getKey();
  while((alarmKey1 == 0) || (alarmKey1 == 42)){
    alarmKey1 = keypad.getKey();
  }
  char alarmNum1 = keyboardNum(alarmKey1);
  lcd.setCursor(0, 1);  // Displays secondary message
  lcd.print(alarmNum1);
  delay(2000);
  unsigned char alarmKey2 = keypad.getKey();
  while((alarmKey2 == 0) || (alarmKey2 == 42)){
    alarmKey2 = keypad.getKey();
  }
  char alarmNum2 = keyboardNum(alarmKey2);
  lcd.setCursor(1, 1);  // Displays secondary message
  lcd.print(alarmNum2);
  delay(2000);
  int alarmNum = (alarmNum1 - '0') * 10 + (alarmNum2 - '0');
  Serial.println(alarmNum);
  lcd.clear();
  lcd.setCursor(0, 0);  // Displays secondary message
  lcd.print("Day: S:0 M:1 T:2");
  lcd.setCursor(0, 1);  // Displays secondary message
  lcd.print("W:3 T:4  F:5 S:6");
  delay(3000);
  unsigned char dayKey = keypad.getKey();
  while((dayKey == 0) || (dayKey == 42)){
    dayKey = keypad.getKey();
  }
  char Day = keyboardNum(dayKey);
  int newDay = (Day - '0');
  Serial.println(newDay);
  lcd.clear();
  lcd.setCursor(0, 0);  // Displays secondary message
  lcd.print("Day:");
  if (Day == '0') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Sunday");
    delay(2000);
  }
  else if (Day == '1') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Monday");
    delay(2000);
  }
  else if (Day == '2') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Tuesday");
    delay(2000);
  }
  else if (Day == '3') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Wednesday");
    delay(2000);
  }
  else if (Day == '4') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Thursday");
    delay(2000);
  }
  else if (Day == '5') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Friday");
    delay(2000);
  }
  else if (Day == '6') {
    lcd.setCursor(0, 1);  // Displays secondary message
    lcd.print("Saturday");
    delay(2000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Time");
  lcd.setCursor(2, 1);
  lcd.print(":");
  delay(2000);

  unsigned char time1key = keypad.getKey();
  while((time1key == 0) || (time1key == 42)){
    time1key = keypad.getKey();
  }
  char time1 = keyboardNum(time1key);
  lcd.setCursor(0, 1);
  lcd.print(time1);
  delay(2000);

  unsigned char time2key = keypad.getKey();
  while((time2key == 0) || (time2key == 42)){
    time2key = keypad.getKey();
  }
  char time2 = keyboardNum(time2key);
  lcd.setCursor(1, 1);
  lcd.print(time2);
  delay(2000);
  int newHour = (time1 - '0') *10 +(time2 - '0'); 
  Serial.println(newHour);
  unsigned char time3key = keypad.getKey();
  while((time3key == 0) || (time3key == 42)){
    time3key = keypad.getKey();
  }
  char time3 = keyboardNum(time3key);
  lcd.setCursor(3, 1);
  lcd.print(time3);

  delay(2000);
  unsigned char time4key = keypad.getKey();
  while((time4key == 0) || (time4key == 42)){
    time4key = keypad.getKey();
  }
  char time4 = keyboardNum(time4key);
  lcd.setCursor(4, 1);
  lcd.print(time4);
  int newMinute = (time3 - '0') *10 +(time4 - '0');
  delay(1000);
Serial.println(newMinute);
  set_alarm(alarmNum, newDay, newHour, newMinute);




  lcd.clear();

  loop();

}








