![rp2040wx_fritz_bb](https://github.com/cgriffis46/RP2040wx/assets/78368880/f782d777-d45e-46a9-9e4c-c5149565e536)


# RP2040wx
 RP2040 Weather Station

/*  By: Cory S Griffis
    7/17/2023

    v00.00.20

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

    Project was intended to use the DS3231 RTC as an external clock source. 
    Connect the 32KHz output to GPIO20 and the 1Hz to GPIO22. 
    The 1Hz will drive the RTC and the 32KHz will drive clk_ref. 

    Requires an Adafruit Fram or EEPROM to store credentials.

    This project uses Wire() for RTC and EEPROM and Wire1() for I2C sensors. 

    #define USE_I2C_EEPROM
          &Wire()
    #define USE_MPL3115A2 - MPL3115A2
          &Wire1()
          Wire1.setSDA(2);
          Wire1.setSCL(3);

    //Can be setup to use the following sensors. 
    //
      // SHT31
      // MPL3115A2
      // 
    // 

*/
