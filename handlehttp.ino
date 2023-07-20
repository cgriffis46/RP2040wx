/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) {  // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F("<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "<h1>HELLO WORLD!!</h1>");
  if (server.client().localIP() == apIP) {
    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  } else {
    Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
  }
  Page += F("<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
            "</body></html>");

  server.send(200, "text/html", Page);
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop();              // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

/** Wifi config page handler */
void handleWifi() {
if (!server.authenticate("user","password")){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }else {

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F("<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "<h1>Wifi config</h1>");
  if (server.client().localIP() == apIP) {
    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  } else {
    Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
  }
  Page += F("\r\n<br />");
  Page += F("<table><tr><th align='left'>Time:");
  Page += String(now.hour(),DEC) + String(":") + String(now.minute(),DEC) + String(":") + String(now.second(),DEC);
  Page += String("</th></tr>");               
  Page += String(F("\r\n<br />"
                   "<table><tr><th align='left'>SoftAP config</th></tr>"
                   "<tr><td>SSID "))
          + String(softAP_ssid) + F("</td></tr>"
                                    "<tr><td>IP ")
          + toStringIp(WiFi.softAPIP()) + F("</td></tr>"
                                            "</table>"
                                            "\r\n<br />"
                                            "<table><tr><th align='left'>WLAN config</th></tr>"
                                            "<tr><td>SSID ")
          + String(ssid) + F("</td></tr>"
                             "<tr><td>IP ")
          + toStringIp(WiFi.localIP()) + F("</td></tr>"
                                           "</table>"
                                           "\r\n<br />"
                                           "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>");
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n > 0) {
    for (int i = 0; i < n; i++) { Page += String(F("\r\n<tr><td>SSID ")) + WiFi.SSID(i) + ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? F(" ") : F(" *")) + F(" (") + WiFi.RSSI(i) + F(")</td></tr>"); }
  } else {
    Page += F("<tr><td>No WLAN found</td></tr>");
  }
  Page += F("</table>"
            "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
            "<input type='text' placeholder='network' name='n'/>"
            "<br /><input type='password' placeholder='password' name='p'/>"
            "<br /><input type='submit' value='Connect/Disconnect'/></form>"
            "<p>You may want to <a href='/'>return to the home page</a>.</p>"
            "</body></html>");
  server.send(200, "text/html", Page);
  server.client().stop();  // Stop is needed because we sent no content length
    }  
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  if (!server.authenticate("user","password")){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }else {
      memset(ssid,0,sizeof(ssid));
      memset(password,0,sizeof(password));
      Serial.println("wifi save");
      server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
      server.arg("p").toCharArray(password, sizeof(password) - 1);
      server.sendHeader("Location", "wifi", true);
      server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      server.sendHeader("Pragma", "no-cache");
      server.sendHeader("Expires", "-1");
      server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
      server.client().stop();              // Stop is needed because we sent no content length
      saveCredentials();
      connect = strlen(ssid) > 0;  // Request WLAN connect with new credentials if there is a SSID
    }
}



