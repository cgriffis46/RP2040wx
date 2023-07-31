#define wx_version String("00.00.20");

#define USE_RP2040_PICO

#ifdef USE_RP2040_PICO
#include "hardware/irq.h"
#include "hardware/rtc.h"
#include "hardware/gpio.h"
//#include "hardware/stdio.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "pico/stdio.h"
#include "hardware/watchdog.h"
#endif

#include <Wire.h>

#include <stdlib.h>
#include <stdio.h>

static void readSensors();

extern TwoWire Wire1;

#include <Ticker.h>
#include <HTTPClient.h>
#include <base64.h>

#define USE_I2C_EEPROM true
#define USE_SHT31  true
#define USE_MPL3115A2 true
#define _USE_DS3231 true

#ifdef USE_I2C_FRAM
  #include <Adafruit_FRAM_I2C.h>
#endif

#ifdef USE_I2C_EEPROM
  #include <Adafruit_EEPROM_I2C.h>
  Adafruit_EEPROM_I2C fram = Adafruit_EEPROM_I2C();
#endif

#ifdef USE_SPI_FRAM
  #include <Adafruit_FRAM_SPI.h>
  uint8_t FRAM_SCK = 14;
  uint8_t FRAM_MISO = 12;
  uint8_t FRAM_MOSI = 13;
  uint8_t FRAM_CS = 15;
  Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_SCK, FRAM_MISO, FRAM_MOSI, FRAM_CS);
#endif

#define mem_SSID (uint32_t)0x0000
#define mem_PASSWORD (uint32_t)0x0025

#define mem_DEV_USER (uint32_t)0x0050
#define mem_DEV_PASSWORD (uint32_t)0x0075

#define mem_SECURTIY_TYPE 
#define mem_DNS_SERVER (uint32_t)0x0200
#define mem_NTP_SERVER (uint32_t)0x0240
#define mem_DEFAULT_DNS_SERVER
#define mem_DEFAULT_NTP_SERVER

#define mem_PRESSURE_OFFSET (uint32_t)0x0400
#define mem_BAR_ENABLE (uint32_t)0x0406
#define mem_INFCE_THERMOMETER_ENABLE (uint32_t)0x0407
#define mem_INFCE_HUMIDITY_ENABLE (uint32_t)0x0408
#define mem_INFCE_THERMOMETER_UNIT (uint32_t)0x0409

#define mem_WUNDERGROUND_ENABLE (uint32_t)0x0100
#define mem_WUNDERGROUND_TEMP_ID (uint32_t)0x0101
#define mem_WUNDERGROUND_HUMIDITY_ID (uint32_t)0x0102
#define mem_WUNDERGROUND_TIME_SOURCE (uint32_t)0x0103

#define mem_WUNDERGROUNDID (uint32_t)0x0125
#define mem_WUNDERGROUNDPASSWORD (uint32_t)0x0150

#define WundergroundStationIDLength 64
#define WundergroundStationIDPassword 64

// For wx stations using the MPL3115A2 barometric pressure sensor 
#ifdef USE_MPL3115A2
  #include <Adafruit_MPL3115A2.h>
  #ifndef USE_BAROMETRIC_PRESSURE_SENSOR
    #define USE_BAROMETRIC_PRESSURE_SENSOR true
  #endif
  #ifndef _INFCE_SEND_BAROMETRIC_PRESSURE
    #define _INFCE_SEND_BAROMETRIC_PRESSURE true
  #endif
  Adafruit_MPL3115A2  mpl3115a2;
#endif
#ifdef USE_BMP280
  #include <Adafruit_BMP280.h>
  #ifndef USE_BAROMETRIC_PRESSURE_SENSOR
    #define USE_BAROMETRIC_PRESSURE_SENSOR true
  #endif
  #ifndef _INFCE_SEND_BAROMETRIC_PRESSURE
    #define _INFCE_SEND_BAROMETRIC_PRESSURE true
  #endif
  // GPIO Pin Definitions
  #define BMP_SCK  (6)
  #define BMP_MISO (8)
  #define BMP_MOSI (7)
  #define BMP_CS   (9)
  Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
