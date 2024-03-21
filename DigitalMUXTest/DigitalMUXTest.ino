/*
  Use the Qwiic Mux to access multiple I2C devices on seperate busses.
  By: Nathan Seidle @ SparkFun Electronics
  Date: May 17th, 2020
  License: This code is public domain but you buy me a beer if you use this
  and we meet someday (Beerware license).

  Some I2C devices respond to only one I2C address. This can be a problem
  when you want to hook multiple of a device to the I2C bus. An I2C Mux
  solves this issue by allowing you to change the 'channel' or port that
  the master is talking to.

  This example shows how to hook up two VL53L1X laser distance sensors with the same address.
  You can read the VL53L1X hookup guide and get the library from https://learn.sparkfun.com/tutorials/qwiic-distance-sensor-vl53l1x-hookup-guide

  The TCA9548A is a mux. This means when you enableMuxPort(2) then the SDA and SCL lines of the master (Arduino)
  are connected to port 2. Whatever I2C traffic you do, such as distanceSensor.startRanging() will be communicated to whatever
  sensor you have on port 2. This example creates an array of objects. This increases RAM but allows for
  independent configuration of each sensor (one sensor may be configured for long range, the others for short, etc).

  Outputs two sets of distances in mm and ft.

  Hardware Connections:
  Attach the Qwiic Mux Shield to your RedBoard or Uno.
  Plug two Qwiic VL53L1X breakout boards into ports 0 and 1.
  Serial.print it out at 115200 baud to serial monitor.

  SparkFun labored with love to create this code. Feel like supporting open
  source? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14685
*/

#include <Wire.h>

#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux
QWIICMUX myMux;

#define NUMBER_OF_SENSORS 2

#include <AS726X.h>

AS726X irSensor;
AS726X visSensor;

void setup()
{
  Serial.begin(115200);
  Serial.println("Qwiic Mux Shield Read Example");

  Wire.begin();

  if (myMux.begin() == false)
  {
    Serial.println("Mux not detected. Freezing...");
    while (1)
      ;
  }
  Serial.println("Mux detected");

  byte currentPortNumber = myMux.getPort();
  Serial.print("CurrentPort: ");
  Serial.println(currentPortNumber);

  //Initialize all the sensors
  bool initSuccess = true;

  myMux.setPort(1);
  if(!irSensor.begin()){
    initSuccess = false;
    }
  myMux.setPort(0);
  if(!visSensor.begin()){
    initSuccess = false;
  }
  

  if (initSuccess == false)
  {
    Serial.print("Freezing...");
    while (1)
      ;
  }

  Serial.println("Mux Shield online");
}

void loop()
{
  


    myMux.setPort(1);                               //Tell mux to connect to this port, and this port only
    visSensor.takeMeasurements();
        //Visible readings
        Serial.print(" Reading: V[");
        Serial.print(visSensor.getCalibratedViolet(), 2);
        Serial.print("] B[");
        Serial.print(visSensor.getCalibratedBlue(), 2);
        Serial.print("] G[");
        Serial.print(visSensor.getCalibratedGreen(), 2);
        Serial.print("] Y[");
        Serial.print(visSensor.getCalibratedYellow(), 2);
        Serial.print("] O[");
        Serial.print(visSensor.getCalibratedOrange(), 2);
        Serial.print("] R[");
        Serial.print(visSensor.getCalibratedRed(), 2);
        Serial.println();
      
     myMux.setPort(0);
     irSensor.takeMeasurements();
        //Near IR readings
        Serial.print(" Reading: R[");
        Serial.print(irSensor.getCalibratedR(), 2);
        Serial.print("] S[");
        Serial.print(irSensor.getCalibratedS(), 2);
        Serial.print("] T[");
        Serial.print(irSensor.getCalibratedT(), 2);
        Serial.print("] U[");
        Serial.print(irSensor.getCalibratedU(), 2);
        Serial.print("] V[");
        Serial.print(irSensor.getCalibratedV(), 2);
        Serial.print("] W[");
        Serial.print(irSensor.getCalibratedW(), 2);
        Serial.println();
      
      
  }
