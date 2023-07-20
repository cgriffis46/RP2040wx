#ifdef USE_WUNDERGROUND_INFCE
void UpdateWundergroundInfce(){
//  now = rtc.now();
  WiFi.hostByName(url,wundergroundIP);
  if(WundergroundInfceEnable&& (strlen(WundergroundStationID)>0) && (strlen(WundergroundStationPassword)>0) && !(wundergroundIP==IPAddress(0,0,0,0))){ 
    //Serial.println("WundergroundInterface is Enabled");
    //Serial.print("Attempting to update "); Serial.println(WundergroundStationID);
    if(!client.connected()){
      if(!client.connect(wundergroundIP,80)){
        Serial.println("could not connect to wunderground IP");
      }}
    if(client.connected()){
      String date_str = "&dateutc=now";
//      String date_str = String(F("&dateutc="))+String(now.year())+F("-")+String(now.month())+F("-")+String(now.day())+String(F("+"))+String(now.hour())+String(F("%3A"))+String(now.minute())+String(F("%3A"))+String(now.second());
      Wundergroundpayload = "";
      // Send temperature and humidity if necessary
      #ifdef _INFCE_SEND_TEMP_HUMIDITY
      if(QueueThermometerForInterfaces){

        switch (thermometer1Type) {
          case 1: 
          {
            Wundergroundpayload += F("&tempc=");Wundergroundpayload += String(tempc);
            break;
          }
          case 2: 
          {
            Wundergroundpayload += F("&tempc2=");Wundergroundpayload += String(tempc);
            break;
          }
          case 3: 
          {
            Wundergroundpayload += F("&tempc3=");Wundergroundpayload += String(tempc);
            break;
          }
          case 4:
          {
            Wundergroundpayload += F("&tempc4=") ;Wundergroundpayload += String(tempc);
            break;
          }
          case 5:
          {
            Wundergroundpayload += F("&indoortempc=");Wundergroundpayload += String(tempc);
            break;
          }
          case 6:
          {
            Wundergroundpayload += F("&soiltempc2=");Wundergroundpayload += String(tempc);
            break;
          }
          case 7:
          {
            Wundergroundpayload += F("&soiltempc3=");Wundergroundpayload += String(tempc);
            break;
          }
          case 8:
          {
            Wundergroundpayload += F("&soiltempc4=");Wundergroundpayload += String(tempc);
            break;
          }
          case 9:
          {
            Wundergroundpayload += F("&tempf=");Wundergroundpayload += String(tempf);
            break;
          }
          case 10:
          {
            Wundergroundpayload += F("&tempf2=");Wundergroundpayload += String(tempf);
            break;
          }
          case 11:
          {
            Wundergroundpayload += F("&tempf3=");Wundergroundpayload += String(tempf);
            break;
          }
          case 12:
          {
            Wundergroundpayload += F("&tempf4=");Wundergroundpayload += String(tempf);
            break;
          }
          case 13:
          {
            Wundergroundpayload += F("&indoortempf=");Wundergroundpayload += String(tempf);
            break;
          }
          case 14:
          {
            Wundergroundpayload += F("&soiltempf=");Wundergroundpayload += String(tempf);
            break;
          }
          case 15:
          {
            Wundergroundpayload += F("&soiltempf2=");Wundergroundpayload += String(tempf);
            break;
          }
          case 16:
          {
            Wundergroundpayload += F("&soiltempf3=");Wundergroundpayload += String(tempf);
            break;
          }

        }

        }
        
        if(QueueHumidityForInterfaces){
        switch (humidity1_sensor_type) {
            case 1: {
              Wundergroundpayload += "&humidity="+String(humidity,2);
              break;
            }
            case 2: {
              Wundergroundpayload += "&indoorhumidity="+String(humidity,2);
              break;
            }
        }}

      #endif
      // Send barometric pressure if necessary 
      #ifdef _INFCE_SEND_BAROMETRIC_PRESSURE
        if(QueueBarometerForInterfaces==true){
          Wundergroundpayload += String("&baromin=")+String(pressure);}
      #endif

      WUcreds = "ID=" + String(WundergroundStationID) + "&PASSWORD="+ String(WundergroundStationPassword);
      if(Wundergroundpayload.length()>0){ // only transmit if we have a payload
        WundergroundHTTPString=WUurl+WUcreds+date_str+Wundergroundpayload+W_Software_Type+action_str;
        //Serial.println(WundergroundHTTPString);
        if(!httpWunderground.connected()){
          if(!httpWunderground.begin(client,WundergroundHTTPString)){
            Serial.println("Could not begin http client");
            Serial.println(WundergroundHTTPString);
          }
        }
        if(httpWunderground.connected()){
          //Serial.print("http get:");
          WundergroundResponseCode = httpWunderground.GET();
          //Serial.println(WundergroundResponseCode);
          if(WundergroundResponseCode==200){
            WundergroundResponse = "Wunderground Infce Updated!";
          }
          else if(WundergroundResponseCode==400){
            WundergroundResponse = "Wunderground Update Rejected by Wunderground. Check system time!";
          }
          else if(WundergroundResponseCode==401){
            WundergroundResponse = "Wunderground Update Rejected by Wunderground. Bad Station ID or Password!";
          }
          else if(WundergroundResponseCode==-11){
            WundergroundResponse = "Wunderground Update Rejected by Wunderground. Check sensors.";
          }
          else {
            WundergroundResponse = "Wunderground Update Rejected by Wunderground. Unknown error";
          }
        }
        else{
            WundergroundResponse = "Could not update Wunderground Interface - Not Connected";
        }
      }
      else {
          WundergroundResponse = "Could not update Wunderground - no payload";
          Serial.println("Could not update Wunderground: No Payload");
      }
    }
    else {
      WundergroundResponse = "Could not update Wunderground - client is not connected";
      Serial.println("could not updaate wunderground; client is not connected");
    }
  }
  else{
    if(!WundergroundInfceEnable){
      WundergroundResponse = "Wunderground interface is Disabled";}
    else if(strlen(WundergroundStationPassword)==0){
      WundergroundResponse = "Wunderground password not set";
    }
    else if(strlen(WundergroundStationID)==0){
      WundergroundResponse = "Wunderground ID not set";
    }
    else if(wundergroundIP==IPAddress(0,0,0,0)){
      WundergroundResponse = "Wunderground IP not found";
    }
    else{
      WundergroundResponse = "Wunderground Interface disabled";
    }
    Serial.println("WundergroundInterface is Disabled");
  }
  shouldUpdateWundergroundInfce = false;
}

