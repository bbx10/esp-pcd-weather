/****************************
The MIT License (MIT)

Copyright (c) 2015 by bbx10node@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************/

#include <ESP8266WiFi.h>
#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif
#ifndef max
#define max(x,y) (((x)>(y))?(x):(y))
#endif
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

static const char SSID[]     = "***********";
static const char PASSWORD[] = "************";

// Use your own API key by signing up for a free developer account.
// http://www.wunderground.com/weather/api/
#define WU_API_KEY "****************"

// Specify your favorite location one of these ways.
//#define WU_LOCATION "CA/San_Francisco"

// US ZIP code
//#define WU_LOCATION "90210"

// Country and city
#define WU_LOCATION "Australia/Sydney"

/******************************************************************
ESP8266 with PCD8544 display

== Parts ==

* Adafruit Huzzah ESP8266 https://www.adafruit.com/products/2471

* Adafruit PCD8544/5110 display https://www.adafruit.com/product/338

* Adafruit USB to TTL serial cable https://www.adafruit.com/products/954

== Connection ==

USB TTL     Huzzah      Nokia 5110  Description
            ESP8266     PCD8544

            GND         GND         Ground
            3V          VCC         3.3V from Huzzah to display
            14          CLK         Output from ESP SPI clock
            13          DIN         Output from ESP SPI MOSI to display data input
            12          D/C         Output from display data/command to ESP
            #5          CS          Output from ESP to chip select/enable display
            #4          RST         Output from ESP to reset display
            15          LED         3.3V to turn backlight on, GND off

GND (blk)   GND                     Ground
5V  (red)   V+                      5V power from PC or charger
TX  (green) RX                      Serial data from IDE to ESP
RX  (white) TX                      Serial data to ESP from IDE
******************************************************************/

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 14 on Huzzah ESP8266
// MOSI is LCD DIN - this is pin 13 on an Huzzah ESP8266
// pin 12 - Data/Command select (D/C) on an Huzzah ESP8266
// pin 5 - LCD chip select (CS)
// pin 4 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(12, 5, 4);

// 5 minutes between update checks. The free developer account has a limit
// on the  number of calls so don't go wild.
#define DELAY_NORMAL    (5*60*1000)
// 20 minute delay between updates after an error
#define DELAY_ERROR     (20*60*1000)

