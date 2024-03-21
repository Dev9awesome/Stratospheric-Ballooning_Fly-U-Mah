

/************************************************
      MnSGC Ballooning/Rocketry PADL-33 Flight Code
      Created by: Ashton Posey

      Modification Date: 1/14/24
************************************************/
//Purpose: Code for the PADL-33 Flight Computer
#define Version "2.2 CSE 1012"

#include <ReefwingLPS22HB.h>
#include <Arduino_HS300x.h>
#include "SparkFun_BMI270_Arduino_Library.h"
#include "DFRobot_BMM150.h"
#include <Arduino_APDS9960.h>
#include <SdFat.h>
#include <Wire.h>
#include <SafeString.h>
#include <math.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // http://librarymanager/All#SparkFun_u-blox_GNSS
#include <SparkFun_KX13X.h> // http://librarymanager/All#SparkFun_KX13X
#include <SFE_MicroOLED.h>  // https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Librar

#include <SparkFun_I2C_Mux_Arduino_Library.h> 
#include "Adafruit_SGP30.h"
#include "Adafruit_BME680.h"


#include "OLED.h"
#include "Thermistor.h"
#include "LED.h"

#include "variables.h" // Must be Last in the Include list

//////////////////////////////////////////// GLOBAL VARIABLES ////////////////////////////////////////////
String header = "hh:mm:ss,FltTimer,T(s),T(ms),Hz,T2,T3,T4,T5,T6,totT,extT(F) or ADC,extT(C),intT(F),intT(C),Fix Type,RTK,PVT,Sats,Date,Time,Lat,Lon,Alt(Ft),Alt(M),HorizAcc(MM),VertAcc(MM),VertVel(Fte3/S),VertVel(MM/S),ECEFstat,ECEFX(CM),ECEFY(CM),ECEFZ(CM),NedVelNorth(M/S),NedVelEast(M/S),NedVelDown(M/S),GndSpd(M/S),Head(Deg),PDOP,kPa,ATM,PSI,C,F,Ft,M,VV(Ft),VV(M),G(y),G(x),G(z),Deg/S(x),Deg/S(y),Deg/S(z),uT(x),uT(y),uT(z),Humidity(%),GasResistance(kOhm)" + String(Version);


//////////////////////////////////////////// GLOBAL VARIABLES ////////////////////////////////////////////

//Digital MUX (new)
QWIICMUX myMux;
final int NUMBER_OF_SPECTRO_SENSORS = 2;
//SGP30 MOX sensor (new)
//Adafruit_SGP30 **vocSensor;

//BME688 (new)
Adafruit_BME680 airSensor;

void setup() { //////////////////////////////////////////// SETUP END ////////////////////////////////////////////

    systemSetUp();

//////////////////////////////////////////// ADD SETUP CODE HERE ////////////////////////////////////////////

//BME688 (new)
if(!airSensor.begin()){
  Serial.print("AQ Sensor Not Found");
  }
//Default oversampling and filter init
airSensor.setTemperatureOversampling(BME680_OS_8X);
airSensor.setHumidityOversampling(BME680_OS_2X);
airSensor.setPressureOversampling(BME680_OS_4X);
airSensor.setIIRFilterSize(BME680_FILTER_SIZE_3);
airSensor.setGasHeater(320, 150); // 320*C for 150 ms
  

//////////////////////////////////////////// ADD SETUP CODE HERE ////////////////////////////////////////////

} ///////////////////////////////////////////////////////// SETUP END ////////////////////////////////////////////

void loop() { ///////////////////////////////////////////// LOOP START /////////////////////////////////////////////

    if(millis() - timer >= DATA_DELAY){ // Don't add code here, refer to the updateData function
        timer = millis();
        updateData();
    }

} ///////////////////////////////////////////////////////// LOOP END /////////////////////////////////////////////

///////// Functions ////////////
// void updateData();

