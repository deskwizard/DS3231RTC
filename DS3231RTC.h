/*
 * DS3231RTC.h - DS323RTC Arduino library
 * https://github.com/deskwizard/DS3231RTC
 *
 * Adds support for additional functionalities provided by the DS3231 RTC over the DS1307.
 *
 * This library is intended to be used with Arduino 'Time' library
 * as an almost drop-in replacement for the original DS1307RTC library by PJRC.
 * 
 * Charles Dorval 11/10/2017
 *
 * PJRC Time library: https://www.pjrc.com/teensy/td_libs_DS1307RTC.html
 * PJRC DS1307RTC library: https://www.pjrc.com/teensy/td_libs_DS1307RTC.html
 * 
 */

#ifndef DS3231RTC_h
#define DS3231RTC_h

#include <TimeLib.h>

// Status register Alarm bits
#define ALARM_1 0
#define ALARM_2 1

// Bitmasks for the Square wave / interrupt pin settings
#define SQW_1HZ   0x00
#define SQW_1024KHZ  0x08
#define SQW_4096KHZ  0x10
#define SQW_8192KHZ  0x18
#define INT_MODE     0x42

// Alarm 1 mode bitmasks
// Alarm 1 bits are defined as follows:
// A1M1 = bit 0, A1M2 = bit 1, A1M3 = bit 2, A1M4 = bit 7, DYDT = bit 6
#define A1_EVERY_SEC     0x87
#define A1_MATCH_SEC    0x86
#define A1_MATCH_MIN_SEC    0x84
#define A1_MATCH_HR_MIN_SEC 0x80
#define A1_MATCH_DAY_HR_MIN_SEC 0x00
#define A2_MATCH_WEEKDAY_HR_MIN 0x40

// Alarm 2 mode bitmasks
// Alarm 2 bits are defined as follows:
// A2M2 = bit 1, A2M3 = bit 2, A2M4 = bit 7, DYDT = bit 6
#define A2_EVERY_MIN    0x86        // at seconds = 00
#define A2_MATCH_MIN    0x84
#define A2_MATCH_HR_MIN 0x80
#define A2_MATCH_DAY_HR_MIN 0x00
#define A2_MATCH_WEEKDAY_HR_MIN  0x40


class DS3231RTC {

  public:
    DS3231RTC();

    // Functionally equivalent functions to DS1307RTC
    static time_t get();
    bool set(time_t t);
    static bool read(tmElements_t &tm);
    bool write(tmElements_t &tm);
    static bool chipPresent() { return exists; }


    // Additional DS3231 Functions
    uint8_t setSQWPin(uint8_t modeSQWPin);

    uint8_t enableBBSQW();
    uint8_t disableBBSQW();

    uint8_t enable32kHz();
    uint8_t disable32kHz();
    uint8_t read32kHzState();

    bool getOSF();
    uint8_t clearOSF();

    uint8_t Alarm1Enable();
    uint8_t Alarm1Disable();
    uint8_t Alarm2Enable();
    uint8_t Alarm2Disable();
    uint8_t clearAlarm1();
    uint8_t clearAlarm2();
    bool alarm1Triggered();
    bool alarm2Triggered();
    bool alarm1State();
    bool alarm2State();
    tmElements_t readAlarm1Time();
    tmElements_t readAlarm2Time();
    uint8_t getAlarm1Mode();
    uint8_t getAlarm2Mode();
    uint8_t setAlarm1Mode(uint8_t alarmMode);
    uint8_t setAlarm2Mode(uint8_t alarmMode);
    uint8_t setAlarm1(uint8_t setHour, uint8_t setMinute, uint8_t setSecond, uint8_t setDay, uint8_t alarmMode);
    uint8_t setAlarm2(uint8_t setHour, uint8_t setMinute, uint8_t setDay, uint8_t alarmMode);

    bool conversionInProgress();
    uint8_t startConversion();
    float readTemp();
    float readLastTemp();

    int8_t getAgingOffset();
    uint8_t setAgingOffset(uint8_t newAgingOffset);


  private:
    static bool exists;

    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);

    int16_t tempInQuarters(uint8_t msb, uint8_t lsb);
    int8_t twos2dec(uint8_t twosComp);
    uint8_t dec2twos(int8_t decimal);

    uint8_t readRegister(uint8_t regAddr);
    uint8_t writeRegister(uint8_t regAddr, uint8_t data);
  	uint8_t readRegisters(uint8_t regAddr, uint8_t* buffer, uint8_t byteCount);
    uint8_t writeRegisters(uint8_t regAddr, uint8_t* buffer, uint8_t byteCount);

};

#ifdef RTC
#undef RTC // workaround for Arduino Due, which defines "RTC"...
#endif

extern DS3231RTC RTC;

#endif
 

