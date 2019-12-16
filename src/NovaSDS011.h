#pragma once

// NovaSDS011( sensor of PM2.5 and PM10 )
// ---------------------------------
//
// By R. Orecki
// December 2019
//
// Documentation:
//		- The iNovaFitness NovaSDS(datasheet)
//

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SoftwareSerial.h>

typedef uint8_t CommandType[19];
typedef uint8_t ReplyType[10];

enum DataReportingMode
{
	active = 0,
	query = 1,
	error = 0xFF
};

enum QuerryError
{
	no_error = 0,
	no_new_data = 1,
	response_error = 2,
	call_to_often = 3
};

class NovaSDS011 {
	public:
		/**
		* Constructor.
		*/
		NovaSDS011();
		
		/**
		* Initialize comunication via serial bus.
		* @param pin_rx 
		* @param pin_tx 
		* @param wait_write_read Max time in ms to wait for respons after sending command to sensor.
		*/
		void begin(uint8_t pin_rx, uint8_t pin_tx, uint16_t wait_write_read = 100);

		/**
		* Set raport mode.
		* Report query mode：Sensor received query data command to report a measurement data.
        * Report active mode：Sensor automatically reports a measurement data in a work period.
		* @param mode choosen mode
		* @param device_id device id (optional)
		* @return true if operation was sucessfull 
		*/
		bool setDataReportingMode(DataReportingMode mode, uint16_t device_id = 0xFFFF);

		/**
		* Get raport mode.
		* Report query mode：Sensor received query data command to report a measurement data.
        * Report active mode：Sensor automatically reports a measurement data in a work period.
		* @param device_id device id (optional)
		* @return DataReportingMode
		*/
		DataReportingMode getDataReportingMode(uint16_t device_id = 0xFFFF);


		QuerryError queryData(float &PM25, float &PM10, uint16_t device_id = 0xFFFF);





		void setDutyCycle(uint8_t duty_cycle);
		uint8_t getDutyCycle();
	
	
		void sleep();
		void wakeup();
		
		
		String SDS_version_date();
		void start_SDS();
		void stop_SDS();
		void set_initiative_SDS();
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
		* Calculate checksum from given repry.
		* Checksum: Low 8bit of the sum result of Data Bytes（not including packet head, tail and
		* Command ID).
		* @param cmd input reply
		* @return checksum value
		*/
		uint8_t calculateReplyCheckSum(ReplyType reply);	


		void readReply(ReplyType &reply);	

		/**
		* Max time to wait for respons after sending command to sensor.
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
