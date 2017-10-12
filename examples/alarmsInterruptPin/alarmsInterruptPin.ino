/*  alarmsInterruptPin example for DS3231RTC library

     Demonstrate the use of the alarm functions of the library
     with both software and hardware RTC interrupts.

     Waits for an interrupt signal from the RTC on digital
     pin 2 before reading from the I²C bus instead of
     repeatedly reading the state of the alarms.

     Either alarm will trigger the interrupt pin if their
     interrupts are enabled.


     Charles Dorval 11/10/2016


     Valid alarm modes for Alarm 1:

        A1_EVERY_SEC                  ->  Alarm will trigger once per second
        A1_MATCH_SEC                  ->  Alarm will trigger when seconds match
        A1_MATCH_MIN_SEC              ->  Alarm will trigger when minutes and seconds match
        A1_MATCH_HR_MIN_SEC           ->  Alarm will trigger when hours, minutes and seconds match
        A1_MATCH_DAY_HR_MIN_SEC       ->  Alarm will trigger when day of the month, hours, minutes and seconds match
        A1_MATCH_WEEKDAY_HR_MIN_SEC   ->  Alarm will trigger when day of the week, hours, minutes and seconds match


     Valid alarm modes for Alarm 2:

        A2_EVERY_MIN             ->   Alarm will trigger once per minute (at seconds 00)
        A2_MATCH_MIN             ->   Alarm will trigger when minutes match
        A2_MATCH_HR_MIN          ->   Alarm will trigger when hours and minutes match
        A2_MATCH_DAY_HR_MIN      ->   Alarm will trigger when day of the month, hours and minutes match
        A2_MATCH_WEEKDAY_HR_MIN  ->   Alarm will trigger when day of the week, hours and minutes match

*/


#include <TimeLib.h>              // https://www.pjrc.com/teensy/td_libs_Time.html
#include <DS3231RTC.h>

const char RTCinterruptPin = 2;

void setup() {

  Serial.begin(57600);

  pinMode(2, INPUT_PULLUP);   // A pullup is required, either use the internal one or use an external one

  // Clear alarm interrupt flags
  RTC.clearAlarm1();
  RTC.clearAlarm2();

  RTC.Alarm1Enable();      // Enable Alarm 1
  RTC.Alarm2Enable();      // Enable Alarm 2

  RTC.setSQWPin(INT_MODE);          // Enable RTC interrupt pin

  // format: hours, minutes, seconds, day/date, mode
  RTC.setAlarm1(22, 20, 30, 6, A1_MATCH_HR_MIN_SEC);

  // Format: hours, minutes, day/date, mode
  RTC.setAlarm2(22, 21, 0, A2_MATCH_MIN);

  Serial.print("Time is currently ");
  printTime(RTC.get());

  printAlarm1(RTC.readAlarm1Time());    // Prints alarm 1 data
  printAlarm2(RTC.readAlarm2Time());    // Prints alarm 2 data

}

void loop() {

  if (digitalRead(2) == LOW) {

    if (RTC.alarm1Triggered() == 1) {

      Serial.print("Alarm 1 triggered at ");
      printTime(RTC.get());

      // Clear alarm 1 interrupt flag
      // Will turn off the interrupt pin
      RTC.clearAlarm1();
    }
    else if (RTC.alarm2Triggered() == 1) {

      Serial.print("Alarm 2 triggered at ");
      printTime(RTC.get());

      // Clear alarm 2 interrupt flag
      // Will turn off the interrupt pin
      RTC.clearAlarm2();
    }
  }


}


// Serial printing functions

// Prints alarm 1 information
void printAlarm1(tmElements_t tm) {
  Serial.println();
  Serial.print("Alarm 1 is set to: ");
  sPrintI00(tm.Hour);
  sPrintDigits(tm.Minute);
  sPrintDigits(tm.Second);
  Serial.print(" on day: ");
  sPrintI00(tm.Day);
  Serial.println();
}

// Prints alarm 2 information
void printAlarm2(tmElements_t tm) {
  Serial.println();
  Serial.print("Alarm 2 is set to: ");
  sPrintI00(tm.Hour);
  sPrintDigits(tm.Minute);
  Serial.print(" on day: ");
  sPrintI00(tm.Day);
  Serial.println();
}

// Function to print time
void printTime(time_t t) {
  sPrintI00(hour(t));
  sPrintDigits(minute(t));
  sPrintDigits(second(t));
  Serial.print(' ');
  Serial.print(dayShortStr(weekday(t)));
  Serial.print(' ');
  Serial.print(monthShortStr(month(t)));
  Serial.print(' ');
  sPrintI00(day(t));
  Serial.print(' ');
  Serial.println(year(t));
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val) {
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
  return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val) {
  Serial.print(':');
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
}
