/* Dev Patel - 3.22.2024
 *  Helper for BME688 with the PADL
 *  gasUpdate() - creates a reading and stores values into data variables
 */
#include "Adafruit_BME680.h"


void gasUpdate(){
  airSensor.performReading();

  humidityData = airSensor.humidity;
  resistanceData = airSensor.gas_resistance / 1000.0;
}