#endif
#ifdef USE_BAROMETRIC_PRESSURE_SENSOR
//  void updatePressureSensorHandler();
//  Ticker PressureSensorTicker(updatePressureSensorHandler,2,0);
static void ReadPressureSensor();
  bool QueueBarometerForInterfaces = false;
  float pressure, PressureOffset;
  float pressureInHg;
  float externalPressureOffset; 
#endif

#ifdef USE_SHT31
  #include <Adafruit_SHT31.h>
  Adafruit_SHT31 sht31 = Adafruit_SHT31(&Wire1);
  bool enableHeater = false;
  uint8_t loopCnt = 0;
  #ifndef _USE_TH_SENSOR
    #define _USE_TH_SENSOR
  #endif
  #ifndef _INFCE_SEND_TEMP_HUMIDITY
   #define _INFCE_SEND_TEMP_HUMIDITY
  #endif
#endif

#ifdef _USE_DS3231
  #include <RTClib.h>
  RTC_DS3231 rtc;
  #ifndef _USE_RTC
    #define _USE_RTC true
  #endif
#endif

#ifdef _USE_TH_SENSOR
static void readTempHumiditySensor();
//  Ticker readTHSensorTicker(readTempHumiditySensor,2,0);
  float temperature, humidity;
  float tempf, tempc;
  bool QueueThermometerForInterfaces = false;
  bool QueueHumidityForInterfaces = false;
  bool UseCelcius = false;
#endif

#include <NTPClient.h>

#include <ESP8266WebServer.h>
#include <HTTPServer.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>
#include <WebServerSecure.h>
#include <WebServerTemplate.h>
#include <Dns.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <ESP8266mDNS.h>
#include <dummy_rp2040.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiAP.h>
//#include <ESP8266WiFiGeneric.h>
//#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
//#include <ESP8266WiFiSTA.h>
//#include <ESP8266WiFiScan.h>
//#include <ESP8266WiFiType.h>

#define pgmStateIdle 0
#define pgmStateSetup 1
#define pgmStateWifiConnect 10
#define pgmStateWaitForConnect 15
#define pgmStateStartNTPService 19
#define pgmStateNTPUpdate 20
#define pgmStateCheckWifi 25
#define pgmStateReadClock 30
#define waitForCredentials 14
#define pgmStateDoWork 99
#define pgmStateConnectAdafruitIO 35
#define pgmStateWaitForAIO 36

void ShouldUpdateWundergroundInterfaceTicker();

/* Soft AP network parameters */
IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);
/* Set these to your desired softAP credentials. They are not configurable at runtime */
#ifndef APSSID
  #define APSSID "RP2040wx"
  #define APPSK "12345678"
#endif

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

#define WIFI_SSID "ssid"
#define WIFI_PASS "password"

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

#define SSID_LENGTH 32
#define PASSWORD_LENGTH 32
#define USER_LENGTH 32

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[SSID_LENGTH] = "";
char password[PASSWORD_LENGTH] = "";

// user and password we need to configure the device
char dev_user[USER_LENGTH] = "user";
char dev_password[PASSWORD_LENGTH] = "password";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server;

// authentication 
const char* www_username = "admin";
const char* www_password = "esp8266";
// allows you to set the realm of authentication Default:"Login Required"
const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
String authFailResponse = "Authentication Failed";

u_int pgmState = pgmStateSetup;

boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;
bool DoNotReconnectWifi = false;
uint8_t ConnectionAttempts = 0;

IPAddress wundergroundIP(0,0,0,0);
const char *url = "weatherstation.wunderground.com";

DateTime now;
DateTime ntptime;
WiFiUDP wifiUdp;
NTPClient timeClient(wifiUdp);

// Wunderground interface 
#define USE_WUNDERGROUND_INFCE

