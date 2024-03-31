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
#define SerialGPS Serial2
#define SerialRX Serial3


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



#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
#endif
TinyGsm        modem(SerialAT);

TinyGsmClient client(modem, 0);
HttpClient    http(client, server, port);
TinyGPSPlus gps;

void setup() {

  int start = millis();
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);



  DBG("Wait...");
  delay(2000);

  // Set GSM module baud rate
  // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(115200);
  SerialRX.begin(9600);
  SerialGPS.begin(9600);

 #if TINY_GSM_TEST_GPRS
    DBG("Initializing modem...");
    bool modem_running = modem.restart();
    while (!modem_running){
      // if (!modem.init()) {
      DBG("Failed to restart modem, delaying 10s and retrying");
      modem_running = modem.restart();
      // restart autobaud in case GSM just rebooted
      // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
    }

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

  
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }



    // bool network_connected = 
    DBG("Waiting for network...");
    if (!modem.waitForNetwork(600000L, true)) {
      delay(10000);
      return;
    }

    if (modem.isNetworkConnected()) { DBG("Network connected"); }

    DBG("Connecting to", apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      delay(10000);
      return;
    }

    bool res = modem.isGprsConnected();
    DBG("GPRS status:", res ? "connected" : "not connected");

    String ccid = modem.getSimCCID();
    DBG("CCID:", ccid);

    String imei = modem.getIMEI();
    DBG("IMEI:", imei);

    String imsi = modem.getIMSI();
    DBG("IMSI:", imsi);

    String cop = modem.getOperator();
    DBG("Operator:", cop);

    IPAddress local = modem.localIP();
    DBG("Local IP:", local);

    int csq = modem.getSignalQuality();
    DBG("Signal quality:", csq);

    // DBG("Enabling GPS/GNSS/GLONASS and waiting 15s for warm-up");
    // modem.enableGPS();
    // delay(15000L);

    DBG("Startup took:");
    DBG(millis()-start);

  #endif
}


void post(HttpClient& http, const String& data)
{
  // String postData = createRandData();
  SerialMon.print("Sendingddata: ");
  SerialMon.println(data);
  String contentType = "application/json";
  http.post(resource, contentType, data);

  // read the status code and body of the response
  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  

  // SerialMon.println(F("Response Headers:"));
  // while (http.headerAvailable()) {
  //   String headerName  = http.readHeaderName();
  //   String headerValue = http.readHeaderValue();
  //   SerialMon.println("    " + headerName + " : " + headerValue);
  // }

  // int length = http.contentLength();
  // if (length >= 0) {
  //   SerialMon.print(F("Content length is: "));
  //   SerialMon.println(length);
  // }
  // if (http.isResponseChunked()) {
  //   SerialMon.println(F("The response is chunked"));
  // }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

}


String gps_data = ""; 

void loop() {    
    
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());

    if (gps.location.isUpdated()) {
      gps_data = String(gps.location.lng(), 13)+","+ String(gps.location.lat(), 13);
    }
  }
  if (SerialRX.available()) {
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    StaticJsonDocument<300> data;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(data, SerialRX);

    if (err == DeserializationError::Ok) {
      data["gps"] = gps_data;
      // data["gps"] = gps_raw;
      // data["ts"] = millis()/1000;

      String payload;
      serializeJson(data, payload);
      // DBG(payload);
      post(http, payload);
      // client.stop();

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


  
}