/***********************************************************
 *
 * Sample requests and responses
 *
http://api.wunderground.com/api/c596b7af83bae426/conditions/q/CA/San_Francisco.json

{
  "response": {
    ...don't care
    }
  },
  "current_observation": {
    "image": {
    ...don't care
    },
    "display_location": {
    ...don't care
    },
    "observation_location": {
    ...don't care
    },
    "estimated": {},
    "station_id": "KCASANFR58",
    "observation_time": "Last Updated on June 27, 5:27 PM PDT",
    "observation_time_rfc822": "Wed, 27 Jun 2012 17:27:13 -0700",
    "observation_epoch": "1340843233",
    "local_time_rfc822": "Wed, 27 Jun 2012 17:27:14 -0700",
    "local_epoch": "1340843234",
    "local_tz_short": "PDT",
    "local_tz_long": "America/Los_Angeles",
    "local_tz_offset": "-0700",
    "weather": "Partly Cloudy",
    "temperature_string": "66.3 F (19.1 C)",
    "temp_f": 66.3,
    "temp_c": 19.1,
    "relative_humidity": "65%",
    "wind_string": "From the NNW at 22.0 MPH Gusting to 28.0 MPH",
    "wind_dir": "NNW",
    "wind_degrees": 346,
    "wind_mph": 22.0,
    "wind_gust_mph": "28.0",
    "wind_kph": 35.4,
    "wind_gust_kph": "45.1",
    "pressure_mb": "1013",
    "pressure_in": "29.93",
    "pressure_trend": "+",
    "dewpoint_string": "54 F (12 C)",
    "dewpoint_f": 54,
    "dewpoint_c": 12,
    "heat_index_string": "NA",
    "heat_index_f": "NA",
    "heat_index_c": "NA",
    "windchill_string": "NA",
    "windchill_f": "NA",
    "windchill_c": "NA",
    "feelslike_string": "66.3 F (19.1 C)",
    "feelslike_f": "66.3",
    "feelslike_c": "19.1",
    "visibility_mi": "10.0",
    "visibility_km": "16.1",
    "solarradiation": "",
    "UV": "5",
    "precip_1hr_string": "0.00 in ( 0 mm)",
    "precip_1hr_in": "0.00",
    "precip_1hr_metric": " 0",
    "precip_today_string": "0.00 in (0 mm)",
    "precip_today_in": "0.00",
    "precip_today_metric": "0",
    "icon": "partlycloudy",
    "icon_url": "http://icons-ak.wxug.com/i/c/k/partlycloudy.gif",
    "forecast_url": "http://www.wunderground.com/US/CA/San_Francisco.html",
    "history_url": "http://www.wunderground.com/history/airport/KCASANFR58/2012/6/27/DailyHistory.html",
    "ob_url": "http://www.wunderground.com/cgi-bin/findweather/getForecast?query=37.773285,-122.417725"
    }
}

TODO alerts
http://api.wunderground.com/api/c596b7af83bae426/alerts/q/IA/Des_Moines.json

{
  "response": {
  "version": "0.1",
  "termsofService": "http://www.wunderground.com/weather/api/d/terms.html",
  "features": {
  "alerts": 1
  }
  },
  "alerts": [{
  "type": "HEA",
  "description": "Heat Advisory",
  "date": "11:14 am CDT on July 3, 2012",
  "date_epoch": "1341332040",
  "expires": "7:00 AM CDT on July 07, 2012",
  "expires_epoch": "1341662400",
  "message": "\u000A...Heat advisory remains in effect until 7 am CDT Saturday...\u000A\u000A* temperature...heat indices of 100 to 105 are expected each \u000A afternoon...as Max temperatures climb into the mid to upper \u000A 90s...combined with dewpoints in the mid 60s to around 70. \u000A Heat indices will remain in the 75 to 80 degree range at \u000A night. \u000A\u000A* Impacts...the hot and humid weather will lead to an increased \u000A risk of heat related stress and illnesses. \u000A\u000APrecautionary/preparedness actions...\u000A\u000AA heat advisory means that a period of hot temperatures is\u000Aexpected. The combination of hot temperatures and high humidity\u000Awill combine to create a situation in which heat illnesses are\u000Apossible. Drink plenty of fluids...stay in an air-conditioned\u000Aroom...stay out of the sun...and check up on relatives...pets...\u000Aneighbors...and livestock.\u000A\u000ATake extra precautions if you work or spend time outside. Know\u000Athe signs and symptoms of heat exhaustion and heat stroke. Anyone\u000Aovercome by heat should be moved to a cool and shaded location.\u000AHeat stroke is an emergency...call 9 1 1.\u000A\u000A\u000A\u000AMjb\u000A\u000A\u000A",
  "phenomena": "HT",
  "significance": "Y",
  "ZONES": [
  {
  "state":"UT",
  "ZONE":"001"
  }
  ],
  "StormBased": {
  "vertices":[
  {
  "lat":"38.87",
  "lon":"-87.13"
  }
  ,
  {
  "lat":"38.89",
  "lon":"-87.13"
  }
  ,
  {
  "lat":"38.91",
  "lon":"-87.11"
  }
  ,
  {
  "lat":"38.98",
  "lon":"-86.93"
  }
  ,
  {
  "lat":"38.87",
  "lon":"-86.69"
  }
  ,
  {
  "lat":"38.75",
  "lon":"-86.3"
  }
  ,
  {
  "lat":"38.84",
  "lon":"-87.16"
  }
  ],
  "Vertex_count":7,
  "stormInfo": {
  "time_epoch": 1363464360,
  "Motion_deg": 243,
  "Motion_spd": 18,
  "position_lat":38.90,
  "position_lon":-86.96
  }
  }
  }]
}
*************************************************************/

#define WUNDERGROUND "api.wunderground.com"

// HTTP request
const char WUNDERGROUND_REQ[] =
    "GET /api/" WU_API_KEY "/conditions/q/" WU_LOCATION ".json HTTP/1.1\r\n"
    "User-Agent: ESP8266/0.1\r\n"
    "Accept: */*\r\n"
    "Host: " WUNDERGROUND "\r\n"
    "Connection: close\r\n"
    "\r\n";

