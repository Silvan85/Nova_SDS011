/** 
 * @file NovaSDS011.cpp
 * @brief Driver for Nova Fitness sds011 air quality sensor.
 *
 * This driver implements protocol described in 
 * Laser Dust Sensor Control Protocol V1.3
 *
 * @author R. Orecki
 * 12.2019
 */

#include "NovaSDS011.h"
#include "Commands.h"

#define MIN_QUERY_INTERVAL 3000

// --------------------------------------------------------
// NovaSDS011:constructor
// --------------------------------------------------------
NovaSDS011::NovaSDS011(void)
{
}

// --------------------------------------------------------
// Debug output
// --------------------------------------------------------
#ifndef NO_TRACES
void NovaSDS011::DebugOut(const String &text, bool linebreak)
{
  if (linebreak)
  {
    Serial.println(text);
  }
  else
  {
    Serial.print(text);
  }
}
#endif

// --------------------------------------------------------
// NovaSDS011:clearSerial
// --------------------------------------------------------
void NovaSDS011::clearSerial()
{
  while (_sdsSerial->available() > 0)
  {
    _sdsSerial->read();
  }
}

// --------------------------------------------------------
// NovaSDS011:calculateCheckSum
// --------------------------------------------------------
uint8_t NovaSDS011::calculateCommandCheckSum(CommandType cmd)
{
  uint16_t checksum = 0;
  for (int i = 2; i <= 16; i++)
  {
    checksum += cmd[i];
  }
  checksum &= 0xFF;
  return checksum;
}

// --------------------------------------------------------
// NovaSDS011:calculateCheckSum
// --------------------------------------------------------
uint8_t NovaSDS011::calculateReplyCheckSum(ReplyType reply)
{
  uint16_t checksum = 0;
  for (int i = 2; i <= 7; i++)
  {
    checksum += reply[i];
  }

  checksum &= 0xFF;
  return checksum;
}

// --------------------------------------------------------
// NovaSDS011:readReply
// --------------------------------------------------------
bool NovaSDS011::readReply(ReplyType &reply)
{
  bool timeout = true;
  
  uint64_t start = millis();
  while (_sdsSerial->available() < sizeof(ReplyType))
  {
    if (millis() > (start + _waitWriteRead))
    {
      break;
    }

    delay(1);
  }

#ifndef NO_TRACES
  uint32_t duration = (millis() - start);
  DebugOut("readReply - Wait for " + String(duration) + "ms");
#endif

  for (int i = 0; (_sdsSerial->available() > 0) && (i < sizeof(ReplyType)); i++)
  {
    reply[i] = _sdsSerial->read();
    if (i == (sizeof(ReplyType) - 1))
    {
      timeout = false;
    }
  }

  clearSerial();
  return timeout;
}

// --------------------------------------------------------
// NovaSDS011:begin
// --------------------------------------------------------
void NovaSDS011::begin(uint8_t pin_rx, uint8_t pin_tx, uint16_t wait_write_read)
{
  _waitWriteRead = wait_write_read;
  SoftwareSerial *softSerial = new SoftwareSerial(pin_rx, pin_tx);

  // Initialize soft serial bus
  softSerial->begin(9600);
  _sdsSerial = softSerial;

  clearSerial();
}

