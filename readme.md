# DS323RTC Arduino library

### Adds support for additional functionalities provided by the DS3231 RTC over the DS1307.

This library is intended to be used with [PJRC's 'Time' library](https://www.pjrc.com/teensy/td_libs_Time.html)
as an almost drop-in replacement for the [original DS1307RTC library by PJRC](https://www.pjrc.com/teensy/td_libs_DS1307RTC.html).

For more details on '_time_t_' and '_TimeElements_' variables types,
see the Time library documentation at the link provided.

_Charles Dorval_
 
_October 12th, 2017_


#

#### The following functions from the DS1307RTC library not available in the DS3231RTC library:

| DS1307RTC function | DS3231RTC replacement |
| --- | --- |
| RTC.isRunning(); | None |
| RTC.setCalibration(_calValue_); | RTC.setAgingOffset(_newAgingOffset_); |
| RTC.getCalibration(); | RTC.getAgingOffset(); |


#
## Library examples


| Name | Description |
| --- | --- |
| alarms |  Basic alarm handling (software only) |
| alarmsInterruptPin |  Alarm handling using the DS3231 interrupt pin |
| readRTCTemperature |  Basic temperature reading |
| readRTCTemperatureAsync |  Temperature reading (asynchronous / Last conversion result) |
| setRTC | Set and read back RTC time and date over serial |

#

## Time functions
*These functions are functionally equivalent to the Time functions of the DS1307RTC library.*

**RTC.get();**

Reads the current date & time as a 32 bit '_time_t_' number.

Returns 0 if the DS3231 does not respond.


**RTC.set(**_t_**);**

Sets the date & time, using a 32 bit '_time_t_' number. 

Returns _**true**_ on success or _**false**_ if any error occured.


**RTC.read(**_tm_**);**

Reads the current date & time as '_TimeElements_' variable.

Returns _**true**_ on success or _**false**_ if any error occured.


**RTC.write(**_tm_**);**

Sets the date & time, using a '_TimeElements_' variable.

Returns _**true**_ on success or _**false**_ if any error occured.


**RTC.chipPresent();**

Returns _**true**_ if a DS3231 compatible chip was present after using one of the 4 functions previously mentionned.

Returns _**false**_ if an error occured while communicating with the DS3231.

#
## Additional functions provided by the DS3231RTC Library

*The following functions return the return value of the Wire.endTransmission() functions calls unless otherwise noted.*


| Returned value | Error |
| --- | --- |
| 0 | None (Success) |
| 1 | Wire library - Data too long to fit in transmit buffer |
| 2 | Wire library - Received NACK on transmit of address |
| 3 | Wire library - Received NACK on transmit of data |
| 4 | Wire library - Other error |


#


**RTC.setSQWPin(**_modeSQWPin_**);**

Configures the function of the 'SQW/INT' pin

| Valid Values | (Default in bold) |
| --- | --- |
| SQW_1HZ |  'SQW/INT' pin outputs a 1Hz square wave |
| SQW_1024KHZ |  'SQW/INT' pin outputs a 1.024kHz square wave |
| SQW_4096KHZ |  'SQW/INT' pin outputs a 4.096kHz square wave |
| **SQW_8192KHZ** |  **'SQW/INT' pin outputs a 8.192kHz square wave** |
| INT_MODE | 'SQW/INT' pin is used as alarm interrupt pin |

**RTC.enableBBSQW();**

Enables battery-backed square wave output (If square wave output is enabled).

**RTC.disableBBSQW();**

Disables battery-backed square wave output.

**RTC.enable32kHz();**

Enables 32kHz output on the '32kHz' pin.

**RTC.disable32kHz();**

Disables 32kHz output on the '32kHz' pin.

**RTC.read32kHzState();**

Reads the state of the 32kHz output.

Returns _**true**_ if the 32kHz output is enabled, returns _**false**_ if disabled.

**RTC.getOSF();**

Reads the state of the Oscillator Stop Flag.

Returns _**true**_ if the Oscillator Stop Flag is set, _**false**_ if it's cleared.

**RTC.clearOSF();**

Clears the Oscillator Stop Flag.

#

## Alarms
### Alarms 1 functions

**RTC.Alarm1Enable();**

Enables Alarm 1 Interrupt.

The Alarm 1 flag ('_A1F_') will be set when an alarm match occurs.

_**NOTE:** The interrupt pin will be asserted if it's enabled._

**RTC.Alarm1Disable();**

Disables Alarm 1 Interrupt.

**RTC.clearAlarm1();**

Clears the Alarm 1 flag ('_A1F_').

_**NOTE:** The interrupt pin will be de-asserted if it was asserted (if the interrupt pin is enabled)._

**RTC.alarm1Triggered();**

Reads the state of the the Alarm 1 flag ('_A1F_').

Returns _**true**_ if the alarm has been triggered, _**false**_ if alarm hasn't triggered.

_**NOTE:** Alarm will remain triggered until it's flag is cleared._

**RTC.alarm1State();**

Reads the state of the alarm (enabled or disabled).

Returns _**true**_ if the alarm is enabled, _**false**_ if disabled.

**RTC.readAlarm1Time();**

Reads the Alarm 1 time values in a '_TimeElements_' variable.

Returns a '_TimeElements_' variable populated with the Alarm 1 values.

_**WARNING:** Not all fields are populated. You **CANNOT** convert that '_TimeElements_' into a '_time_t_' directly!_

**RTC.getAlarm1Mode();**

Read current Alarm 1 mode.

Returns the Alarm 1 mode. (See table below)

**RTC.setAlarm1Mode(**_alarmMode_**);**

Sets the Alarm 1 mode to the mode specified. (See table below)

**RTC.setAlarm1(**_setHour ,setMinute ,setSecond ,setDay ,alarmMode_**);**

Sets Alarm 1 to the specified values.

| Valid modes for Alarm 1 | |
| ------------ | ------------ |
| A1_EVERY_SEC | Alarm will trigger once per second |
| A1_MATCH_SEC | Alarm will trigger when seconds match |
| A1_MATCH_MIN_SEC | Alarm will trigger when minutes and seconds match |
| A1_MATCH_HR_MIN_SEC | Alarm will trigger when hours, minutes and seconds match |
| A1_MATCH_DAY_HR_MIN_SEC | Alarm will trigger when day of the month, hours, minutes and seconds match |
| A1_MATCH_WEEKDAY_HR_MIN_SEC | Alarm will trigger when day of the week, hours, minutes and seconds match |


### Alarms 2 functions

**RTC.Alarm2Enable();**

Enables Alarm 2 Interrupt.

The Alarm 2 flag ('_A2F_') will be set when an alarm match occurs.

_**NOTE:** The interrupt pin will be asserted if it's enabled._

**RTC.Alarm2Disable();**

Disables Alarm 2 Interrupt.

**RTC.clearAlarm2();**

Clears the Alarm 2 flag ('_A2F_').

_**NOTE:** The interrupt pin will be de-asserted if it was asserted (if the interrupt pin is enabled)._

**RTC.alarm2Triggered();**

Reads the state of the Alarm 2 flag ('_A2F_').

Returns _**true**_ if the alarm has been triggered, _**false**_ if alarm hasn't triggered.

_**NOTE:** Alarm will remain triggered until it's flag is cleared._

**RTC.alarm2State();**

Reads the state of the alarm (enabled or disabled).

Returns _**true**_ if the alarm is enabled, _**false**_ if disabled.

**RTC.readAlarm2Time();**

Reads the Alarm 2 time values in a '_TimeElements_' variable.

Returns a '_TimeElements_' variable populated with the Alarm 2 values.

_**WARNING:** Not all fields are populated. You **CANNOT** convert that '_TimeElements_' into a '_time_t_' directly!_

**RTC.getAlarm2Mode();**

Read current Alarm 2 mode.

Returns the Alarm 2 mode. (See table below)

**RTC.setAlarm2Mode(**_alarmMode_**);**

Sets the Alarm 2 mode to the mode specified. (See table below)

**RTC.setAlarm2(**_setHour ,setMinute ,setDay ,alarmMode_**);**

Sets Alarm 2 to the specified values.


| Valid modes for Alarm 2 | |
| ------------ | ------------ |
| A2_EVERY_MIN | Alarm will trigger once per minute (at seconds 00) |
| A2_MATCH_MIN | Alarm will trigger when minutes match |
| A2_MATCH_HR_MIN | Alarm will trigger when hours and minutes match |
| A2_MATCH_DAY_HR_MIN | Alarm will trigger when day of the month, hours and minutes match |
| A2_MATCH_WEEKDAY_HR_MIN | Alarm will trigger when day of the week, hours and minutes match |


#
## Temperature sensor functions

The DS3231 automatically starts a new temperature conversion every 64 seconds.
If you don't need "real-time" temperature measurement, just use readLastTemp() without requesting a conversion.

_**Warning:** As mentionned in the datasheet, you need to make sure no conversion is in progress before requesting a new one._

**RTC.readTemp();**

Requests a new temperature conversion, waits for completion and returns the result as a float.

**RTC.readLastTemp();**

Returns the result of the last temperature conversion as a float.

**RTC.startConversion();**

Requests a new temperature conversion.

**RTC.conversionInProgress();**

Returns _**true**_ if a temperature conversion is in progress, _**false**_ otherwise.


#
## Aging offset functions

**RTC.getAgingOffset();**

Reads the current aging offset value from the RTC and returns it's decimal value.

**RTC.setAgingOffset(**_newAgingOffset_**)**

Sets the RTC aging offset (in decimal).



