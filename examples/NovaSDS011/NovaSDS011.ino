#include <NovaSDS011.h>

#define SDS_PIN_RX 2
#define SDS_PIN_TX 3

NovaSDS011 sds011;

void setup()
{
  Serial.begin(115200); // Output to Serial at 9600 baud
  sds011.begin(SDS_PIN_RX, SDS_PIN_TX);

  if(sds011.setDataReportingMode(DataReportingMode::active))
  {
    Serial.println("Success setting SDS011 Report Mode to Active.");
  }
  else
  {
    Serial.println("Fail setting SDS011 Report Mode to Active.");
  }

  DataReportingMode reportMode = sds011.getDataReportingMode();  
  Serial.println("Current SDS011 Report Mode is " + String(reportMode) + ".");
}

void loop()
{
  float p25, p10; 
  if(sds011.queryData(p25, p10) == QuerryError::no_error)
  {  
    Serial.println(String(millis()/1000) +"s:PM2.5=" + String(p25) + ", PM10=" + String(p10));  
  }
  delay(1000);
}