// --------------------------------------------------------
// NovaSDS011:setDataReportingMode
// --------------------------------------------------------
bool NovaSDS011::setDataReportingMode(DataReportingMode mode, uint16_t device_id)
{
  ReplyType reply;

  REPORT_TYPE_CMD[3] = 0x01; //Set reporting mode
  REPORT_TYPE_CMD[4] = uint8_t(mode & 0xFF);
  REPORT_TYPE_CMD[15] = device_id & 0xFF;
  REPORT_TYPE_CMD[16] = (device_id >> 8) & 0xFF;
  REPORT_TYPE_CMD[17] = calculateCommandCheckSum(REPORT_TYPE_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(REPORT_TYPE_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("setDataReportingMode - Error read reply timeout");
#endif
    return false;
  }
  readReply(reply);

  REPORT_TYPE_REPLY[3] = REPORT_TYPE_CMD[3]; //Set reporting mode
  REPORT_TYPE_REPLY[4] = REPORT_TYPE_CMD[4]; //Reporting mode
  if (device_id != 0xFFFF)
  {
    REPORT_TYPE_REPLY[6] = REPORT_TYPE_CMD[15]; //Device ID byte 1
    REPORT_TYPE_REPLY[7] = REPORT_TYPE_CMD[16]; //Device ID byte 2
  }
  else
  {
    REPORT_TYPE_REPLY[6] = reply[6]; //Device ID byte 1
    REPORT_TYPE_REPLY[7] = reply[7]; //Device ID byte 2
  }
  REPORT_TYPE_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (REPORT_TYPE_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("setDataReportingMode - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(REPORT_TYPE_REPLY[i]));
#endif
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------
// NovaSDS011:getDataReportingMode
// --------------------------------------------------------
DataReportingMode NovaSDS011::getDataReportingMode(uint16_t device_id)
{
  ReplyType reply;

  REPORT_TYPE_CMD[3] = 0x00; //Get reporting mode
  REPORT_TYPE_CMD[15] = device_id & 0xFF;
  REPORT_TYPE_CMD[16] = (device_id >> 8) & 0xFF;
  REPORT_TYPE_CMD[17] = calculateCommandCheckSum(REPORT_TYPE_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(REPORT_TYPE_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("getDataReportingMode - Error read reply timeout");
#endif
    return DataReportingMode::report_error;
  }

  REPORT_TYPE_REPLY[3] = REPORT_TYPE_CMD[3]; //Get reporting mode
  REPORT_TYPE_REPLY[4] = reply[4];           //Reporting mode
  if (device_id != 0xFFFF)
  {
    REPORT_TYPE_REPLY[6] = REPORT_TYPE_CMD[15]; //Device ID byte 1
    REPORT_TYPE_REPLY[7] = REPORT_TYPE_CMD[16]; //Device ID byte 2
  }
  else
  {
    REPORT_TYPE_REPLY[6] = reply[6]; //Device ID byte 1
    REPORT_TYPE_REPLY[7] = reply[7]; //Device ID byte 2
  }
  REPORT_TYPE_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (REPORT_TYPE_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("getDataReportingMode - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(REPORT_TYPE_REPLY[i]));
#endif
      return DataReportingMode::report_error;
    }
  }

  if (reply[4] == DataReportingMode::active)
  {
    return DataReportingMode::active;
  }
  else if (reply[4] == DataReportingMode::query)
  {
    return DataReportingMode::query;
  }
  else
  {
    return DataReportingMode::report_error;
  }
}

// --------------------------------------------------------
// NovaSDS011:queryData
// --------------------------------------------------------
QuerryError NovaSDS011::queryData(float &PM25, float &PM10, uint16_t device_id)
{
  static uint64_t lastCall = 0;
  static uint16_t lastPM25 = 0;
  static uint16_t lastPM10 = 0;

  ReplyType reply;
  uint16_t pm25Serial = 0;
  uint16_t pm10Serial = 0;

  if (millis() < (lastCall + MIN_QUERY_INTERVAL))
  {
    return QuerryError::call_to_often;
  }
  lastCall = millis();

  QUERY_CMD[15] = device_id & 0xFF;
  QUERY_CMD[16] = (device_id >> 8) & 0xFF;
  QUERY_CMD[17] = calculateCommandCheckSum(QUERY_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(QUERY_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("queryData - Error read reply timeout");
#endif
    return QuerryError::response_error;
  }

  QUERY_REPLY[2] = reply[2]; //data byte 1 (PM2.5 low byte)
  QUERY_REPLY[3] = reply[3]; //data byte 2 (PM2.5 high byte)
  QUERY_REPLY[4] = reply[4]; //data byte 3 (PM10 low byte)
  QUERY_REPLY[5] = reply[5]; //data byte 4 (PM10 high byte)

  if (device_id != 0xFFFF)
  {
    QUERY_REPLY[6] = QUERY_CMD[15]; //Device ID byte 1
    QUERY_REPLY[7] = QUERY_CMD[16]; //Device ID byte 2
  }
  else
  {
    QUERY_REPLY[6] = reply[6]; //Device ID byte 1
    QUERY_REPLY[7] = reply[7]; //Device ID byte 2
  }
  QUERY_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (QUERY_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("queryData - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(REPORT_TYPE_REPLY[i]));
#endif
      return QuerryError::response_error;
    }
  }

  pm25Serial = reply[2];
  pm25Serial += (reply[3] << 8);
  pm10Serial = reply[4];
  pm10Serial += (reply[5] << 8);

  if ((lastPM25 == pm25Serial) && (lastPM10 == pm10Serial))
  {
    return QuerryError::no_new_data;
  }

  lastPM25 = pm25Serial;
  lastPM10 = pm10Serial;

  PM25 = (float)pm25Serial / 10.0;
  PM10 = (float)pm10Serial / 10.0;

  return QuerryError::no_error;
}

// --------------------------------------------------------
// NovaSDS011:setDeviceID
// --------------------------------------------------------
bool NovaSDS011::setDeviceID(uint16_t new_device_id, uint16_t device_id)
{
  ReplyType reply;

  SET_ID_CMD[13] = new_device_id & 0xFF;
  SET_ID_CMD[14] = (new_device_id >> 8) & 0xFF;
  SET_ID_CMD[15] = device_id & 0xFF;
  SET_ID_CMD[16] = (device_id >> 8) & 0xFF;
  SET_ID_CMD[17] = calculateCommandCheckSum(SET_ID_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(SET_ID_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("setDeviceID - Error read reply timeout");
#endif
    return false;
  }

  SET_ID_REPLY[6] = SET_ID_CMD[13]; //Device ID byte 1
  SET_ID_REPLY[7] = SET_ID_CMD[14]; //Device ID byte 2

  SET_ID_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (SET_ID_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("setDeviceID - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(REPORT_TYPE_REPLY[i]));
#endif
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------
// NovaSDS011:setWorkingMode
// --------------------------------------------------------
bool NovaSDS011::setWorkingMode(WorkingMode mode, uint16_t device_id)
{
  ReplyType reply;
  bool timeout;

  WORKING_MODE_CMD[3] = 0x01; //Set reporting mode
  WORKING_MODE_CMD[4] = uint8_t(mode & 0xFF);
  WORKING_MODE_CMD[15] = device_id & 0xFF;
  WORKING_MODE_CMD[16] = (device_id >> 8) & 0xFF;
  WORKING_MODE_CMD[17] = calculateCommandCheckSum(WORKING_MODE_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(WORKING_MODE_CMD[i]);
  }
  _sdsSerial->flush();

  timeout = readReply(reply);

  if ((mode == WorkingMode::mode_sleep) && (timeout))
  {
#ifndef NO_TRACES
    DebugOut("setWorkingMode - Read timeout");
#endif
    return true;
  }

  WORKING_MODE_REPLY[3] = WORKING_MODE_CMD[3]; //Set reporting mode
  WORKING_MODE_REPLY[4] = WORKING_MODE_CMD[4]; //Reporting mode
  if (device_id != 0xFFFF)
  {
    WORKING_MODE_REPLY[6] = WORKING_MODE_REPLY[15]; //Device ID byte 1
    WORKING_MODE_REPLY[7] = WORKING_MODE_REPLY[16]; //Device ID byte 2
  }
  else
  {
    WORKING_MODE_REPLY[6] = reply[6]; //Device ID byte 1
    WORKING_MODE_REPLY[7] = reply[7]; //Device ID byte 2
  }
  WORKING_MODE_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (WORKING_MODE_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("setWorkingMode - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(WORKING_MODE_REPLY[i]));
#endif
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------
// NovaSDS011:getWorkingMode
// --------------------------------------------------------
WorkingMode NovaSDS011::getWorkingMode(uint16_t device_id)
{
  ReplyType reply;

  WORKING_MODE_CMD[3] = 0x00; //Get reporting mode
  WORKING_MODE_CMD[15] = device_id & 0xFF;
  WORKING_MODE_CMD[16] = (device_id >> 8) & 0xFF;
  WORKING_MODE_CMD[17] = calculateCommandCheckSum(WORKING_MODE_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(WORKING_MODE_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("getWorkingMode - Error read reply timeout");
#endif
    return WorkingMode::mode_work_error;
  }

  WORKING_MODE_REPLY[3] = WORKING_MODE_CMD[3]; //Get reporting mode
  WORKING_MODE_REPLY[4] = reply[4];            //Reporting mode
  if (device_id != 0xFFFF)
  {
    WORKING_MODE_REPLY[6] = WORKING_MODE_CMD[15]; //Device ID byte 1
    WORKING_MODE_REPLY[7] = WORKING_MODE_CMD[16]; //Device ID byte 2
  }
  else
  {
    WORKING_MODE_REPLY[6] = reply[6]; //Device ID byte 1
    WORKING_MODE_REPLY[7] = reply[7]; //Device ID byte 2
  }
  WORKING_MODE_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (WORKING_MODE_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("getWorkingMode - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(WORKING_MODE_REPLY[i]));
#endif
      return WorkingMode::mode_work_error;
    }
  }

  if (reply[4] == WorkingMode::mode_sleep)
  {
    return WorkingMode::mode_sleep;
  }
  else if (reply[4] == WorkingMode::work)
  {
    return WorkingMode::mode_work;
  }
  else
  {
    return WorkingMode::mode_work_error;
  }
}

// --------------------------------------------------------
// NovaSDS011:setDutyCycle
// --------------------------------------------------------
bool NovaSDS011::setDutyCycle(uint8_t duty_cycle, uint16_t device_id)
{
  ReplyType reply;
  bool timeout;

  if (duty_cycle > 30)
  {
    return false;
  }

  DUTY_CYCLE_CMD[3] = 0x01; //Set reporting mode
  DUTY_CYCLE_CMD[4] = duty_cycle;
  DUTY_CYCLE_CMD[15] = device_id & 0xFF;
  DUTY_CYCLE_CMD[16] = (device_id >> 8) & 0xFF;
  DUTY_CYCLE_CMD[17] = calculateCommandCheckSum(DUTY_CYCLE_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(DUTY_CYCLE_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("setDutyCycle - Error read reply timeout");
#endif
    return false;
  }

  DUTY_CYCLE_REPLY[3] = DUTY_CYCLE_CMD[3]; //Set reporting mode
  DUTY_CYCLE_REPLY[4] = DUTY_CYCLE_CMD[4]; //Reporting mode
  if (device_id != 0xFFFF)
  {
    DUTY_CYCLE_REPLY[6] = DUTY_CYCLE_CMD[15]; //Device ID byte 1
    DUTY_CYCLE_REPLY[7] = DUTY_CYCLE_CMD[16]; //Device ID byte 2
  }
  else
  {
    DUTY_CYCLE_REPLY[6] = reply[6]; //Device ID byte 1
    DUTY_CYCLE_REPLY[7] = reply[7]; //Device ID byte 2
  }
  DUTY_CYCLE_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (DUTY_CYCLE_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("setDutyCycle - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(DUTY_CYCLE_REPLY[i]));
#endif
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------
// NovaSDS011:getDutyCycle
// --------------------------------------------------------
uint8_t NovaSDS011::getDutyCycle(uint16_t device_id)
{
  ReplyType reply;

  DUTY_CYCLE_CMD[3] = 0x00; //Get reporting mode
  DUTY_CYCLE_CMD[15] = device_id & 0xFF;
  DUTY_CYCLE_CMD[16] = (device_id >> 8) & 0xFF;
  DUTY_CYCLE_CMD[17] = calculateCommandCheckSum(DUTY_CYCLE_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(DUTY_CYCLE_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("getDutyCycle - Error read reply timeout");
#endif
    return WorkingMode::mode_work_error;
  }

  DUTY_CYCLE_REPLY[3] = DUTY_CYCLE_CMD[3]; //Get reporting mode
  DUTY_CYCLE_REPLY[4] = reply[4];          //Reporting mode
  if (device_id != 0xFFFF)
  {
    DUTY_CYCLE_REPLY[6] = DUTY_CYCLE_CMD[15]; //Device ID byte 1
    DUTY_CYCLE_REPLY[7] = DUTY_CYCLE_CMD[16]; //Device ID byte 2
  }
  else
  {
    DUTY_CYCLE_REPLY[6] = reply[6]; //Device ID byte 1
    DUTY_CYCLE_REPLY[7] = reply[7]; //Device ID byte 2
  }
  DUTY_CYCLE_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (DUTY_CYCLE_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("getDutyCycle - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(DUTY_CYCLE_REPLY[i]));
#endif
      return WorkingMode::mode_work_error;
    }
  }

  if (reply[4] > 30)
  {
    return 0xFF;
  }
  else
  {
    return reply[4];
  }
}

/*****************************************************************
/* read NovaSDS011 sensor values
/*****************************************************************/
SDS011Version NovaSDS011::getVersionDate(uint16_t device_id)
{
  ReplyType reply;

  VERSION_CMD[15] = device_id & 0xFF;
  VERSION_CMD[16] = (device_id >> 8) & 0xFF;
  VERSION_CMD[17] = calculateCommandCheckSum(VERSION_CMD);

  for (uint8_t i = 0; i < 19; i++)
  {
    _sdsSerial->write(VERSION_CMD[i]);
  }
  _sdsSerial->flush();

  if (readReply(reply))
  {
#ifndef NO_TRACES
    DebugOut("getVersionDate - Error read reply timeout");
#endif
    return {false, 0, 0, 0};
  }

  VERSION_REPLY[3] = reply[3]; //Get reporting mode
  VERSION_REPLY[4] = reply[4]; //Reporting mode
  VERSION_REPLY[5] = reply[5]; //Reporting

  if (device_id != 0xFFFF)
  {
    VERSION_REPLY[6] = VERSION_CMD[15]; //Device ID byte 1
    VERSION_REPLY[7] = VERSION_CMD[16]; //Device ID byte 2
  }
  else
  {
    VERSION_REPLY[6] = reply[6]; //Device ID byte 1
    VERSION_REPLY[7] = reply[7]; //Device ID byte 2
  }
  VERSION_REPLY[8] = calculateReplyCheckSum(reply);

  for (int i = 0; i < sizeof(ReplyType); i++)
  {
    if (VERSION_REPLY[i] != reply[i])
    {
#ifndef NO_TRACES
      DebugOut("getVersionDate - Error on byte " + String(i) + " Received byte=" + String(reply[i]) +
               " Expected byte=" + String(VERSION_REPLY[i]));
#endif
      return {false, 0, 0, 0};
    }
  }
  return {true, VERSION_REPLY[3], VERSION_REPLY[4], VERSION_REPLY[5]};
}