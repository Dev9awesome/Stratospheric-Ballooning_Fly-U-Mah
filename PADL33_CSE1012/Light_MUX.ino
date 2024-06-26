/*
* Helper file for I2C MUX and AS726x visual light and ir sensors. Allow for clean, readable, non-blocking code in the system file
* lightUpdate() - takes readings from IR sensor and then the VIS sensor when connected to an I2C mux, using variables.h constants for ports
* Uses raw readings as the total range of all possible values is standardized between wavelengths/colors
* STATUS - FUNCTIONAL 3/25/2024
*/
#include <SparkFun_I2C_Mux_Arduino_Library.h> 
#include <AS726X.h>

//Method to be called in systemUpdate() method with a non-blocking timer. Forces MUX to read IR port first, fill the associated data array, and then do the same with the VIS port after
//THERE IS NO SMART CHECKING FOR WHICH SENSOR IS CONNECTED TO WHICH MUX PORT. ENSURE THE DEFINED PORT ENTRIES ARE CORRECT FOR THE HARDWARE CONFIGURATION
void lightUpdate(){
  //Note setPort() automatically closes all other ports so there is no conflict
  myMux.setPort(IR_MUX_PORT);
  irSensor.takeMeasurements();

  //Array for IR data is filled with readings. Raw values have a consistent maximum 
  irData[0] = irSensor.getR();
  irData[1] = irSensor.getS();
  irData[2] = irSensor.getT();
  irData[3] = irSensor.getU();
  irData[4] = irSensor.getV();
  irData[5] = irSensor.getW();

  //Repeats above IR readings for the VIS (visual light) sensor.
  myMux.setPort(VIS_MUX_PORT);
  visSensor.takeMeasurements();

  visData[0] = visSensor.getViolet();
  visData[1] = visSensor.getBlue();
  visData[2] = visSensor.getGreen();
  visData[3] = visSensor.getYellow();
  visData[4] = visSensor.getOrange();
  visData[5] = visSensor.getRed();
  }

  
