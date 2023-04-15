#include<NoDelay.h>
#include<LiquidCrystal.h>

// Create an LCD object. Parameters: (RS, E, Data Lines)
LiquidCrystal lcd = LiquidCrystal(2,3, 4,5,6,7);

int stClk = 8;  // RCLK (12) primary register
int srClk = 12;  // SRCLK (11) primary register
int dataPin = 11;  // SER (14) primary register
int dataPin2 = 10;  // SER (14) secondary register
int button1 = 9;  // push button
int button1State = 0;

char output[100];
int LED = 0;  // Which LED should be illuminated
bool hasNotification = 0;

int alarm1 = 9;  // Hour of time to give notification
int alarm2 = 18;

void setup(){
  Serial.begin(9600);
  // Parameters: (cols, rows)
  lcd.begin(16,2);
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
  delay(500);
}

// Time conversions
unsigned long day = 86400000; // 86400000 milliseconds in a day
unsigned long hour = 3600000; // 3600000 milliseconds in an hour
unsigned long minute = 60000; // 60000 milliseconds in a minute
unsigned long second =  1000; // 1000 milliseconds in a second

int startDay = 0;
int startHour = 8;
int startMin = 59;
int startSec = 55;
unsigned long startTime = day * startDay + hour * startHour + minute * startMin + second * startSec;  // Time operation begins in ms

unsigned int currDay = 0;
unsigned int currHour = 0;
unsigned int currMin = 0;
unsigned int currSec = 0;
unsigned int currMil = 0;

unsigned long lastUpdate = 0;

int melody[] = {1000,1500,2000,2500,3000};  // An array of frequencies


void loop(){  
  button1State = digitalRead(button1);
  // What to do when reset button is pushed
  if (button1State == HIGH) {
    lcd.clear();
    LED = 0;  // Turns off LEDs
    digitalWrite(stClk, LOW);  // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, 0);  // Outputs a value of 0 to registers
    shiftOut(dataPin2, srClk, LSBFIRST, 0);
    digitalWrite(stClk, HIGH);  // Ends communications with register
  }
  
  if (millis() - lastUpdate > 1000) {  // Checks if 1 second has elapsed
    lastUpdate = millis() + startTime;
    currSec = (lastUpdate/second)%60;  // Converts miliseconds to other time units
    currMin = (lastUpdate/minute)%60;
    currHour = (lastUpdate/hour)%24;
    currDay = (lastUpdate/day)%30;
    printTime();
    test();
    if ((currSec == 0) && (currMin == 0) && (currHour == alarm1)) {  // Checks if time = alarm time
      LED = (1 << (currDay * 2));
      lcd.setCursor(0, 1);
      lcd.print("Time for AM RX");
      lcd.setCursor(0, 0);
      notify();
    }
    if ((currSec == 0) && (currMin == 0) && (currHour == alarm2)){
      LED = (2 << (currDay * 2));
      lcd.setCursor(0, 1);  // Displays secondary message
      lcd.print("Time for PM RX");
      lcd.setCursor(0, 0);
      notify();
    }
  }
}

// A method to test and cycle through all LEDs
void test(){
  while(true){
    Serial.print("Day: ");
    Serial.println(currDay);
    LED = (1 << (currDay * 2));
    notify();
    delay(2000);
    LED = (2 << (currDay * 2));
    notify();
    delay(2000);
    currDay += 1;
  }
}

// Method to alert user an alarm is triggered
void notify(){
  if (LED < 256){  // Uses first register
    digitalWrite(stClk, LOW);  // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, LED);
    digitalWrite(stClk, HIGH);  // Ends communications with register
  }
  else if(LED >= 256){  // Uses second register
    digitalWrite(stClk, LOW);  // Sets register to recieve data
    shiftOut(dataPin, srClk, LSBFIRST, 0);
    shiftOut(dataPin2, srClk, LSBFIRST, (LED >> 8));  // Shifts data by 8 bits to other register
    digitalWrite(stClk, HIGH);  // Ends communications with register
  }
  // Sounds alarm
  for (int thisNote = 0; thisNote < (sizeof(melody) / sizeof(melody[0])) ; thisNote++){  // Loops for each note in melody[]
      tone(13, melody[thisNote], 500);  // Plays the note at the position in melody[] for 100ms
      delay(500);  // Waits for 100ms for the note to finish
    }
}

void printTime(){
  delay(50);
  lcd.setCursor(0,0);
  sprintf(output, "day:%02d %02d:%02d:%02d",currDay, currHour, currMin, currSec);  // Formats time output to LCD
  lcd.print(output);
}
