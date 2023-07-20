# RP2040wx
 RP2040 Weather Station

/*  By: Cory S Griffis
    7/17/2023

    Simple weather station program to grab environmental data from sensors and upload to Wunderground. 
    Captive portal simplifies setting up the station, WiFi credentials and interfaces.

    Setup:
    Use a phone or wifi device to connect to the device SSID. You may be prompted for credentials. 
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

    //This version was coded to use a 16x2 character lcd with I2C backpack.

    //Can be setup to use the following sensors:
    
    Future versions will not use the ESP8266 as the chip is obsolete.
    This program will see limited support if any. The main purpose was to 
    demonstrate a simple wifi weather station. Reusing old hardware is fun
    and keeps products out the landfills. At this time  
    new designs use RP2040 or ESP32 based boards instead. 

*/