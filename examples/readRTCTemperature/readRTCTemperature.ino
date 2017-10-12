/*  Temperature sensor reading example for DS3231RTC library
 *   
 *   Reads the temperature sensor value from the RTC and prints it over serial
 *   
 *   
 *   ********** WARNING *********
 *   *
 *   *  The readTemp() function starts a conversion and will block
 *   *  until the conversion is done.   
 *   *  Please take that into consideration!
 *   *
 *   *  See the readRTCTemperatureAsync example for separate conversion 
 *   *  start and temperature reading.
 *   *
 *   *  Also see that example if you only need periodic updates, as the 
 *   *  DS3231 starts a new conversion every 64 seconds automatically.
 *   *
 *   ****************************
 *   
 *   
 *   Charles Dorval 11/10/2016
 *        
*/


#include <TimeLib.h>
#include <DS3231RTC.h>

void setup() {
  Serial.begin(57600);
}

void loop() {

  Serial.print("Temperature: ");
  Serial.print(RTC.readTemp(), 2);
  Serial.println(" °C");
  Serial.println();
  delay(1000);

}
