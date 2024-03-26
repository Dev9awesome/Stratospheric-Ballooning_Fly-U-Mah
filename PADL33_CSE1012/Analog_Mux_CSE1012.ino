// Code for the CD74HC4067 Analog 16 Channel Mux
// Author: Ashton Posey - posey033@umn.edu

//Edited by Dev Patel - 3.25.2024

// Mux Variables 
int AMUX_ANALOG_PIN = A1;
int AMUX_PINS [4] = {3, 4, 5, 6};
int amux_bit = 0;

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
      thermData[j] = analogMuxValue/1023.0 * 5; //NOTE: CONVERSION TO TEMPERATURE HAS NOT BEEN CALCULATED YET. THIS CURRENTLY READS VOLTAGE DROP.
      
      amux_bit = amux_bit+1;
  }
  amux_bit = 0;
}
