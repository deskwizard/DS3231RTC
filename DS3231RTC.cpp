/*
 * DS3231RTC.c - DS323RTC Arduino library
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


#include <Arduino.h>

#if defined (__AVR_ATtiny84__) || defined(__AVR_ATtiny85__) || (__AVR_ATtiny2313__)
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif
#include "DS3231RTC.h"

#define DS3231_TIME_BASE     0x00
#define DS3231_ADDRESS  0x68
#define DS3231_CONTROL  0x0E
#define DS3231_STATUS   0x0F
#define DS3231_ALARM1_BASE  0x07
#define DS3231_ALARM2_BASE  0x0B
#define DS3231_TEMP_BASE  0x11
#define DS3231_AGING_OFFSET 0x10

DS3231RTC::DS3231RTC() {
  Wire.begin();
}
  

// PUBLIC FUNCTIONS


// Time

// Get time from RTC and convert into time_t
time_t DS3231RTC::get() {
  tmElements_t tm;
  if (read(tm) == false) return 0;
  return(makeTime(tm));
}

// Convert time_t into tmElements_t and write to RTC
bool DS3231RTC::set(time_t t) {
  tmElements_t tm;
  breakTime(t, tm);
  return write(tm); 
}

// Aquire data from the RTC chip in BCD format
bool DS3231RTC::read(tmElements_t &tm) {

  uint8_t sec;
  Wire.beginTransmission(DS3231_ADDRESS);

  Wire.write((uint8_t)0x00); 

  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;

  // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
  Wire.requestFrom(DS3231_ADDRESS, tmNbrFields);
  if (Wire.available() < tmNbrFields) return false;

  sec = Wire.read();
  tm.Second = bcd2dec(sec & 0x7f);   
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec(Wire.read() );
  tm.Day = bcd2dec(Wire.read() );
  tm.Month = bcd2dec(Wire.read() );
  tm.Year = y2kYearToTm((bcd2dec(Wire.read())));

  return true;
}

// Write time to the RTC using a tmElements_t 
bool DS3231RTC::write(tmElements_t &tm) {

  uint8_t writeReg[7] = { 0 };

  writeReg[0] = dec2bcd(tm.Second);
  writeReg[1] = dec2bcd(tm.Minute);
  writeReg[2] = dec2bcd(tm.Hour);
  writeReg[3] = dec2bcd(tm.Wday);
  writeReg[4] = dec2bcd(tm.Day);
  writeReg[5] = dec2bcd(tm.Month);
  writeReg[6] = dec2bcd(tmYearToY2k(tm.Year));

  if (writeRegisters(DS3231_TIME_BASE, writeReg, 7) != 0) {
    exists = false;
  }
  else {
    exists = true;
  }

  return exists;
}


// SQW/INT pin and 32kHz output

// Set the SQW/INT pin configuration
// Valid values: SQW_1HZ, SQW_1024KHZ, SQW_4096KHZ, SQW_8192KHZ or INT_MODE
uint8_t DS3231RTC::setSQWPin(uint8_t modeSQWPin) {

  uint8_t controlReg = readRegister(DS3231_CONTROL);

  if (modeSQWPin == INT_MODE) {
    controlReg |= (1 << 2);   // Set interrupt control bit
  }
  else {
    controlReg &= ~(1 << 2);  // Clear interrupt control bit
    controlReg &= ~0x18;      // Clear bits 3-4
    controlReg |= modeSQWPin; // Set bits 2-3 with mode mask
  }

  return writeRegister(DS3231_CONTROL, controlReg);
}

// Enable battery-backed square wave output
uint8_t DS3231RTC::enableBBSQW() {
  uint8_t controlReg = readRegister(DS3231_CONTROL);
  controlReg |= (1 << 6);
  return writeRegister(DS3231_CONTROL, controlReg);
}

// Disable battery-backed square wave output
uint8_t DS3231RTC::disableBBSQW() {
  uint8_t controlReg = readRegister(DS3231_CONTROL);
  controlReg &= ~(1 << 6);
  return writeRegister(DS3231_CONTROL, controlReg);
}

// Enable 32kHz output
uint8_t DS3231RTC::enable32kHz() {
  uint8_t statusReg = readRegister(DS3231_STATUS);
  statusReg |= (1 << 3);
  return writeRegister(DS3231_STATUS, statusReg);
}

// Disable 32kHz output
uint8_t DS3231RTC::disable32kHz() {
  uint8_t statusReg = readRegister(DS3231_STATUS);
  statusReg &= ~(1 << 3);
  return writeRegister(DS3231_STATUS, statusReg);
}

// Read the 32kHz output configuration state from the RTC
// 1 = enabled, 0 = disabled
uint8_t DS3231RTC::read32kHzState() {
  uint8_t regData = readRegister(DS3231_STATUS);
  return (regData & (1 << 3)) >> 3;
}

// Read the oscillator stop flag
bool DS3231RTC::getOSF() {
  uint8_t regData = readRegister(DS3231_STATUS);
  return (regData & (1 << 7)) >> 7;
}

// Clear Oscillator Stop flag
uint8_t DS3231RTC::clearOSF() {
  uint8_t regData = readRegister(DS3231_STATUS);
  regData &= ~(1 << 7);
  return writeRegister(DS3231_STATUS, regData);
}


// Alarms

// Enable interrupt on alarm 1 match
uint8_t DS3231RTC::Alarm1Enable() {
  uint8_t regData = readRegister(DS3231_CONTROL);
  regData |= (1 << 0);
  return writeRegister(DS3231_CONTROL, regData);
}

// Disable interrupt on alarm 1 match
uint8_t DS3231RTC::Alarm1Disable() {
  uint8_t regData = readRegister(DS3231_CONTROL);
  regData &= ~(1 << 0);
  return writeRegister(DS3231_CONTROL, regData);
}

// Enable interrupt on alarm 2 match
uint8_t DS3231RTC::Alarm2Enable() {
  uint8_t regData = readRegister(DS3231_CONTROL);
  regData |= (1 << 1);
  return writeRegister(DS3231_CONTROL, regData);
}

// Disable interrupt on alarm 2 match
uint8_t DS3231RTC::Alarm2Disable() {
  uint8_t regData = readRegister(DS3231_CONTROL);
  regData &= ~(1 << 1);
  return writeRegister(DS3231_CONTROL, regData);
}

// Clear alarm 1 interrupt flag
uint8_t DS3231RTC::clearAlarm1() {
  uint8_t regData = readRegister(DS3231_STATUS);
  regData &= ~(1 << ALARM_1);
  return writeRegister(DS3231_STATUS, regData);
}

// Clear alarm 2 interrupt flag
uint8_t DS3231RTC::clearAlarm2() {
  uint8_t regData = readRegister(DS3231_STATUS);
  regData &= ~(1 << ALARM_2);
  return writeRegister(DS3231_STATUS, regData);
}

// Read the current alarm 1 interrupt state
// true = enabled, false = disabled
bool DS3231RTC::alarm1State() {
  uint8_t regData = readRegister(DS3231_CONTROL);
  return (regData & (1 << ALARM_1)) >> ALARM_1;
}

// Read the current alarm 2 interrupt state
// true = enabled, false = disabled
bool DS3231RTC::alarm2State() {
  uint8_t regData = readRegister(DS3231_CONTROL);
  return (regData & (1 << ALARM_2)) >> ALARM_2;
}

// Return true if alarm 1 has been triggered, false otherwise
bool DS3231RTC::alarm1Triggered() {
  uint8_t regData = readRegister(DS3231_STATUS);
  return (regData & (1 << ALARM_1)) >> ALARM_1;
}

// Return true if alarm 2 has been triggered, false otherwise
bool DS3231RTC::alarm2Triggered() {
  uint8_t regData = readRegister(DS3231_STATUS);
  return (regData & (1 << ALARM_2)) >> ALARM_2;
}

// Read the current alarm 1 time from the RTC
tmElements_t DS3231RTC::readAlarm1Time() {

  tmElements_t tm;

  uint8_t regRead[4] = { 0 };

  readRegisters(DS3231_ALARM1_BASE, regRead, 4);

  tm.Second = bcd2dec(regRead[0] & ~0x80);
  tm.Minute = bcd2dec(regRead[1] & ~0x80);
  tm.Hour = bcd2dec(regRead[2] & ~0xC0);
  tm.Day = bcd2dec(regRead[3] & ~0xC0);

  return tm;
}

// Read the current alarm 2 time from the RTC
tmElements_t DS3231RTC::readAlarm2Time() {

  tmElements_t tm;

  uint8_t regRead[3] = { 0 };
  readRegisters(DS3231_ALARM2_BASE, regRead, 3);

  tm.Minute = bcd2dec(regRead[0] & ~0x80);
  tm.Hour = bcd2dec(regRead[1] & ~0xC0);
  tm.Day = bcd2dec(regRead[2] & ~0xC0);

  return tm;
}

// Read the alarm 1 mode from the RTC
uint8_t DS3231RTC::getAlarm1Mode() {

  uint8_t alarmModeRead = 0;
  uint8_t regRead[4] = { 0 };

  readRegisters(DS3231_ALARM1_BASE, regRead, 4);

  alarmModeRead |= (regRead[0] & (1 << 7)) >> 7;  // read A1M1 bit and bitshift to bit pos 0
  alarmModeRead |= (regRead[1] & (1 << 7)) >> 6;  // read A1M2 bit and bitshift to bit pos 1
  alarmModeRead |= (regRead[2] & (1 << 7)) >> 5;  // read A1M3 bit and bitshift to bit pos 2 
  alarmModeRead |= (regRead[3] & ~0x3F);          // drop all bits but 6-7 and with alarmBits

  return alarmModeRead;
}

// Read the alarm 2 mode from the RTC
uint8_t DS3231RTC::getAlarm2Mode() {

  uint8_t alarmModeRead = 0; 
  uint8_t regRead[3] = { 0 };

  readRegisters(DS3231_ALARM2_BASE, regRead, 3);

  alarmModeRead |= (regRead[0] & (1 << 7)) >> 6;  // read A1M2 bit and bitshift to bit pos 1
  alarmModeRead |= (regRead[1] & (1 << 7)) >> 5;  // read A1M3 bit and bitshift to bit pos 2  
  alarmModeRead |= (regRead[2] & ~0x3F);          // drop all bits but 6-7 and with alarmBits

  return alarmModeRead;
}

// Set alarm 1 mode
uint8_t DS3231RTC::setAlarm1Mode(uint8_t alarmMode) {

  uint8_t regRead[4] = { 0 };

  readRegisters(DS3231_ALARM1_BASE, regRead, 4);

  regRead[0] = (regRead[0] & ~0x80) | ((alarmMode & (1 << 0)) << 7);
  regRead[1] = (regRead[1] & ~0x80) | ((alarmMode & (1 << 1)) << 6);
  regRead[2] = (regRead[2] & ~0x80) | ((alarmMode & (1 << 2)) << 5);
  regRead[3] = (regRead[3] & ~0xC0) | (alarmMode & ~0x3F);

  return writeRegisters(DS3231_ALARM1_BASE, regRead, 4);
}

// Set alarm 2 mode
uint8_t DS3231RTC::setAlarm2Mode(uint8_t alarmMode) {

  uint8_t regRead[3] = { 0 };

  readRegisters(DS3231_ALARM2_BASE, regRead, 3);

  regRead[0] = (regRead[0] & ~0x80) | ((alarmMode & (1 << 1)) << 6);
  regRead[1] = (regRead[1] & ~0x80) | ((alarmMode & (1 << 2)) << 5);
  regRead[2] = (regRead[2] & ~0xC0) | (alarmMode & ~0x3F);

  return writeRegisters(DS3231_ALARM2_BASE, regRead, 3);
}

// Set alarm 1
uint8_t DS3231RTC::setAlarm1(uint8_t setHour, uint8_t setMinute, uint8_t setSecond, uint8_t setDay, uint8_t alarmMode) {

  uint8_t regRead[4] = { 0 };

  regRead[0] = dec2bcd(setSecond) | ((alarmMode & (1 << 0)) << 7);
  regRead[1] = dec2bcd(setMinute) | ((alarmMode & (1 << 1)) << 6);
  regRead[2] = dec2bcd(setHour) | ((alarmMode & (1 << 2)) << 5);
  regRead[3] = dec2bcd(setDay) | (alarmMode & ~0x3F);

  return writeRegisters(DS3231_ALARM1_BASE, regRead, 4);
}

// Set alarm 2
uint8_t DS3231RTC::setAlarm2(uint8_t setHour, uint8_t setMinute, uint8_t setDay, uint8_t alarmMode) {

  uint8_t regRead[3] = { 0 };

  regRead[0] = dec2bcd(setMinute) | ((alarmMode & (1 << 1)) << 6);
  regRead[1] = dec2bcd(setHour) | ((alarmMode & (1 << 2)) << 5);
  regRead[2] = dec2bcd(setDay) | (alarmMode & ~0x3F);

  return writeRegisters(DS3231_ALARM2_BASE, regRead, 3);
}


// Temperature 

// Returns true if a conversion is in progress, false otherwise
bool DS3231RTC::conversionInProgress() {
  if (readRegister(DS3231_STATUS) & (1 << 2) || readRegister(DS3231_CONTROL) & (1 << 5) ) {  // Check for BUSY bit & CONV bit states
    return true;
  }
  return false;
}

// Start a temperature conversion
uint8_t DS3231RTC::startConversion() {
  uint8_t controlReg = readRegister(DS3231_CONTROL);
  controlReg |= (1 << 5);
  return writeRegister(DS3231_CONTROL, controlReg);
}

// Start a conversion, wait for completion and return the result
float DS3231RTC::readTemp() {
  startConversion();

  while (conversionInProgress() == true); // Wait for conversion to be finished

  return readLastTemp();
}

// Read result of the last conversion
float DS3231RTC::readLastTemp() {

  uint8_t regRead[2] = { 0 };

  readRegisters(DS3231_TEMP_BASE, regRead, 2);

  float temperature = tempInQuarters(regRead[0], regRead[1]) * 0.25f;   // Let's set it as positive right away

  return temperature;
}


// Aging Offset

// Returns the aging offset in decimal
int8_t DS3231RTC::getAgingOffset() {
  return twos2dec(readRegister(DS3231_AGING_OFFSET) );
}

// Writes a new value to the aging offset register
uint8_t DS3231RTC::setAgingOffset(uint8_t newAgingOffset) {
  return writeRegister(DS3231_AGING_OFFSET, dec2twos(newAgingOffset) );
}


// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS3231RTC::dec2bcd(uint8_t num) {
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS3231RTC::bcd2dec(uint8_t num) {
  return ((num/16 * 10) + (num % 16));
}

// Calculates the temperature in quarters from the 2 RTC temperature registers
int16_t DS3231RTC::tempInQuarters(uint8_t msb, uint8_t lsb) {

  int16_t quarters = (msb << 2) | (lsb >> 6);

  if (msb & (1 << 7)) {   // if bit 7 is set, we have a negative number// If bit 7 is set, we have a negative number

    msb &= ~(1 << 7);   // Ditch the sign bit
    msb ^= 0x7F;        // Invert MSB
    lsb = ~lsb;         // Invert LSB

    quarters = ((msb | lsb) >> 6) + 1;
    quarters = -quarters;
  }

  return quarters;
}

// Convert 8bit two's complement to decimal
int8_t DS3231RTC::twos2dec(uint8_t twosComp) {

  int8_t resultDec = twosComp;   // If the number is positive, don't do anything

  if (twosComp & (1 << 7)) {   // If bit 7 is set, we have a negative number

    twosComp &= ~(1 << 7);   // Ditch the sign bit
    twosComp ^= 0x7F;        // Invert

    // Here we have the positive value of the number, so lets make it negative
    resultDec = -(twosComp + 1);  // 2's complement = 1's complement +1
  }

  return resultDec;
}

// Convert 8bit decimal to two's complement
uint8_t DS3231RTC::dec2twos(int8_t decimal) {

  uint8_t resultTwos = decimal;     // If the number is positive, don't do anything

  if ((decimal &  (1 << 7))) {  // If bit 7 is set, we have a negative number
    decimal &= ~(1 << 7);   // Ditch the sign bit for now
    decimal ^= 0x7F;        // Invert
    decimal |= (1 << 7);    // Set sign bit back
  }

  return resultTwos;
}

// Read multiple register into buffer
uint8_t DS3231RTC::readRegisters(uint8_t regAddr, uint8_t* buffer, uint8_t byteCount) {

  Wire.beginTransmission((uint8_t) DS3231_ADDRESS);
  Wire.write(regAddr);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t) DS3231_ADDRESS, byteCount);

  for (uint8_t x = 0; x < byteCount; ++x) {
    buffer[x] = Wire.read();
  }

  return Wire.endTransmission();
}

// Write multiple register from buffer
uint8_t DS3231RTC::writeRegisters(uint8_t regAddr, uint8_t* buffer, uint8_t byteCount) {
  
  Wire.beginTransmission((uint8_t) DS3231_ADDRESS);
  Wire.write(regAddr);

  for (uint8_t x = 0; x < byteCount; ++x) {
    Wire.write(buffer[x]);
  }

  return Wire.endTransmission();
}

// Read a single register
uint8_t DS3231RTC::readRegister(uint8_t regAddr) {

  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(regAddr);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ADDRESS, (int)1);
  
  uint8_t regRead = Wire.read();
  Wire.endTransmission();

  return regRead;
}

// Write a single register
uint8_t DS3231RTC::writeRegister(uint8_t regAddr, uint8_t data) {

  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(regAddr);
  Wire.write(data);

  return Wire.endTransmission();
}


bool DS3231RTC::exists = false;

DS3231RTC RTC = DS3231RTC(); // create an instance for the user