void handleNotFound() {
  if (captivePortal()) {  // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++) { message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n"); }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}

void handleWeather(){
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F("<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "<h1>Weather</h1>");
  Page += F("<table>");

  Page += F("<tr><th></th><th>Current</th><th>Max</th><th>Min</th></tr>");

  #ifdef _INFCE_SEND_TEMP_HUMIDITY
    Page += F("<tr><td>temperature</td><td>");
    if(UseCelcius){
      Page += String(tempc);
      Page += String("C");
    }else{
      Page += String(tempf);
      Page += String("F");
    }
//    Page += String((temperature * 1.8) + 32);
    Page += F("</td><td>max</td><td>min</td></tr>");
    Page += F("<tr><td>humidity</td><td>");
    Page += String(humidity)+F("%");
    Page += F("</td><td>max</td><td>min</td></tr>");
  #endif
  #ifdef _INFCE_SEND_BAROMETRIC_PRESSURE
    Page += F("<tr><td>pressure<td>");
    Page += String(pressure);
    Page += F(" inHg</td>");
    Page += F("<td>max</td><td>min</td></tr>");
  #endif
  Page += F("</table>");
  Page += F("</body></html>");
  server.send(200, "text/html", Page);
  server.client().stop();  // Stop is needed because we sent no content length
}

void handleConfig(){

if (!server.authenticate(dev_user,dev_password)){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }else {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F("<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>");
  // User Credentials section
  Page += F("\r\n<form method='POST' action='savedevicecredentials'>");
  Page += F("<form>");
  Page += F("<h1>Device Credentials</h1>");
  Page += F("<Label for 'DevicePassword'>Device Password:</label>");
  Page += F("<Input type='password' id='DevicePassword' name='d'");
  Page += F("<input type='submit' value='Save'/>");
  Page += F("</form>");
  Page += F("<h1>network</h1>");
  Page += F("</table>");
  Page += F("\r\n<br /><form method='POST' action='networksave'>");
  Page += F("<label>NTP Server</label>");
  Page += F("<input type='url' placeholder='pool.ntp.org'");
//  Page += F("'network'");
  Page += F(" name='n'/>");
  Page += F("<br /><label>DNS Server</label>");
  Page += F("<input type='text' placeholder='password' name='p'/>"
            "<br /><input type='submit' value='Save'/></form>");
  Page += F("<br />");

  Page += F("<h1>wunderground</h1>");
  Page += F("</table>");
  Page += F("\r\n<form method='POST' action='wundergroundsave'>");
  Page += F("<label>Enable</label>");
  Page += F("<input type='checkbox' ");
  if(WundergroundInfceEnable==true){
    Page += F("checked");
  }
  Page += F(" name='e'/><br />");

  Page += F("<Label for 'WundergroundTimeSource'>Time Source </label>");
  Page += F("<select name='z' id='WundergroundTimeSource'>");
  Page += F("<option value='1'");if(WundergroundTimeSource==1){Page += F("selected");} Page += F(">&now</option>");
  #ifdef _USE_RTC
    Page += F("<option value='2'");if(WundergroundTimeSource==2){Page += F("selected");} Page += F(">rtc</option>");
  #endif
  Page += F("<option value='3'");if(WundergroundTimeSource==3){Page += F("selected");} Page += F(">ntp</option>");
  Page += F("</select><br />");

  Page += F("<br /><label>Wunderground ID</label>");
  Page += F("<input type='text' placeholder='ID'");

  if(strlen(WundergroundStationID)>0){
      Page += F(" value='");
      Page += String(WundergroundStationID);
      Page += F("'");
  }
  Page += F(" name='i'/>");
  Page += F("<br /><label>Wunderground Password</label>");
  Page += F("<input type='text' placeholder='password' value='");
  Page += String(WundergroundStationPassword); 
  Page += F("' name='p'/><br />");
    Page += F("<Label for 'WundergroundThermometerID'>Thermometer ID </label>");
    Page += F("<select name='t' id='WundergroundThermometerID'>");
    if(UseCelcius){
      Page += F("<option value='1'");if(thermometer1Type==1){Page += F("selected");} Page += F(">tempc</option>");
      Page += F("<option value='2'");if(thermometer1Type==2){Page += F("selected");} Page += F(">tempc2</option>");
      Page += F("<option value='3'");if(thermometer1Type==3){Page += F("selected");} Page += F(">tempc3</option>");
      Page += F("<option value='4'");if(thermometer1Type==4){Page += F("selected");} Page += F(">tempc4</option>");
      Page += F("<option value='5'");if(thermometer1Type==5){Page += F("selected");} Page += F(">indoortempc</option>");
      Page += F("<option value='6'");if(thermometer1Type==6){Page += F("selected");} Page += F(">soiltempc</option>");
      Page += F("<option value='7'");if(thermometer1Type==7){Page += F("selected");} Page += F(">soiltempc2</option>");
      Page += F("<option value='8'");if(thermometer1Type==8){Page += F("selected");} Page += F(">soiltempc3</option>");
    }
    else
    {
      Page += F("<option value='9'");if(thermometer1Type==9){Page += F("selected");} Page += F(">tempf</option>");
      Page += F("<option value='10'");if(thermometer1Type==10){Page += F("selected");} Page += F(">tempf2</option>");
      Page += F("<option value='11'");if(thermometer1Type==11){Page += F("selected");} Page += F(">tempf3</option>");
      Page += F("<option value='12'");if(thermometer1Type==12){Page += F("selected");} Page += F(">tempf4</option>");
      Page += F("<option value='13'");if(thermometer1Type==13){Page += F("selected");} Page += F(">indoortempf</option>");
      Page += F("<option value='14'");if(thermometer1Type==14){Page += F("selected");} Page += F(">soiltemptempf</option>");
      Page += F("<option value='15'");if(thermometer1Type==15){Page += F("selected");} Page += F(">soiltempf2</option>");
      Page += F("<option value='16'");if(thermometer1Type==16){Page += F("selected");} Page += F(">soiltemp34</option>");
    }
  Page += F("</select><br />");
  //
  Page += F("<Label for 'WundergroundHumidity1ID'>Humidity Sensor ID </label>");
  Page += F("<select name='h' id='WundergroundHumidity1ID'>");
  Page += F("<option value='1'");if(humidity1_sensor_type==1){Page += F("selected");} Page += F(">humidity</option>");
  Page += F("<option value='2'");if(humidity1_sensor_type==2){Page += F("selected");} Page += F(">indoorhumidity</option>");
  Page += F("</select><br />");
  Page += F("<br /><input type='submit' value='Save'/></form>");
  Page += F("<br />");

  Page += F("<label>");
  Page += String(WundergroundHTTPString);
  Page += F("</label>");
  Page += F("<br />");

  Page += F("<label>");
  Page += String(WundergroundResponseCode);
  Page += F("</label>");
  Page += F("<br />");

  Page += F("<label>");
  Page += String(WundergroundResponse);
  Page += F("</label>");
  Page += F("<br />");

  Page += F("<h2>Restore Defaults</h2>");
  Page += F("\r\n<br /><form method='POST' action='restoredefaults'><br />");
  Page += F("<br /><input type='submit' value='Save'/></form>");

  Page += F("</body></html>");
  server.send(200, "text/html", Page);
  server.client().stop();  // Stop is needed because we sent no content length

}
}

void HandleSensors(){
  if (!server.authenticate(dev_user,dev_password)){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
  }
  else {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");

    String Page;
    Page += F("<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>");

#ifdef USE_BAROMETRIC_PRESSURE_SENSOR
    Page += F("<h2>Barometer</h2>");
    Page += F("<table>");
    Page += F("\r\n<form method='POST' action='SaveSensors'>");
    Page += F("<form>");
    Page += F("<Label for 'SendToInterfaces'>Send to Interfaces?</label><br>");
    Page += F("<Input type='checkbox' id=SendToInterfaces' name='i'");
    if(QueueBarometerForInterfaces==true){
      Page += F("checked");
    }
    Page += F("/><br>");
    Page += F("<label for'PressureOffset'>Calibration Offset</label><br>");
    Page += F("<input type='number' id='PressureOffset' name='b' min='-1' max='1' step='.01' value=");
    Page += String(PressureOffset,2);
    Page +=F(">");
    Page += F("<input type='submit' value='Save'/>");
    Page += F("</form>");
    Page += F("</table>");
#endif

#ifdef _USE_TH_SENSOR
    Page += F("<table>");
    Page += F("\r\n<form method='POST' action='SaveThermometer'>");
    Page += F("<form>");
    Page += F("<h2>Thermometer</h2>");
    Page += F("<Label for 'ThermometerUnitC'>Unit</label><br>");
    Page += F(" <br>"
              "<select name='u' id='ThermometerUnitC'>");
    if(UseCelcius){
      Page += F("<option value='C' selected>C</option>");
      Page += F("<option value='F'>F</option>");
    }
    else{
      Page += F("<option value='C'>C</option>");
      Page += F("<option value='F' selected>F</option>");
    }
    Page += F("</select><br />");
    Page += F("<Label for 'SendThermometerToInterfaces'>Send to Interfaces?</label><br>");
    Page += F("<Input type='checkbox' id=SendThermometerToInterfaces' name='t'");
    if(QueueThermometerForInterfaces==true){
      Page += F("checked");
    }
    Page += F("/>");
    Page += F("<input type='submit' value='Save'/>");
    Page += F("</form>");
    Page += F("</table>");
#endif
#ifdef _USE_TH_SENSOR
    Page += F("<table>");
    Page += F("<h2>Humidity</h2>");
    Page += F("\r\n<form method='POST' action='SaveHumidity'>");
    Page += F("<form>");
    Page += F("<Label for 'SendHumidityToInterfaces'>Send to Interfaces?</label><br>");
    Page += F("<Input type='checkbox' id=SendHumidityToInterfaces' name='h' ");
    if(QueueHumidityForInterfaces==true){
      Page += F("checked");
    }
    Page += F("/>");
    Page += F("<input type='submit' value='Save'/>");
    Page += F("</form>");
    Page += F("</table>");
#endif 
    Page += F("</body></html>");
    server.send(200, "text/html", Page);
    server.client().stop();  // Stop is needed because we sent no content length
  }
}

void SaveSensors(){
    if (!server.authenticate(dev_user,dev_password)){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }else {
//      memset(ssid,0,sizeof(ssid));
//      memset(password,0,sizeof(password));
      Serial.print("Save Pressure Offset: ");
      externalPressureOffset = server.arg("b").toFloat();
      QueueBarometerForInterfaces = server.arg("i").equals("on");
      Serial.println();
      server.sendHeader("Location", "sensors", true);
      server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      server.sendHeader("Pragma", "no-cache");
      server.sendHeader("Expires", "-1");
      server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
      server.client().stop();              // Stop is needed because we sent no content length
      saveSensorsToDisk();
    }
}

void SaveThermometer(){
    if (!server.authenticate(dev_user,dev_password)){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }else {
//      memset(ssid,0,sizeof(ssid));
//      memset(password,0,sizeof(password));
      Serial.print("Enable Thermometer: ");
      Serial.println(QueueThermometerForInterfaces = server.arg("t").equals("on"));
      Serial.println(UseCelcius = server.arg("u").equals("C"));
      server.sendHeader("Location", "sensors", true);
      server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      server.sendHeader("Pragma", "no-cache");
      server.sendHeader("Expires", "-1");
      server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
      server.client().stop();              // Stop is needed because we sent no content length
      saveThermometerToDisk();
    }
}

void SaveHumidity(){
    if (!server.authenticate(dev_user,dev_password)){
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }else {
//      memset(ssid,0,sizeof(ssid));
//      memset(password,0,sizeof(password));
      Serial.print("Enable Humidity: ");     
      Serial.println(QueueHumidityForInterfaces = server.arg("h").equals("on"));
      server.sendHeader("Location", "sensors", true);
      server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      server.sendHeader("Pragma", "no-cache");
      server.sendHeader("Expires", "-1");
      server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
      server.client().stop();              // Stop is needed because we sent no content length
      saveHumidityToDisk();
    }
}
