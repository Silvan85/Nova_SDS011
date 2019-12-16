#define TS_DELAY 60 * 100
#include <NovaSDS011.h>

bool is_SDS_running = true;

#define SDS_PIN_RX 2
#define SDS_PIN_TX 3

NovaSDS011 sds011;

void setup()
{
  Serial.begin(115200); // Output to Serial at 9600 baud
  sds011.begin(SDS_PIN_RX, SDS_PIN_TX);
  sds011.setDutyCycle(1);
}

void loop()
{
  float p25, p10; 
  int error = sds011.read(&p25, &p10);
  if(!error)
  {  
    Serial.println("PM2.5=" + String(p25) + ", PM10=" + String(p10));  
  }
  delay(1000);
}