#ifdef USE_WUNDERGROUND_INFCE 
  bool shouldUpdateWundergroundInfce = false;
  bool WundergroundInfceEnable = false;
  char WundergroundStationID[WundergroundStationIDLength] = "";
  char WundergroundStationPassword[WundergroundStationIDPassword] = "";

  uint8_t thermometer1Type = 1;
  uint8_t humidity1_sensor_type = 1;
  uint8_t WundergroundTimeSource = 1;

  Ticker updateWundergroundTicker(ShouldUpdateWundergroundInterfaceTicker,5,0);

  String action_str = "&action=updateraw";
  String W_Software_Type = "&softwaretype=rp2040wx%20version"+wx_version;

  HTTPClient httpWunderground;
  WiFiClient client;

#endif

#define USE_CWOP_REST

#ifdef USE_CWOP_REST

  #define CWOPIDLENGTH 32

  bool shouldUpdateCWOPInfce = false;
  bool EnableCWOPRestInfce = false;
  String CWOPRestURL;
  char CWOPID[CWOPIDLENGTH] = "";
  char CWOPGPSLAT[CWOPIDLENGTH] = "";
  char CWOPGPSLONG[CWOPIDLENGTH] = "";

  #define mem_CWOP_ID (uint32_t)0x0500
  #define mem_CWOP_GPS_LAT (uint32_t)0x0525
  #define mem_CWOP_GPS_LON (uint32_t)0x0550

#endif

void setup() {
  WiFiMode_t mode;  
  Serial.begin(115200);
  // 
  if(watchdog_caused_reboot()){
    Serial.println("Watchdog caused reboot!!!!");
  }else if(watchdog_enable_caused_reboot()){
    Serial.println("Watchdog Enable caused reboot!!!!");
  }
  // Initialize NVRAM
  Serial.println("Initialize NVRAM");
  if(fram.begin(0x50)){
     loadCredentials();            // Load WLAN credentials from network
     LoadSensorsFromDisk();
     LoadWundergroundCredentials();// Load Wunderground Interface credentials
  }
  else{
    Serial.println("could not initialize fram");
  }
  // Set the internal RTC using an external RTC
  #ifdef _USE_RTC
    Serial.println("Initialize RTC");
    if(rtc.begin(&Wire)){
      #ifdef _USE_DS3231
        rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
        rtc.enable32K();
      #endif
      now = rtc.now();

    datetime_t t = {
            .year  = now.year(),
            .month = now.month(),
            .day   = now.day(),
            .dotw  = now.dayOfTheWeek(), // 0 is Sunday, so 5 is Friday
            .hour  = now.hour(),
            .min   = now.minute(),
            .sec   = now.second()
    };
    // Set the RP2040 internal RTC from the External RTC 
    rtc_set_datetime(&t);
  }
  else {
    Serial.println("Could not initialize RTC!");
  }

   #ifdef _USE_DS3231 // configure system clocks to use ds3231 outputs
    // Set the RP2040 clk_ref to use the 32khz from the DS3231
    // Set the RP2040 clk_rtc to use the 1hz from the DS3231
    // The 32KHz can be used a tick source or divided to 1Hz for clk_rtc
    clock_stop(clk_ref);// Stop clk_ref to save power. DS3231 will provide external clock. 
    clock_stop(clk_rtc);// clk_rtc is derived from clk_ref. We will use DS3231 1hz. 
    gpio_set_function(20,GPIO_FUNC_GPCK); // configure rp2040 GPIO pin 20 for external clock
    gpio_set_function(22,GPIO_FUNC_GPCK); // configure rp2040 GPIO pin 22 for external clock
    gpio_pull_up(20); // Configure pullups if necessary 
    gpio_pull_up(22); // Configure pullups if necessary 
    clock_configure_gpin(clk_ref,20,32768,32768); // configure clk_ref to use external 32khz clock on pin 20
    clock_configure_gpin(clk_rtc,22,1,1); // configure clk_rtc to use external 1hz clock on pin 22 
    rtc_hw->clkdiv_m1 = 0; // configure rtc clock divider for 1hz
    clocks_init(); // restart the clocks
   #endif // configure clocks 
  #endif // configure RTC 

    // start the system RTC
    rtc_init();

  // initialize watchdog timer
  
  connect = strlen(ssid)>0;  // Request WLAN connect if there is a SSID
  ConnectionAttempts = 0;

  // Start NTP 
  timeClient.begin();

  if(connect){
    Serial.println(ssid);
  }

  // we don't have a wifi SSID yet. Open captive portal to obtain credentials. 
  if(!connect){
    Serial.println("No SSID found. Starting AP");

    WiFi.mode(WIFI_AP_STA);
    // setup AP for captive portal configuration
    WiFi.softAPConfig(apIP, apIP, netMsk);
    softAP_password = dev_password;

    WiFi.softAP(softAP_ssid, softAP_password);

    delay(500);  // Without delay I've seen the IP address blank
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());}

/* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  
  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.on("/",handleRoot);
 server.on("/config",handleConfig);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/weather",handleWeather);
  server.on("/sensors",HandleSensors);
  server.on("/SaveSensors",SaveSensors);
  server.on("/SaveThermometer",SaveThermometer);
  server.on("/SaveHumidity",SaveHumidity);
  server.on("/savedevicecredentials",SaveDeviceCredentials);
//  server.on("/networksave",networksave);
  server.on("/wundergroundsave",handleWundergroundConfigSave);
  //server.on("/generate_204", handleRoot);  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
  //server.on("/fwlink", handleRoot);        // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.onNotFound(handleNotFound);
  server.on("/restoredefaults",RestoreDefaults);
  server.begin();  // Web server start
  Serial.println("HTTP server started");

  // if ready to connect to wifi, try connecting
  if(connect){
    connectWifi();
    pgmState = pgmStateWaitForConnect;
  }
  else{ // if we don't have wifi credentials, wait for credentials
    pgmState = waitForCredentials;
  }
  mode = WiFi.getMode();
  if(mode==WIFI_AP_STA){
    Serial.print("Mode: ");Serial.println(mode);
  }
  else if(mode==WIFI_AP){
    Serial.print("Mode: ");Serial.println(mode);
  }
  else if(mode==WIFI_STA){
    Serial.print("Mode: ");Serial.println(mode);
  }
  else if(mode==WIFI_OFF){
    Serial.print("Mode: ");Serial.println(mode);
  }
  else{
    Serial.print("Mode: ");Serial.println(mode);
  }

  //watchdog_start_tick(1);
  watchdog_enable(100000,1);

}

void loop() {
  unsigned int s = WiFi.status();
  watchdog_update();
 // Serial.print("RTC frequency: ");Serial.println(frequency_count_khz(clk_ref));
  //now = rtc.now();
  //rtc_get_datetime(&now);

  if (WiFi.status() == WL_CONNECTED) { MDNS.update(); } else { pgmState = pgmStateWifiConnect; }
  dnsServer.processNextRequest();
  server.handleClient();

   // timeClient must be called every loop to update NTP time 
      if(timeClient.update()) {
          if(timeClient.isTimeSet()){
            // adjust the external RTC
            #ifdef _USE_RTC
              rtc.adjust(now = DateTime(timeClient.getEpochTime()));
            #endif
            #ifndef _USE_RTC
              now = DateTime(timeClient.getEpochTime());
            #endif
            datetime_t t = {
              .year  = now.year(),
              .month = now.month(),
              .day   = now.day(),
              .dotw  = now.dayOfTheWeek(), // 0 is Sunday, so 5 is Friday
              .hour  = now.hour(),
              .min   = now.minute(),
              .sec   = now.second()
              };
            rtc_set_datetime(&t);
            Serial.println("Updated RTC time!");}

      }
      else {
      //          Serial.println("Could not update NTP time!");
      }

  switch (pgmState) {

    case waitForCredentials: // Obtain wifi credentials to connect to wifi
    {
      if(WiFi.status()==WL_CONNECTED){
        pgmState = pgmStateDoWork;
      }
      else{
    // user will put credentials on captive portal. ssid should not exist until user enters the ssid 
    if(connect = strlen(ssid) > 0){
      Serial.print("ssid:");
      Serial.println(ssid);
      pgmState = pgmStateWifiConnect;
    }  // Request WLAN connect if there is a SSID
    }
    break;
  }
  case pgmStateWifiConnect: // Try connecting to wifi
  {
    if(!DoNotReconnectWifi){
      if(WiFi.status()==WL_CONNECTED&(ssid!=WiFi.SSID())&(strlen(ssid)>0)) { // SSID changed. Connect to a different SSID. 
        WiFi.disconnect(false);
        connectWifi();
        pgmState = pgmStateWaitForConnect;
        break;
      }
      else if(WiFi.status()==WL_CONNECTED){ // Connected. 
        ConnectionAttempts = 0;
        pgmState = pgmStateDoWork;
      }
      else if(s==WL_CONNECT_FAILED){
          ConnectionAttempts++;
          if(ConnectionAttempts<5){
            connectWifi();
          }
          else{
                memset(ssid,0,sizeof(ssid));
                WiFi.mode(WIFI_AP_STA);
                // setup AP for captive portal configuration
                WiFi.softAPConfig(apIP, apIP, netMsk);

                WiFi.softAP(softAP_ssid, softAP_password);

                delay(500);  // Without delay I've seen the IP address blank
                Serial.print("AP IP address: ");
                Serial.println(WiFi.softAPIP());
                pgmState = waitForCredentials;
                return;
                }
          
//        WiFi.reconnect();
      }
      else if(s==WL_DISCONNECTED){
          connectWifi();
      }
      else if(WiFi.status()==WL_IDLE_STATUS){ // connecting 
        pgmState = pgmStateWaitForConnect;
      }
      else if(strlen(ssid) > 0){  // we have an SSID, try connecting to it. 
            connectWifi();
            lastConnectTry = millis();
            pgmState = pgmStateWaitForConnect;
            }
      else // Wait for SSID credentials. 
      {
        connect = false;
       // WiFi.reconnect();
//        connectWifi();
        pgmState = waitForCredentials;
      }
    }
    break;
  }
  case pgmStateWaitForConnect:  // non-blocking Wait for wifi connect
  {
    Serial.print("Wifi Status: ");Serial.println(s = WiFi.status());
    if(s == WL_CONNECTED){
        
        pgmState = pgmStateDoWork;
        break;
    }
    else if (s == WL_IDLE_STATUS){
      Serial.print(".");
      break;
    }
    else if (s == WL_NO_SSID_AVAIL) {
          WiFi.disconnect();
//          WiFi.enableAP(true); // enable AP so user can configure wifi credentials
          DoNotReconnectWifi = true;
          // wait 10 seconds before next reconnect attempt
          //DoNotConnectWifiTicker.attach(10,DoNotConnectWifiTickerHandler);
          Serial.println("SSID unavailable!");
          if(strlen(ssid)>0){
            pgmState = waitForCredentials;
          }
          break;
        }
    else if(s==WL_CONNECT_FAILED){
      Serial.println("Connection failed!");
 //   else if (s==WL_WRONG_PASSWORD){
          //WiFi.
//          Serial.println("Bad Password!");
 //         WiFi.enableAP(true);
//          WiFi.setAutoReconnect(false);
//          WiFi.setAutoConnect(false);
          WiFi.persistent(false); // don't connect on boot
          //DoNotConnectWifiTicker.attach(10,DoNotConnectWifiTickerHandler);
          DoNotReconnectWifi = true;
          pgmState = pgmStateWifiConnect;
          break;
    }
    else if(s==WL_DISCONNECTED){
      Serial.println("Disconnected!");
      pgmState = pgmStateWifiConnect;
    }
    else {
      Serial.println("wifi connect failed!");
//      WiFi.enableAP(true); // enable AP so user can configure WLAN
//      WiFi.setAutoReconnect(false); // dont autoreconnect 
//      WiFi.setAutoConnect(false);  // don't autoconnect
      WiFi.persistent(false); // don't connect on boot
//      WiFi.printDiag(Serial);
      DoNotReconnectWifi = true;
      //DoNotConnectWifiTicker.attach(10,DoNotConnectWifiTickerHandler); // dont try to reconnect for 10 seconds
      pgmState = waitForCredentials;
      break;
    }
    break;
  }
  case pgmStateCheckWifi:
  {
    s = WiFi.status();
     if(s == WL_CONNECTED) {
       
        if(ssid!=WiFi.SSID()&strlen(ssid)>0){ // SSID changed
          WiFi.disconnect();
          pgmState = pgmStateWifiConnect;
          break;
        }
        else {
              pgmState = pgmStateDoWork;
          break;
        }
     } // not connected
     else if(s == WL_IDLE_STATUS){ // connecting
        pgmState == pgmStateWaitForConnect;
        break;
     }
     else if(s==WL_DISCONNECTED){
       pgmState = pgmStateWifiConnect;
     }
     else {
      Serial.println("not connected!");
      pgmState = pgmStateWifiConnect;}
      break;
    } // end pgmStateCheckWifi

  case pgmStateDoWork:
  {
    // Update interfaces if necesssary 
    if(WiFi.status() == WL_CONNECTED){
        updateWundergroundTicker.update();
       if(shouldUpdateWundergroundInfce){
          shouldUpdateWundergroundInfce = false; // ticker will change to true
         UpdateWundergroundInfce();
          }
        pgmState = pgmStateIdle;
        break;
    }
  else {
    if(strlen(ssid)>0){
      pgmState = pgmStateWifiConnect;
    } else{
      pgmState = waitForCredentials;
    }
    break;
  }
  break;
  } // end case pgmStateDoWork

    case pgmStateIdle:
  { 
    if(WiFi.status()==WL_CONNECTED){ // if we're already connected, sleep for 5 seconds
      
      pgmState = pgmStateCheckWifi;
      sleep_ms(100);
     // ESP.deepSleep(5000000);
    }
    else{ // if we are not connected, sleep for .1ms to poll for credentials
      pgmState = pgmStateCheckWifi;
      //ESP.deepSleep(100);
    break;
    }
  }

  }// end switch
}

void setup1(){

  // setup I2C2 bus 
  Wire1.setSDA(2);
  Wire1.setSCL(3);

#ifdef USE_SHT31
  if (sht31.begin()) {   // Initialize the SHT31 T/H Sensor
  }
  else{
      Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
#endif

#ifdef USE_MPL3115A2 // Initialize the USE_MPL3115A2 Pressure sensor
    if (mpl3115a2.begin(&Wire1)) {
    }
    else{
      Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }
#endif 
    updateWundergroundTicker.start(); // Start the 5 second timer for the Wunderground interface
}

void loop1(){

  readSensors(); // Read environmental sensors
  //delay(1000);

  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  if (loopCnt >= 30) {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");

    loopCnt = 0;
  }
  loopCnt++;

  sleep_ms(2000);
}

// Connect to the Wifi SSID
void connectWifi() {
  Serial.println("Connecting as wifi client ");
  Serial.println(ssid);
  Serial.println(password);
  WiFi.begin(ssid, password); // Connect to Wifi
  //int connRes = WiFi.waitForConnectResult(); // Wait for Wifi to connect
  //Serial.print("connRes: "); 
  //Serial.println(connRes);
}

// Determines if we are ready to update the Wunderground interface. 
// Was intended to be called by a ticker or timer. Main loop will
// do the actual update. 
// Needs to be updated to require a "last update" timestamp
void ShouldUpdateWundergroundInterfaceTicker(){
   if(WiFi.status()==WL_CONNECTED){ // Must be connected to wifi to update wunderground 
    if(WundergroundInfceEnable&&(strlen(WundergroundStationID)>0)&&(strlen(WundergroundStationPassword)>0)&&rtc_running()){ // require station ID, Password, Infce Enabled, and RTC to update interface
        shouldUpdateWundergroundInfce = true;} //
    else{
      shouldUpdateWundergroundInfce = false;}
  }
  else {  
//    shouldUpdateDNS = false; 
    shouldUpdateWundergroundInfce = false;
    }
  return;
}

static void readSensors(){
//  (void)param;
  readTempHumiditySensor();
  ReadPressureSensor();
}