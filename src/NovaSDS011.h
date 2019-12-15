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


class NovaSDS011 {
	public:
		NovaSDS011();
		void begin(uint8_t pin_rx, uint8_t pin_tx);
		int read(float *p25, float *p10);
		void setDutyCycle(uint8_t duty_cycle);
		void sleep();
		void wakeup();
		
		
		String SDS_version_date();
		void start_SDS();
		void stop_SDS();
		void set_initiative_SDS();
	private:
		uint8_t calculateCheckSum(byte cmd[19]);		
		bool is_SDS_running = false;
		uint8_t _pin_rx, _pin_tx;
		Stream *sds_data;		
};
