/*  setRTC example for DS3231RTC library
 *   
 *   Provides an easy way to set the RTC over serial
 *   
 *   Configure the serial monitor at 57600 baud with 'Newline' line ending.
 *   
 *   
 *   Send the time/date over the serial monitor in the following format:
 *    
 *    hours, minutes, seconds, day, month, year
 *    
 *    
 *  For example, to set the time/date to 3rd of November 2017 at 15:42:30 send:
 *  
 *    15, 42, 30, 3, 11, 2017
 *   
 *   
 *   Charles Dorval 11/10/2016
 */


#include <TimeLib.h>
#include <DS3231RTC.h>

// The time_t variable type contains unix time value
// (The number of seconds elapsed since Jan 1st, 1970)
// see the Time library documentation for more details
// https://www.pjrc.com/teensy/td_libs_Time.html

time_t t; 


void setup() {
  Serial.begin(57600);

  t = RTC.get();    // Get the current time from DS3231

  Serial.print("RTC Time is currently: ");
  printTime(t);

}

void loop() {

  readTimeFromSerial();

}

void readTimeFromSerial() {

  // Read data if any is available
  while (Serial.available() > 0) {

    // Parse the integers from the serial data
    int hours = Serial.parseInt();
    int minutes = Serial.parseInt();
    int seconds = Serial.parseInt();
    int days = Serial.parseInt();
    int months = Serial.parseInt();
    int years = Serial.parseInt();

    if (Serial.read() == '\n') {  // End of data (Newline)

      // Set the time and date to the received values
      setTime(hours, minutes, seconds, days, months, years);
      Serial.print("Time is now: ");
      printTime(now());
      
      // Update the RTC with the new time and date
      RTC.set(now());
      Serial.print("RTC is now set to: ");
      printTime(RTC.get());
    }
  }
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