void handleWundergroundConfigSave(){
  memset(WundergroundStationID,0,sizeof(WundergroundStationID));
  memset(WundergroundStationPassword,0,sizeof(WundergroundStationPassword));
  if(server.arg("e").equals("on")) {
    WundergroundInfceEnable = true;
  }
  else{
    WundergroundInfceEnable = false;
  }
  server.arg("i").toCharArray(WundergroundStationID, sizeof(WundergroundStationID) - 1);
  server.arg("p").toCharArray(WundergroundStationPassword, sizeof(WundergroundStationPassword) - 1);
  Serial.println(thermometer1Type = server.arg("t").toInt());
  Serial.println(humidity1_sensor_type = server.arg("h").toInt());
  server.sendHeader("Location", "config", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop();              // Stop is needed because we sent no content length
  SaveWundergroundCredentials();
}

void LoadWundergroundCredentials(){
  char ok[2 + 1];
  uint8_t enable = 0;
  uint32_t address;
  uint8_t buffer1[256];
  // wunderground interface enable
  #ifdef USE_SPI_FRAM
    enable = fram.read8(mem_WUNDERGROUND_ENABLE);
  #endif
  #ifdef USE_I2C_FRAM
    enable = fram.read(mem_WUNDERGROUND_ENABLE);
  #endif
  #ifdef USE_I2C_EEPROM
    enable = fram.read(mem_WUNDERGROUND_ENABLE);
  #endif
  if(enable>0){
    Serial.println("wunderground infce enabled");
    WundergroundInfceEnable = true;
  }
  else {
    Serial.println("wunderground infce disabled");
    WundergroundInfceEnable = false;
  }
  // Wunderground ID
  for(address = 0; address <sizeof(WundergroundStationID);address++){
    #ifdef USE_SPI_FRAM
      buffer1[address] = fram.read8(mem_WUNDERGROUNDID+address);
    #endif
    #ifdef USE_I2C_FRAM
      buffer1[address] = fram.read(mem_WUNDERGROUNDID+address);
    #endif
    #ifdef USE_I2C_EEPROM
      buffer1[address] = fram.read(mem_WUNDERGROUNDID+address);
    #endif
    WundergroundStationID[address] = char(buffer1[address]);
    }
  if(strlen(WundergroundStationID)>0){
    Serial.println(WundergroundStationID);
  }
  // Wunderground Password
  for(address = 0; address <sizeof(WundergroundStationPassword);address++){
    #ifdef USE_SPI_FRAM
      buffer1[address] = fram.read8(mem_WUNDERGROUNDPASSWORD+address);
    #endif
    #ifdef USE_I2C_FRAM
      buffer1[address] = fram.read(mem_WUNDERGROUNDPASSWORD+address);
    #endif
    #ifdef USE_I2C_EEPROM
      buffer1[address] = fram.read(mem_WUNDERGROUNDPASSWORD+address);
    #endif
    WundergroundStationPassword[address] = char(buffer1[address]);}
    // thermometer 1 type 
    #ifdef USE_SPI_FRAM
      thermometer1Type = ram.read8(mem_WUNDERGROUND_TEMP_ID);
    #endif
    #ifdef USE_I2C_FRAM
      thermometer1Type = fram.read(mem_WUNDERGROUND_TEMP_ID);
    #endif
    #ifdef USE_I2C_EEPROM
      thermometer1Type = fram.read(mem_WUNDERGROUND_TEMP_ID);
    #endif
    // humidity sensor 1 type 
    #ifdef USE_SPI_FRAM
      humidity1_sensor_type = ram.read8(mem_WUNDERGROUND_HUMIDITY_ID);
    #endif
    #ifdef USE_I2C_FRAM
      humidity1_sensor_type = fram.read(mem_WUNDERGROUND_HUMIDITY_ID);
    #endif
    #ifdef USE_I2C_EEPROM
      humidity1_sensor_type = fram.read(mem_WUNDERGROUND_HUMIDITY_ID);
    #endif

}

void SaveWundergroundCredentials(){
  uint32_t address;
  char ok[2 + 1];
  uint8_t buffer1[512];
  // wunderground interface enable 
  Serial.print("Station ID: "); Serial.println(WundergroundStationID);
  Serial.print("Station Password: "); Serial.println(WundergroundStationPassword);
  if(WundergroundInfceEnable == true){
    #ifdef USE_SPI_FRAM
      fram.writeEnable(true);
      fram.write8(mem_WUNDERGROUND_ENABLE,(uint8_t)255);
      fram.writeEnable(false);
    #endif
    #ifdef USE_I2C_FRAM
      fram.write(mem_WUNDERGROUND_ENABLE,(uint8_t)255);
    #endif
    #ifdef USE_I2C_EEPROM
      fram.write(mem_WUNDERGROUND_ENABLE,(uint8_t)255);
    #endif
    }
  else {
    #ifdef USE_SPI_FRAM
      fram.writeEnable(true);
      fram.write8(mem_WUNDERGROUND_ENABLE,0);
      fram.writeEnable(false);
    #endif
    #ifdef USE_I2C_FRAM
      fram.write(mem_WUNDERGROUND_ENABLE,0);
    #endif
    #ifdef USE_I2C_EEPROM
      fram.write(mem_WUNDERGROUND_ENABLE,0);
    #endif
    }
  
  // wunderground interface id
  for(address = 0; address <sizeof(WundergroundStationID);address++){
    buffer1[address] = (uint8_t)WundergroundStationID[address];
    #ifdef USE_SPI_FRAM
      fram.writeEnable(true);
      fram.write8(mem_WUNDERGROUNDID+address,buffer1[address]);
      fram.writeEnable(false);
    #endif
    #ifdef USE_I2C_FRAM
      fram.write(mem_WUNDERGROUNDID+address,buffer1[address]);
    #endif
    #ifdef USE_I2C_EEPROM
      fram.write(mem_WUNDERGROUNDID+address,buffer1[address]);
    #endif
    }

  // wunderground interface password
  for(address = 0; address <sizeof(WundergroundStationPassword);address++){
    buffer1[address] = (uint8_t)WundergroundStationPassword[address];
    #ifdef USE_SPI_FRAM
      fram.writeEnable(true);
      fram.write8(mem_WUNDERGROUNDPASSWORD+address,buffer1[address]);
      fram.writeEnable(false);
    #endif
    #ifdef USE_I2C_FRAM
      fram.write(mem_WUNDERGROUNDPASSWORD+address,buffer1[address]);
    #endif
    #ifdef USE_I2C_EEPROM
      fram.write(mem_WUNDERGROUNDPASSWORD+address,buffer1[address]);
    #endif
    }

    // wunderground thermometer sensor ID
    #ifdef USE_SPI_FRAM
      fram.writeEnable(true);
      fram.write8(mem_WUNDERGROUND_TEMP_ID,thermometer1Type);
      fram.writeEnable(false);
    #endif
    #ifdef USE_I2C_FRAM
      fram.write(mem_WUNDERGROUND_TEMP_ID,thermometer1Type);
    #endif
    #ifdef USE_I2C_EEPROM
      fram.write(mem_WUNDERGROUND_TEMP_ID,thermometer1Type);
    #endif

        // wunderground humidity sensor ID
    #ifdef USE_SPI_FRAM
      fram.writeEnable(true);
      fram.write8(mem_WUNDERGROUND_HUMIDITY_ID,humidity1_sensor_type);
      fram.writeEnable(false);
    #endif
    #ifdef USE_I2C_FRAM
      fram.write(mem_WUNDERGROUND_HUMIDITY_ID,humidity1_sensor_type);
    #endif
    #ifdef USE_I2C_EEPROM
      fram.write(mem_WUNDERGROUND_HUMIDITY_ID,humidity1_sensor_type);
    #endif
}
#endif
