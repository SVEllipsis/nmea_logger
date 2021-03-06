// Libraries required
#include <SPI.h>
#include <Ethernet.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// SeaTalk connection
static const int RXPin = 6, TXPin = 7;
static const uint32_t GPSBaud = 4800;
SoftwareSerial ss(RXPin, TXPin);

//Configure Ethernet and declare a web server
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(80);

// The TinyGPS++ object and custom handlers
TinyGPSPlus gps;
TinyGPSCustom waterTemp(gps, "VWMTW", 1);
TinyGPSCustom waterSpeed(gps, "VWVHW", 5);
TinyGPSCustom windDirection(gps, "ECMWV", 1);
TinyGPSCustom windSpeed(gps, "ECMWV", 3);

// For stats that happen every 5 seconds
unsigned long last = 0UL;

void setup()
{
  Serial.begin(9600);

  //Start GPS connection
  ss.begin(GPSBaud);

  // start the web server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());


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

  listenForEthernetClients();
}

void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/text;charset=utf-8");
          client.println("Server: raymarine");
          client.println("Connnection: close");
          client.println();
          
          client.print(gps.location.lat(), 6);
          client.print("|");
          client.print(gps.location.lng(), 6);
          client.print("|");
          client.print(gps.date.value());
          client.print("|");
          client.print(gps.time.value());
          client.print("|");
          client.print(gps.speed.knots());
          client.print("|");
          client.print(gps.course.deg());
          client.print("|");
          client.print(waterTemp.value());
          client.print("|");
          client.print(waterSpeed.value());
          client.print("|");
          client.print(windDirection.value());
          client.print("|");
          client.print(windSpeed.value());          
         
          client.println();

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
