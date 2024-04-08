// 


/************************************************
      Code for the CD74HC4067 Analog 16 Channel Mux
      Created by: Ashton Posey

      Fly-U-Mah additions by Dev Patel
      *Added analog reading to celsius temperature conversion based on logic from Thermistor.h
      *Stores each reading in an array output to an SD card in the main PADL33_CSE1012 file
      Modification Date: 4/5/24
************************************************/

// Mux Variables 
int AMUX_ANALOG_PIN = A1;
int AMUX_PINS [4] = {3, 4, 5, 6};
int amux_bit = 0;

// A, B, and C are constants used for a 10k resistor and 10k thermistor for the steinhart-hart equation
float A = 0.001125308852122;   
float B = 0.000234711863267;
float C = 0.000000085663516; 

//Testing helper function that converts from voltage drop to analog reading
float voltageToAnalog(float volts){
    return volts/(5.0) * 1023.0;
  }

//Flight-ready function that converts analog pin readings to celsius temperature outputs
float analogToCel(float aRead){
  float R1 = 10000;
  float adcMax = pow(2, 10)-1.0; //analog Resolution bits is 10;
  float adcVal = aRead; // Steinhart-Hart equation begin:
  float logR = log(((adcMax/adcVal)-1)*R1);
  float Tinv = A+B*logR+C*logR*logR*logR;
  float T = 1/Tinv;
  float currentTempC = T-273.15; // converting to celcius

  return currentTempC;
}

void muxThermSetup() {
  for (int i=0; i<4; i++) pinMode(AMUX_PINS[i], OUTPUT);
  Serial.println("MUX Thermistors Successful");
}

void muxThermUpdate() {
  for (int j=0; j<16; j++){
      for (int i=0; i<4; i++){  
          digitalWrite(AMUX_PINS[i], bitRead(amux_bit,i));
      }
      
      int analogMuxValue = analogRead(AMUX_ANALOG_PIN);
      thermData[j] = analogToCel(analogMuxValue); 
      amux_bit = amux_bit+1;
  }
  amux_bit = 0;
}
