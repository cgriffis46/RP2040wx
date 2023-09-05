/*
  Written by : Cory S Griffis
  (C) 8/10/2010
  Provided under MIT license. 
  No warranty provided. 
*/

#ifdef _USE_TH_SENSOR
static void readTempHumiditySensor(){

 // (void) param;

#ifdef USE_SHT31
float t, h;
  if(sht31.FetchData(&t, &h)){
  if (! isnan(t)) {  // check if 'is not a number'
//    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
    temperature = t;
    tempc = temperature;
    tempf = (temperature * 1.8) + 32;
  } else {
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
//    Serial.print("Hum. % = "); Serial.println(h);
    humidity = h;
  } else { 
    Serial.println("Failed to read humidity");
  }
}
#endif
}

#endif

#ifdef USE_BAROMETRIC_PRESSURE_SENSOR 

static void ReadPressureSensor(){
 // (void)param;
    #ifdef USE_MPL3115A2
      pressure = mpl3115a2.getPressure()*0.02953+PressureOffset;
    #endif

    #ifdef USE_BMP280
      if(bmp.takeForcedMeasurement()) {
      // can now print out the new measurements
      pressure = bmp.readPressure()*0.0002953+PressureOffset;
      if(WiFi.status()==WL_CONNECTED){
        if(!updateWundergroundTicker.active()){
        updateWundergroundTicker.attach(5,ShouldUpdateWundergroundInterfaceTicker);}
      }

    } else {
      Serial.println("Forced measurement failed!");
    }
    #endif

  //return 0;

}

#endif