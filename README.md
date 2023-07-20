# RP2040wx
 RP2040 Weather Station

/*  By: Cory S Griffis
    7/17/2023

    Simple weather station program to grab environmental data from sensors and upload to Wunderground. 
    Captive portal simplifies setting up the station, WiFi credentials and interfaces.

    Setup:
    Use a phone or wifi device to connect to the device SSID. You may be prompted for credentials. 
    default user "user" password "password". Change device password. 
    Open the captive portal and navigate to the device http://<deviceip>/wifi.
    Enter SSID and password and attempt to connect to Wifi.

    From the browser, determine the WiFi IP. and navigate to http://<deviceip>/config
    Here you can add the credentials for the Wunderground interface. 
    
    Requires RP2040 pico w mcu board. 
    Requires an Adafruit Fram or EEPROM to store credentials.
    #define USE_I2C_EEPROM
          &Wire()
    #define USE_MPL3115A2 - MPL3115A2
          &Wire1()
          Wire1.setSDA(2);
          Wire1.setSCL(3);
    //Can be setup to use a DS3231 type RTC.

    //Can be setup to use the following sensors:
    
    // 

*/