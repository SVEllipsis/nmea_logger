#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   This sample code demonstrates just about every built-in operation of TinyGPS++ (TinyGPSPlus).
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;

// The TinyGPS++ object
TinyGPSPlus gps;
TinyGPSCustom waterTemp(gps, "VWMTW", 1);
TinyGPSCustom waterSpeed(gps, "VWVHW", 5);
TinyGPSCustom windDirection(gps, "ECMWV", 1);
TinyGPSCustom windSpeed(gps, "ECMWV", 3);

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// For stats that happen every 5 seconds
unsigned long last = 0UL;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("Ellipsis Connected Boat Platform v0.1"));
  Serial.print(F("Requiring TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
}

void loop()
{
  // Dispatch incoming characters
  while (ss.available() > 0)
    gps.encode(ss.read());

  if (gps.location.isUpdated())
  { 
    Serial.print("LATA=");
    Serial.println(gps.location.lat(), 6);
    Serial.print(F("LONG="));
    Serial.println(gps.location.lng(), 6);      
   }

  else if (gps.date.isUpdated())
  {
    Serial.print("DATE=");
    Serial.println(gps.date.value());
  }

  else if (gps.time.isUpdated())
  {
    Serial.print("TIME=");
    Serial.println(gps.time.value());
  }

  else if (gps.speed.isUpdated())
  {
    Serial.print(F("KNOT="));
    Serial.println(gps.speed.knots());
  }

  else if (gps.course.isUpdated())
  {
    Serial.print(F("BEAR="));
    Serial.println(gps.course.deg());
  }

  else if (waterTemp.isUpdated())
  {
    Serial.print(F("WATT="));
    Serial.println(waterTemp.value());
  }

  else if (waterSpeed.isUpdated())
  {
    Serial.print(F("WATS="));
    Serial.println(waterSpeed.value());
  }  
  
  else if (windDirection.isUpdated())
  {
    Serial.print(F("WIND="));
    Serial.println(windDirection.value());
  }  
  
  else if (windSpeed.isUpdated())
  {
    Serial.print(F("WINS="));
    Serial.println(windSpeed.value());
  }  

  else if (millis() - last > 5000)
  {
    if (gps.charsProcessed() < 10)
      Serial.println(F("WARNING: No GPS data.  Check wiring."));
    last = millis();
    Serial.println();
  }
}
