/* Dev Patel - 3.22.2024
 *  Helper for BME688 with the PADL
 *  gasUpdate() - creates a reading and stores values into data variables
 *  STATUS - FUNCTIONAL 3/25/2024
 */
#include "Adafruit_BME680.h"


void gasUpdate(){
  airSensor.performReading();

  humidityData = airSensor.humidity;
  resistanceData = airSensor.gas_resistance / 1000.0; //Divides by 1000 to convert from Ohms to KiloOhms
}
