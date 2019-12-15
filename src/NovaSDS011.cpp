// NovaSDS011 dust sensor PM2.5 and PM10
// ---------------------
//
// By R. Zschiegner (rz@madavi.de)
// April 2016
//
// Documentation:
//		- The iNovaFitness NovaSDS011 datasheet
//

#include "NovaSDS011.h"
#include "Commands.h"

static bool is_SDS_running = false;


NovaSDS011::NovaSDS011(void) {

}

// --------------------------------------------------------
// NovaSDS011:read
// --------------------------------------------------------
int NovaSDS011::read(float *p25, float *p10) {
	byte buffer;
	int value;
	int len = 0;
	int pm10_serial = 0;
	int pm25_serial = 0;
	int checksum_is;
	int checksum_ok = 0;
	int error = 1;
	while ((sds_data->available() > 0) && (sds_data->available() >= (10-len))) {
		buffer = sds_data->read();
		value = int(buffer);
		switch (len) {
			case (0): if (value != 170) { len = -1; }; break;
			case (1): if (value != 192) { len = -1; }; break;
			case (2): pm25_serial = value; checksum_is = value; break;
			case (3): pm25_serial += (value << 8); checksum_is += value; break;
			case (4): pm10_serial = value; checksum_is += value; break;
			case (5): pm10_serial += (value << 8); checksum_is += value; break;
			case (6): checksum_is += value; break;
			case (7): checksum_is += value; break;
			case (8): if (value == (checksum_is % 256)) { checksum_ok = 1; } else { len = -1; }; break;
			case (9): if (value != 171) { len = -1; }; break;
		}
		len++;
		if (len == 10 && checksum_ok == 1) {
			*p10 = (float)pm10_serial/10.0;
			*p25 = (float)pm25_serial/10.0;
			len = 0; checksum_ok = 0; pm10_serial = 0.0; pm25_serial = 0.0; checksum_is = 0;
			error = 0;
		}
		yield();
	}
	return error;
}

// --------------------------------------------------------
// NovaSDS011:sleep
// --------------------------------------------------------
void NovaSDS011::sleep() {
	for (uint8_t i = 0; i < 19; i++) {
		sds_data->write(SLEEPCMD[i]);
	}
	sds_data->flush();
	while (sds_data->available() > 0) {
		sds_data->read();
	}
}

// --------------------------------------------------------
// NovaSDS011:wakeup
// --------------------------------------------------------
void NovaSDS011::wakeup() {
	sds_data->write(0x01);
	sds_data->flush();
}

// --------------------------------------------------------
// NovaSDS011:setDutyCycle
// --------------------------------------------------------
void NovaSDS011::setDutyCycle(uint8_t duty_cycle) {

	DUTTYCMD[4] = duty_cycle;
	unsigned int checksum = 0;
	for (int i = 2; i <= 16; i++)
	{
		checksum += DUTTYCMD[i];
	}
	
	checksum = checksum % 0x100;
	DUTTYCMD[17] = checksum;

	sds_data->write(DUTTYCMD, sizeof(DUTTYCMD));
	sds_data->flush();

	while (sds_data->available() > 0) 
	{
		sds_data->read();
	}
}

// --------------------------------------------------------
// NovaSDS011:begin
// --------------------------------------------------------
void NovaSDS011::begin(uint8_t pin_rx, uint8_t pin_tx) {
	_pin_rx = pin_rx;
	_pin_tx = pin_tx;

	SoftwareSerial *softSerial = new SoftwareSerial(_pin_rx, _pin_tx);

	//Initialize the 'Wire' class for I2C-bus communication.
	softSerial->begin(9600);

	sds_data = softSerial;
}

 /*****************************************************************
  /* read NovaSDS011 sensor values                                     *
  /*****************************************************************/
String NovaSDS011::SDS_version_date()
{
  const uint8_t version_SDS_cmd[] = {0xAA, 0xB4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB};
  String s = "";
  String value_hex;
  byte buffer;
  int value;
  int len = 0;
  String version_date = "";
  String device_id = "";
  int checksum_is;
  int checksum_ok = 0;
  int position = 0;


  delay(100);

  sds_data->write(version_SDS_cmd, sizeof(version_SDS_cmd));
  sds_data->flush();
  delay(100);

  while (sds_data->available() > 0)
  {
    buffer = sds_data->read();
    // debug_out(String(len) + " - " + String(buffer, HEX) + " - " + int(buffer) + " .", 1);
    //    "aa" = 170, "ab" = 171, "c0" = 192
    value = int(buffer);
    switch (len)
    {
    case (0):
      if (value != 170)
      {
        len = -1;
      };
      break;
    case (1):
      if (value != 197)
      {
        len = -1;
      };
      break;
    case (2):
      if (value != 7)
      {
        len = -1;
      };
      break;
    case (3):
      version_date = String(value);
      checksum_is = 7 + value;
      break;
    case (4):
      version_date += "-" + String(value);
      checksum_is += value;
      break;
    case (5):
      version_date += "-" + String(value);
      checksum_is += value;
      break;
    case (6):
      if (value < 0x10)
      {
        device_id = "0" + String(value, HEX);
      }
      else
      {
        device_id = String(value, HEX);
      };
      checksum_is += value;
      break;
    case (7):
      if (value < 0x10)
      {
        device_id += "0";
      };
      device_id += String(value, HEX);
      checksum_is += value;
      break;
    case (8):
      if (value == (checksum_is % 256))
      {
        checksum_ok = 1;
      }
      else
      {
        len = -1;
      };
      break;
    case (9):
      if (value != 171)
      {
        len = -1;
      };
      break;
    }
    len++;
    if (len == 10 && checksum_ok == 1)
    {
      s = version_date + "(" + device_id + ")";
      len = 0;
      checksum_ok = 0;
      version_date = "";
      device_id = "";
      checksum_is = 0;
    }
    yield();
  }

  return s;
}


/*****************************************************************
  /* start NovaSDS011 sensor                                           *
  /*****************************************************************/
void NovaSDS011::start_SDS()
{
  const uint8_t start_SDS_cmd[] =
      {
          0xAA, 0xB4, 0x06, 0x01, 0x01,
          0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0x05, 0xAB};
  sds_data->write(start_SDS_cmd, sizeof(start_SDS_cmd));
  sds_data->flush();
  
  is_SDS_running = true;
  
  while (sds_data->available() > 0) {
    sds_data->read();
  }
}

/*****************************************************************
  /* stop NovaSDS011 sensor                                            *
  /*****************************************************************/
void NovaSDS011::stop_SDS()
{
  const uint8_t stop_SDS_cmd[] =
      {
          0xAA, 0xB4, 0x06, 0x01, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0x05, 0xAB};
  sds_data->write(stop_SDS_cmd, sizeof(stop_SDS_cmd));
  is_SDS_running = false;
  sds_data->flush();
  
  while (sds_data->available() > 0) {
    sds_data->read();
  }
}

//set initiative mode
void NovaSDS011::set_initiative_SDS()
{
  //aa, 0xb4, 0x08, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0a, 0xab
  const uint8_t stop_SDS_cmd[] =
      {
          0xAA, 0xB4, 0x08, 0x01, 0x03,
          0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0x0A, 0xAB};
  sds_data->write(stop_SDS_cmd, sizeof(stop_SDS_cmd));
  is_SDS_running = false;
  sds_data->flush();
  
  while (sds_data->available() > 0) {
    sds_data->read();
  }
}

