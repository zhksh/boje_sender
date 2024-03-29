/**************************************************************
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * NOTE:
 * Some of the functions may be unavailable for your modem.
 * Just comment them out.
 *
 **************************************************************/

// Select your modem:
// #define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_SIM7000
// #define TINY_GSM_MODEM_SIM7000SSL
// #define TINY_GSM_MODEM_SIM7080
// #define TINY_GSM_MODEM_SIM5360
#define TINY_GSM_MODEM_SIM7600
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_SARAR4
// #define TINY_GSM_MODEM_M95
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_MC60
// #define TINY_GSM_MODEM_MC60E
// #define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_XBEE
// #define TINY_GSM_MODEM_SEQUANS_MONARCH

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1
#define SerialRX Serial3
#define SerialGPS Serial2


// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 115200
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

/*
 * Tests enabled
 */
#define TINY_GSM_TEST_GPRS true
#define TINY_GSM_TEST_WIFI false
#define TINY_GSM_TEST_TCP true
#define TINY_GSM_TEST_SSL true
#define TINY_GSM_TEST_CALL false
#define TINY_GSM_TEST_SMS false
#define TINY_GSM_TEST_USSD false
#define TINY_GSM_TEST_BATTERY true
#define TINY_GSM_TEST_TEMPERATURE true
#define TINY_GSM_TEST_GSM_LOCATION false
#define TINY_GSM_TEST_NTP false
#define TINY_GSM_TEST_TIME false
#define TINY_GSM_TEST_GPS true
// disconnect and power down modem after tests
#define TINY_GSM_POWERDOWN false

// set GSM PIN, if any
#define GSM_PIN ""

// Set phone numbers, if you want to test SMS and Calls
// #define SMS_TARGET  "+380xxxxxxxxx"
// #define CALL_TARGET "+380xxxxxxxxx"

// Your GPRS credentials, if any
const char apn[] = "YourAPN";
// const char apn[] = "ibasis.iot";
const char gprsUser[] = "";
const char gprsPass[] = "";


const char server[]   = "pgm-sensor-relay.ncmg.eu";
// const char server[]   = "116.202.237.199";
const char resource[] = "/api/post-sensor-data";
// const int port        = 52342;
const int port        = 80;
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>


#if TINY_GSM_TEST_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_TEST_GPRS
#undef TINY_GSM_TEST_WIFI
#define TINY_GSM_TEST_GPRS false
#define TINY_GSM_TEST_WIFI true
#endif
#if TINY_GSM_TEST_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif

TinyGsmClient client(modem, 0);
HttpClient    http(client, server, port);
TinyGPSPlus gps;

void setup() {

  int start = millis();
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  DBG("Wait...");
  delay(2000);

  // Set GSM module baud rate
  // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(115200);
  SerialRX.begin(9600);
  SerialGPS.begin(9600);

  DBG("Initializing modem...");
  bool modem_running = modem.restart();
  while (!modem_running){
    // if (!modem.init()) {
    DBG("Failed to restart modem, delaying 10s and retrying");
    modem_running = modem.restart();
    // restart autobaud in case GSM just rebooted
    // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  }

//   String name = modem.getModemName();
//   DBG("Modem Name:", name);

//   String modemInfo = modem.getModemInfo();
//   DBG("Modem Info:", modemInfo);

//   #if TINY_GSM_TEST_GPRS
//     // Unlock your SIM card with a PIN if needed
//     if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
//   #endif


// // #if TINY_GSM_TEST_GPRS && defined TINY_GSM_MODEM_XBEE
// //   // The XBee must run the gprsConnect function BEFORE waiting for network!
// //   modem.gprsConnect(apn, gprsUser, gprsPass);
// // #endif
//   // bool network_connected = 
//   DBG("Waiting for network...");
//   if (!modem.waitForNetwork(600000L, true)) {
//     delay(10000);
//     return;
//   }

//   if (modem.isNetworkConnected()) { DBG("Network connected"); }

//   #if TINY_GSM_TEST_GPRS
//     DBG("Connecting to", apn);
//     if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
//       delay(10000);
//       return;
//     }

//     bool res = modem.isGprsConnected();
//     DBG("GPRS status:", res ? "connected" : "not connected");

//     String ccid = modem.getSimCCID();
//     DBG("CCID:", ccid);

//     String imei = modem.getIMEI();
//     DBG("IMEI:", imei);

//     String imsi = modem.getIMSI();
//     DBG("IMSI:", imsi);

//     String cop = modem.getOperator();
//     DBG("Operator:", cop);

//     IPAddress local = modem.localIP();
//     DBG("Local IP:", local);

//     int csq = modem.getSignalQuality();
//     DBG("Signal quality:", csq);

    // DBG("Enabling GPS/GNSS/GLONASS and waiting 15s for warm-up");
    // modem.enableGPS();
    // delay(15000L);

    DBG("Startup took:");
    DBG(millis()-start);

  #endif
}

String createRandData(){
  return "{\"temp\": \"" + String(random(10, 200)) + " \", \"pressure\": \"" + String(random(5000, 30000)/100) + " \", \"depth\": \"" + String(random(0, 100)/10) + " \", \"env_dist\": \"" + String(random(0, 20)/10) + " \", \"gps\": \"12.39059306, 6.79417558\", \"ts\": \"" + String(millis()/1000) + "\"}";
}

void post(HttpClient& http, const String& data)
{
  SerialMon.println("making POST request");
  String contentType = "application/json";
  // String postData = createRandData();
  SerialMon.print("Data: ");
  SerialMon.println(data);
  http.post(resource, contentType, data);

  // read the status code and body of the response
  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  

  SerialMon.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName  = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialMon.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());
}




