// Code for the CD74HC4067 Analog 16 Channel Mux
// Author: Ashton Posey - posey033@umn.edu

// Mux Variables 
int AMUX_ANALOG_PIN = A6;
int AMUX_PINS [4] = {3, 4, 5, 6};
float AMUX_VALUES [16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int amux_bit = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i=0; i<4; i++) pinMode(AMUX_PINS[i], OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  for (int j=0; j<16; j++){
      //Serial.println("Bit: " + String(amux_bit));
      for (int i=0; i<4; i++){
          // Serial.print(bitRead(amux_bit,i));
          digitalWrite(AMUX_PINS[i], bitRead(amux_bit,i));
      }
      //delay(10);
     // Serial.println();
      int analogMuxValue = analogRead(A6);
      //Serial.print("Analog Value: ");
      //Serial.println(analogMuxValue);
      //Serial.print("Analog Value Printed as Voltage: ");
     // Serial.println(analogMuxValue/1023.0 * 3.3, 4);
      AMUX_VALUES[j] = analogMuxValue/1023.0 * 5; 
      //Serial.println();
      //for (int i=0; i<4; i++) digitalWrite(MUX_PINS[i], LOW);
      amux_bit = amux_bit+1;
      //delay(10);
  }
  amux_bit = 0;

  Serial.println("\nAll 16 Values: ");
  for (int i=0; i<16; i++) Serial.println("Bit: " + String(i) + "  Voltage: " + String(AMUX_VALUES[i]) + "");
  delay(1000);
}
