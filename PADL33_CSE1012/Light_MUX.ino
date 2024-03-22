#include <SparkFun_I2C_Mux_Arduino_Library.h> 
#include <AS726X.h>

//NOTE: THIS ASSUMES THAT PORT 1 IS CONNECTED TO THE VISUAL SENSOR AND PORT 0 TO THE IR SENSOR. I will probably make a variable for this later
void lightUpdate(){
  myMux.setPort(0);
  irSensor.takeMeasurements();

  irData[0] = irSensor.getCalibratedR();
  irData[1] = irSensor.getCalibratedS();
  irData[2] = irSensor.getCalibratedT();
  irData[3] = irSensor.getCalibratedU();
  irData[4] = irSensor.getCalibratedV();
  irData[5] = irSensor.getCalibratedW();

  myMux.setPort(1);
  visSensor.takeMeasurements();
  visData[0] = visSensor.getCalibratedViolet();
  visData[1] = visSensor.getCalibratedBlue();
  visData[2] = visSensor.getCalibratedGreen();
  visData[3] = visSensor.getCalibratedYellow();
  visData[4] = visSensor.getCalibratedOrange();
  visData[5] = visSensor.getCalibratedRed();
  }

  