void updateData(){
    systemUpdate();

//////////////////////////////////////////// ADD LOOP CODE HERE ////////////////////////////////////////////
//BME688 (new)
airSensor.performReading();

//////////////////////////////////////////// ADD LOOP CODE HERE ////////////////////////////////////////////

    data = flightTimer;
    data += ",";
    data += flightTimerString;
    data += ",";
    data += timerSec;
    data += ",";
    data += timer;
    data += ",";
    data += frequencyHz;
    data += ",";
    data += timer2;
    data += ",";
    data += timer3;
    data += ",";
    data += timer4;
    data += ",";
    data += timer5;
    data += ",";
    data += timer6;
    data += ",";
    data += timerTotal;
    data += ",";

    if(thermExtOrADC){
        data += ThermistorExt.getTempF();
        data += ",";
        data += ThermistorExt.getTempC();
        data += ",";
    }
    else{
        data += photoresistorADCValue;
        data += ",";
        data += 0;
        data += ",";
    }

    data += ThermistorInt.getTempF();
    data += ",";
    data += ThermistorInt.getTempC();
    data += ",";
    data += fixTypeGPS;
    data += ",";
    data += fixTypeRTK;
    data += ",";
    data += PVTstatus;
    data += ",";
    data += SIV;
    data += ",";
    data += gpsMonth;
    data += "/";
    data += gpsDay;
    data += "/";
    data += gpsYear;
    data += ",";
    data += gpsHour;
    data += ":";
    data += gpsMinute;
    data += ":";
    data += gpsSecond;
    data += ".";

    if (gpsMillisecond < 10) {
        data += "00";
        data += gpsMillisecond;
        data += ",";
    }
    else if (gpsMillisecond < 100) {
        data += "0";
        data += gpsMillisecond;
        data += ",";
    }
    else{
        data += gpsMillisecond; 
        data += ",";
    }

    data += gpsLatInt;
    data += ".";
    data += gpsLatDec;
    data += ",";
    data += gpsLonInt;
    data += ".";
    data += gpsLonDec;
    data += ",";
    data += gpsAltFt;
    data += ",";
    data += gpsAltM;
    data += ",";
    data += gpsHorizAcc;
    data += ",";
    data += gpsVertAcc;
    data += ",";
    data += gpsVertVelFt;
    data += ",";
    data += gpsVertVelM;
    data += ",";
    data += ecefStatus;
    data += ",";
    data += ecefX;
    data += ",";
    data += ecefY; 
    data += ",";
    data += ecefZ;
    data += ","; 
    data += velocityNED[0];
    data += ",";
    data += velocityNED[1]; 
    data += ",";
    data += velocityNED[2];
    data += ","; 
    data += gpsGndSpeed;
    data += ",";
    data += gpsHeading;
    data += ",";
    data += gpsPDOP;
    data += ",";
    data += pressureSensor[0];
    data += ",";
    data += pressureSensor[1];
    data += ",";
    data += pressureSensor[2];
    data += ",";
    data += pressureSensor[3];
    data += ",";
    data += pressureSensor[4];
    data += ",";
    data += pressureSensor[5];
    data += ",";
    data += pressureSensor[6];
    data += ",";
    data += vertVelFt;
    data += ",";
    data += vertVelM;
    data += ",";
    data += imu.data.accelX;
    data += ",";
    data += imu.data.accelY;
    data += ",";
    data += imu.data.accelZ;
    data += ",";
    data += imu.data.gyroX;
    data += ",";
    data += imu.data.gyroY;
    data += ",";
    data += imu.data.gyroZ;
    data += ",";
    data += magData.x;
    data += ",";
    data += magData.y;
    data += ",";
    data += magData.z;
    data += ",";
    data += airSensor.humidity;
    data += ",";
    data += airSensor.gas_resistance / 1000.0;
    data += "\n";

    Serial.print(data);

    timer5 = millis() - timer7; ///////////// Timer 5 End ///////////// 
    

    timer6 = millis(); ///////////// Timer 6 Start ///////////// 

    String strOLED, strOLED2;

    strOLED = String(gpsLat,4) + "\n" + String(gpsLon,4) + "\n" + String(gpsAltFt, 2) + "ft\n";

    strOLED += String(SIV);
    strOLED2 = String(SIV);
    for(int i=strOLED2.length(); i<3; i++){
        strOLED += " ";
    }
    strOLED += "S " + String(frequencyHz, 1) + "\nI:" + String(int(ThermistorInt.getTempF())) + " E:" + String(int(ThermistorExt.getTempF())) + "\n" + String(pressureSensor[5]) + " Ft";
    OLED.update(strOLED);

    dataAdded += data;
    if (dataCounter == 3){
        datalog.print(dataAdded);
        datalog.flush();
        dataAdded = "";
    }
    dataCounter++;
    if (dataCounter == 4) dataCounter = 0;

    timer6 = millis() - timer6; ///////////// Timer 6 End ///////////// 

    timerTotal = timer2 + timer3 + timer4 + timer5 + timer6;

} /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
