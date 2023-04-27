/** 
 * @file NovaSDS011.h
 * @brief Driver for Nova Fitness sds011 air quality sensor.
 *
 * This driver implements protocol described in 
 * Laser Dust Sensor Control Protocol V1.3
 *
 * @author R. Orecki
 * 12.2019
 */

#pragma once

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NO_TRACES 

#ifdef ESP32
// ESP32 does not support softwareserial.
#include<HardwareSerial.h>
#endif

typedef uint8_t CommandType[19];
typedef uint8_t ReplyType[10];

enum DataReportingMode
{
	active = 0,
	query = 1,
	report_error = 0xFF
};

enum QuerryError
{
	no_error = 0,
	no_new_data = 1,
	response_error = 2,
	call_to_often = 3
};

enum WorkingMode
{
	mode_sleep = 0,
	mode_work = 1,
	mode_error = 0xFF
};

struct SDS011Version
{
	bool valid;
	uint8_t year;
	uint8_t month;
	uint8_t day;
};


class NovaSDS011
{
public:
	/**
		* Constructor.
		*/
	NovaSDS011();

#ifndef ESP32
	/**
		* Initialize communication via serial bus.
		* @param pin_rx 
		* @param pin_tx 
		* @param wait_write_read Max time in ms to wait for response after sending command to sensor.
		*/
	void begin(uint8_t pin_rx, uint8_t pin_tx, uint16_t wait_write_read = 500);

#else
	/**
		* Initialize communication via serial bus.
		* @param serial The hardware serial to use
		* @param pin_rx 
		* @param pin_tx 
		* @param wait_write_read Max time in ms to wait for response after sending command to sensor.
		*/
	void begin(HardwareSerial * serial, uint8_t pin_rx, uint8_t pin_tx, uint16_t wait_write_read = 500);
#endif

	/**
		* Set report mode to specific device or to all devices connected to bus.
		* Report query mode：Sensor received query data command to report the measurement data.
        * Report active mode：Sensor automatically reports the measurement data in a work period.
		* @param mode chosen mode
		* @param device_id device id (optional)
		* @return true if operation was sucessful 
		*/
	bool setDataReportingMode(DataReportingMode mode, uint16_t device_id = 0xFFFF);

	/**
		* Get report mode from specific device or from all devices connected to bus.
		* Report query mode：Sensor received query data command to report the measurement data.
        * Report active mode：Sensor automatically reports the measurement data in a work period.
		* @param device_id device id (optional)
		* @return DataReportingMode
		*/
	DataReportingMode getDataReportingMode(uint16_t device_id = 0xFFFF);

	/**
		* Send query to sensor asking for measurement data.
		* According to specification recommended query interval of not less than 3 seconds.
		* @param [out] PM25 value of PM2.5 particles in (μg/m3)
		* @param [out] PM10 value of PM10 particles in (μg/m3)
		* @param device_id device id (optional)
		* @return QuerryError
		*/
	QuerryError queryData(float &PM25, float &PM10, uint16_t device_id = 0xFFFF);


	/**
		* Set new device ID to specific device or to all devices connected to bus.
		* @param new_device_id new device id
		* @param device_id device id (optional)
		* @return true if operation was sucessful 
		*/
	bool setDeviceID(uint16_t new_device_id, uint16_t device_id = 0xFFFF);

	/**
		* Set new working mode to specific device or to all devices connected to bus.
		* @param mode new mode
		* @param device_id device id (optional)
		* @return true if operation was sucessful 
		*/
	bool setWorkingMode(WorkingMode mode, uint16_t device_id = 0xFFFF);

	/**
		* Get working mode from specific device or from all devices connected to bus.
		* It may be imposible to get working mode if sensor is sleeping.
		* @param device_id device id (optional)
		* @return WorkingMode
		*/
	WorkingMode getWorkingMode(uint16_t device_id = 0xFFFF);

	/**
		* Set duty cycle to specific device or to all devices connected to bus.
		* 0：continuous(default) 
		* 1-30minute：[work 30 seconds and sleep n*60-30 seconds] 
		* @param duty_cycle 
		* @param device_id device id (optional)
		* @return true if operation was sucessful 
		*/
	bool setDutyCycle(uint8_t duty_cycle, uint16_t device_id = 0xFFFF);

	/**
		* Get duty cycle from specific device or from all devices connected to bus.
		* @param device_id device id (optional)
		* @return uint8_t duty cycle, 0xFF if error occurs
		*/
	uint8_t getDutyCycle(uint16_t device_id = 0xFFFF);

	/**
		* Get software version from specific device or from all devices connected to bus.
		* @param device_id device id (optional)
		* @return SDS011Version valid is false if error occurs
		*/
	SDS011Version getVersionDate(uint16_t device_id = 0xFFFF);
	
private:
	void clearSerial();
	/**
		* Calculate checksum from given command.
		* Checksum: Low 8bit of the sum result of Data Bytes（not including packet head, tail and
		* Command ID).
		* @param cmd input command
		* @return checksum value
		*/
	uint8_t calculateCommandCheckSum(CommandType cmd);

	/**
		* Calculate checksum from given reply.
		* Checksum: Low 8bit of the sum result of Data Bytes（not including packet head, tail and
		* Command ID).
		* @param cmd input reply
		* @return checksum value
		*/
	uint8_t calculateReplyCheckSum(ReplyType reply);

	/**
		* Wait up to _waitWriteRead ms for reply to arrive and read it into &reply. 
		* Command ID).
		* @param [out] reply place for reply
		* @return if timeout
		*/
	bool readReply(ReplyType &reply);

	void DebugOut(const String &text, bool linebreak = true);

	/**
		* Max time to wait for response after sending command to sensor.
		*/
	uint16_t _waitWriteRead = 1000;

	/**
		* Current state of SDS011 sensor.
		*/
	bool _isSDSRunning = false;

	/**
		* Current state of SDS011 sensor.
		*/
	Stream *_sdsSerial;
};