void loop() {
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  
      //  String gps_raw = modem.getGPSraw();

      // float lat2      = 0;
      // float lon2      = 0;
      // modem.getGPS(&lat2, &lon2);      
    
      while (SerialGPS.available() > 0) {
        gps.encode(SerialGPS.read());

        if (gps.location.isUpdated()) {
           DBG("GPS/GNSS Based Location String:");


          // Breitengrad mit 3 Nachkommastellen
          SerialMon.print("Breitengrad= ");
          SerialMon.print(gps.location.lat(), 8);

          // Längengrad mit 3 Nachkommastellen
          SerialMon.print(" Längengrad= ");
          SerialMon.println(gps.location.lng(), 8);
        }
      }
    return;
  if (SerialRX.available()) {
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    StaticJsonDocument<300> data;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(data, SerialRX);

    if (err == DeserializationError::Ok) {
      String gps_raw = modem.getGPSraw();
      DBG("GPS/GNSS Based Location String:", gps_raw);
      data["gps"] = "11.57960005323633, 48.12800085254037";
      data["gps"] = gps_raw;
      // data["ts"] = millis()/1000;

      String payload;
      serializeJson(data, payload);
      // DBG(payload);
      post(http, payload);
      // client.stop();

      // Serial.print("timestamp = ");
      // Serial.println(data["temp"].as<float>());
      // Serial.print("altitude = ");
      // Serial.println(data["altitude"].as<float>());
      // Serial.print("pressure = ");
      // Serial.println(data["pressure"].as<float>());
    } 
    else {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (SerialRX.available() > 0)
        SerialRX.read();
    }
  }
  // int c = 0;
  // while (true && c < 200){
  //   DBG("reading from serial");
  //   while (SerialRX.available() > 0) {
  //     String data = SerialRX.readString();
  //     DBG(data);
  //   post(http, "hi", c++);
  //   client.stop();
  // }




// #if TINY_GSM_TEST_GSM_LOCATION && defined TINY_GSM_MODEM_HAS_GSM_LOCATION
//   float lat      = 0;
//   float lon      = 0;
//   float accuracy = 0;
//   int   year     = 0;
//   int   month    = 0;
//   int   day      = 0;
//   int   hour     = 0;
//   int   min      = 0;
//   int   sec      = 0;
//   for (int8_t i = 15; i; i--) {
//     DBG("Requesting current GSM location");
//     if (modem.getGsmLocation(&lat, &lon, &accuracy, &year, &month, &day, &hour,
//                              &min, &sec)) {
//       DBG("Latitude:", String(lat, 8), "\tLongitude:", String(lon, 8));
//       DBG("Accuracy:", accuracy);
//       DBG("Year:", year, "\tMonth:", month, "\tDay:", day);
//       DBG("Hour:", hour, "\tMinute:", min, "\tSecond:", sec);
//       break;
//     } else {
//       DBG("Couldn't get GSM location, retrying in 15s.");
//       delay(15000L);
//     }
//   }
//   DBG("Retrieving GSM location again as a string");
//   String location = modem.getGsmLocation();
//   DBG("GSM Based Location String:", location);
// #endif

// #if TINY_GSM_TEST_GPS && defined TINY_GSM_MODEM_HAS_GPS
//   DBG("Enabling GPS/GNSS/GLONASS and waiting 15s for warm-up");
//   modem.enableGPS();
//   delay(15000L);
//   float lat2      = 0;
//   float lon2      = 0;
//   float speed2    = 0;
//   float alt2      = 0;
//   int   vsat2     = 0;
//   int   usat2     = 0;
//   float accuracy2 = 0;
//   int   year2     = 0;
//   int   month2    = 0;
//   int   day2      = 0;
//   int   hour2     = 0;
//   int   min2      = 0;
//   int   sec2      = 0;
//   for (int8_t i = 15; i; i--) {
//     DBG("Requesting current GPS/GNSS/GLONASS location");
//     if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
//                      &year2, &month2, &day2, &hour2, &min2, &sec2)) {
//       DBG("Latitude:", String(lat2, 8), "\tLongitude:", String(lon2, 8));
//       DBG("Speed:", speed2, "\tAltitude:", alt2);
//       DBG("Visible Satellites:", vsat2, "\tUsed Satellites:", usat2);
//       DBG("Accuracy:", accuracy2);
//       DBG("Year:", year2, "\tMonth:", month2, "\tDay:", day2);
//       DBG("Hour:", hour2, "\tMinute:", min2, "\tSecond:", sec2);
//       break;
//     } else {
//       DBG("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
//       delay(15000L);
//     }
//   }
//   DBG("Retrieving GPS/GNSS/GLONASS location again as a string");
//   String gps_raw = modem.getGPSraw();
//   DBG("GPS/GNSS Based Location String:", gps_raw);
//   DBG("Disabling GPS");
//   modem.disableGPS();
// #endif



// #if TINY_GSM_POWERDOWN

// #if TINY_GSM_TEST_GPRS
//   modem.gprsDisconnect();
//   delay(5000L);
//   if (!modem.isGprsConnected()) {
//     DBG("GPRS disconnected");
//   } else {
//     DBG("GPRS disconnect: Failed.");
//   }
// #endif

// #if TINY_GSM_TEST_WIFI
//   modem.networkDisconnect();
//   DBG("WiFi disconnected");
// #endif

//   // Try to power-off (modem may decide to restart automatically)
//   // To turn off modem completely, please use Reset/Enable pins
//   modem.poweroff();
//   DBG("Poweroff.");
// #endif

//   DBG("End of tests.");

  // Do nothing forevermore
  // while (true) { modem.maintain(); }

  
}