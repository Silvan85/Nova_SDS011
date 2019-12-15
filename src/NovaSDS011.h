// NovaSDS(st sensor PM2.5 and PM10
// ---------------------------------
//
// By R. Zschiegner (rz@madavi.de)
// April 2016
//
// Documentation:
//		- The iNovaFitness NovaSDS(tasheet
//

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SoftwareSerial.h>


class NovaSDS() {
	public:
		NovaSDS();
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
		uint8_t _pin_rx, _pin_tx;
		Stream *sds_data;		
};