void setup()
{
  Serial.begin(115200);

  // Turn LCD backlight on
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  // TODO Reconnect to AP, if disconnected

  // Initialize LCD
  display.begin();
  display.setContrast(50);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(BLACK);

  // Give WU credit.
  display.clearDisplay();
  display.println(F("  Powered by"));
  display.println(F("   Weather"));
  display.println(F("  Underground"));
  display.display();

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

static char respBuf[4096];

bool showWeather(char *json);

void loop()
{
  // TODO check for disconnect from AP

  // Open socket to WU server port 80
  Serial.print(F("Connecting to "));
  Serial.println(WUNDERGROUND);

  // Use WiFiClient class to create TCP connections
  WiFiClient httpclient;
  const int httpPort = 80;
  if (!httpclient.connect(WUNDERGROUND, httpPort)) {
    Serial.println(F("connection failed"));
    delay(DELAY_ERROR);
    return;
  }

  // This will send the http request to the server
  Serial.print(WUNDERGROUND_REQ);
  httpclient.print(WUNDERGROUND_REQ);
  httpclient.flush();

  // Collect http response headers and content from Weather Underground
  // HTTP headers are discarded.
  // The content is formatted in JSON and is left in respBuf.
  int respLen = 0;
  bool skip_headers = true;
  while (httpclient.connected() || httpclient.available()) {
    if (skip_headers) {
      String aLine = httpclient.readStringUntil('\n');
      //Serial.println(aLine);
      // Blank line denotes end of headers
      if (aLine.length() <= 1) {
        skip_headers = false;
      }
    }
    else {
      int bytesIn;
      bytesIn = httpclient.read((uint8_t *)&respBuf[respLen], sizeof(respBuf) - respLen);
      Serial.print(F("bytesIn ")); Serial.println(bytesIn);
      if (bytesIn > 0) {
        respLen += bytesIn;
        if (respLen > sizeof(respBuf)) respLen = sizeof(respBuf);
      }
      else if (bytesIn < 0) {
        Serial.print(F("read error "));
        Serial.println(bytesIn);
      }
    }
    delay(1);
  }
  httpclient.stop();

  if (respLen >= sizeof(respBuf)) {
    Serial.print(F("respBuf overflow "));
    Serial.println(respLen);
    delay(DELAY_ERROR);
    return;
  }
  // Terminate the C string
  respBuf[respLen++] = '\0';
  Serial.print(F("respLen "));
  Serial.println(respLen);
  //Serial.println(respBuf);

  if (showWeather(respBuf)) {
    delay(DELAY_NORMAL);
  }
  else {
    delay(DELAY_ERROR);
  }
}

bool showWeather(char *json)
{
  StaticJsonBuffer<3*1024> jsonBuffer;

  // Skip characters until first '{' found
  // Ignore chunked length, if present
  char *jsonstart = strchr(json, '{');
  //Serial.print(F("jsonstart ")); Serial.println(jsonstart);
  if (jsonstart == NULL) {
    Serial.println(F("JSON data missing"));
    return false;
  }
  json = jsonstart;

  // Parse JSON
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }

  // Extract weather info from parsed JSON
  JsonObject& current = root["current_observation"];
  const float temp_f = current["temp_f"];
  Serial.print(temp_f, 1); Serial.print(F(" F, "));
  const float temp_c = current["temp_c"];
  Serial.print(temp_c, 1); Serial.print(F(" C, "));
  const char *humi = current[F("relative_humidity")];
  Serial.print(humi);   Serial.println(F(" RH"));
  const char *weather = current["weather"];
  Serial.println(weather);
  const char *pressure_mb = current["pressure_mb"];
  Serial.println(pressure_mb);
  const char *observation_time = current["observation_time_rfc822"];
  Serial.println(observation_time);
  //Date/time string looks like this
  //Wed, 27 Jun 2012 17:27:14 -0700
  //012345678901234567890
  //          1         2
  // LCD has 14 characters per line so show date on one line and time on the
  // next. And shorten the date so it fits.
  char date[14+1];
  const char *time;
  //Wed, 27 Jun 12
  memcpy(date, observation_time, 12);
  memcpy(&date[12], &observation_time[14], 2);
  date[14] = '\0';
  //17:27:14 -0700
  time = &observation_time[17];

  display.clearDisplay();
  display.println(date);
  display.println(time);
  display.print(temp_f, 1); display.print  (F(" F "));
  display.print(temp_c, 1); display.println(F(" C"));
  display.print(humi);      display.print(F(" RH "));
  display.print(pressure_mb); display.println(F("mb"));
  display.setTextWrap(true);
  display.print(weather);
  display.display();
  display.setTextWrap(false);
  return true;
}
