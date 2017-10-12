/*  readRTCTemperatureAsync temperature sensor reading example for DS3231RTC library
 *   
 *   Demonstrate the temperature sensor functions in async mode
 *   
 *   Starts a temperature sensor conversion, wait for it's completion
 *   and prints the result of the last conversion.
 *   
 *   
 *    Note: The RTC starts a temperature conversion automatically
 *          every 64 seconds. If you don't need real time temperature
 *          measurement, you can only use readLastTemp() to read
 *          the result of the last conversion.
 * 
 * 
 *   Charles Dorval 11/10/2016
 * 
*/


#include <TimeLib.h>
#include <DS3231RTC.h>

void setup() {
  Serial.begin(57600);

  RTC.startConversion();
}

void loop() {

  // Do something else in the meantime obviously...

  if (RTC.conversionInProgress() == false) {   // Conversion completed

    Serial.print("Temperature: ");
    Serial.print(RTC.readLastTemp(), 2);
    Serial.println(" °C");
    Serial.println();

    RTC.startConversion();  // Start another conversion
  }

}



