#include <NovaSDS011.h>

#define SDS_PIN_RX 2
#define SDS_PIN_TX 3

NovaSDS011 sds011;

void testDataReportingMode(uint16_t device_id = 0xFFFF)
{
  Serial.println("Start: testDataReportingMode with ID " + String(device_id));

  // Test Set to Active
  if (!sds011.setDataReportingMode(DataReportingMode::active, device_id))
  {
    Serial.println("Fail: setting SDS011 Report Mode to active.");
    return;
  }
  DataReportingMode reportMode = sds011.getDataReportingMode(device_id);
  if (reportMode == DataReportingMode::report_error)
  {
    Serial.println("Fail: getting SDS011 Report Mode.");
    return;
  }
  if (reportMode == DataReportingMode::query)
  {
    Serial.println("Fail: SDS011 Report Mode not as set.");
    return;
  }

  // Test Set to Query
  if (!sds011.setDataReportingMode(DataReportingMode::query, device_id))
  {
    Serial.println("Fail: setting SDS011 Report Mode to query.");
    return;
  }
  reportMode = sds011.getDataReportingMode(device_id);
  if (reportMode == DataReportingMode::report_error)
  {
    Serial.println("Fail: getting SDS011 Report Mode.");
    return;
  }
  if (reportMode == DataReportingMode::active)
  {
    Serial.println("Fail: SDS011 Report Mode not as set.");
    return;
  }

  // Test Set to Active Again
  if (!sds011.setDataReportingMode(DataReportingMode::active, device_id))
  {
    Serial.println("Fail: setting SDS011 Report Mode to active.");
    return;
  }
  reportMode = sds011.getDataReportingMode(device_id);
  if (reportMode == DataReportingMode::report_error)
  {
    Serial.println("Fail: getting SDS011 Report Mode.");
    return;
  }
  if (reportMode == DataReportingMode::query)
  {
    Serial.println("Fail: SDS011 Report Mode not as set.");
    return;
  }
  Serial.println("Success: testDataReportingMod");
}

void testDataWorkingMode(uint16_t device_id = 0xFFFF)
{
  Serial.println("Start: testDataWorkingMode with ID " + String(device_id));

  // Test Set to Sleep
  if (!sds011.setWorkingMode(WorkingMode::mode_sleep, device_id))
  {
    Serial.println("Fail: setting SDS011 Working Mode to sleep.");
    return;
  }

  delay(4000); //Hear fan turning off
  //Test Set to Work
  if (!sds011.setWorkingMode(WorkingMode::mode_work, device_id))
  {
    Serial.println("Fail: setting SDS011 Working Mode to work.");
    return;
  }
  delay(400);
  WorkingMode workingMode = sds011.getWorkingMode(device_id);
  if (workingMode == WorkingMode::mode_error)
  {
    Serial.println("Fail: getting SDS011 Working Mode.");
    return;
  }
  if (workingMode == WorkingMode::mode_sleep)
  {
    Serial.println("Fail: SDS011 Working Mode not as set.");
    return;
  }

  Serial.println("Success: testDataWorkingMode");
}

void testDataDutyCycle(uint16_t device_id = 0xFFFF)
{
  Serial.println("Start: testDataDutyCycle with ID " + String(device_id));

  // Test Set to 20
  if (!sds011.setDutyCycle(20, device_id))
  {
    Serial.println("Fail: setting SDS011 duty cycle to 20.");
    return;
  }
  uint8_t dutyCycle = sds011.getDutyCycle(device_id);
  if (dutyCycle == 0xFF)
  {
    Serial.println("Fail: getting SDS011 duty cycle.");
    return;
  }
  else if (dutyCycle != 20)
  {
    Serial.println("Fail: SDS011 duty cycle not set. Expected 20 get " + String(dutyCycle));
    return;
  }

  // Test Set to 0
  if (!sds011.setDutyCycle(0, device_id))
  {
    Serial.println("Fail: setting SDS011 duty cycle to 0.");
    return;
  }

  Serial.println("Success: testDataDutyCycle");
}

void testSetDeviceID(uint16_t device_id = 0xFFFF)
{
  Serial.println("Start: testSetDeviceID to " + String(device_id));

  if (!sds011.setDeviceID(device_id))
  {
    Serial.println("Fail setting SDS011 Device ID to 0xAAAA.");
    return;
  }

  Serial.println("Success: testSetDeviceID");
}

void setup()
{
  Serial.begin(115200); // Output to Serial at 9600 baud
  sds011.begin(SDS_PIN_RX, SDS_PIN_TX);

  //testDataReportingMode();
  //testDataWorkingMode();
  //testDataDutyCycle();
  //testSetDeviceID(0xAAAA);

  if (sds011.setWorkingMode(WorkingMode::mode_work))
  {
    Serial.println("SDS011 working mode \"Work\"");
  }
  else
  {
    Serial.println("FAIL: Unable to set working mode \"Work\"");
  }

  SDS011Version version = sds011.getVersionDate();

  if (version.valid)
  {
    Serial.println("SDS011 Firmware Vesion:\nYear: " + String(version.year) + "\nMonth: " +
                   String(version.month) + "\nDay: " + String(version.day));
  }
  else
  {
    Serial.println("FAIL: Unable to obtain Software Version");
  }

  if (sds011.setDutyCycle(5))
  {
    Serial.println("SDS011 Duty Cycle set to 5min");
  }
  else
  {
    Serial.println("FAIL: Unable to set Duty Cycle");
  }
}

void loop()
{
  float p25, p10;
  if (sds011.queryData(p25, p10) == QuerryError::no_error)
  {
    Serial.println(String(millis() / 1000) + "s:PM2.5=" + String(p25) + ", PM10=" + String(p10));
    delay(60000);
  }
}
