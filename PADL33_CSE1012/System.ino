/************************************************
      PADL-33 System Code
      Created by: Ashton Posey
************************************************/
//Purpose: General functions to run the buzzer, heater, update frequency, thermistors, voltage readers, & Micro SD Card

////////////////////////////////////// System Set Up //////////////////////////////////////
// void systemSetUp();
// void sdSetup(char *, byte const &, byte const &);
// float updateFrequency(unsigned long);
// void setHeaterState();
// void beep();
// void updateBeep();

void systemSetUp(){
    delay(2000);
    Serial.begin(SERIAL_BAUD);
    Serial.println("Serial Online!");

    Wire.begin();
    Wire.setClock(I2C_CLOCK);

    Wire1.begin();
    Wire1.setClock(I2C_CLOCK);

    OLED.begin();
    OLED.update("Init...");
    delay(500);
    Wire1.setClock(I2C_CLOCK);

    // BLE33 LED's 
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);

    digitalWrite(LEDR, LOW);       // will turn the LED on
    delay(100);
    digitalWrite(LEDG, LOW);       // will turn the LED on
    delay(100);
    digitalWrite(LEDR, HIGH);       // will turn the LED off
    digitalWrite(LEDB, LOW);       // will turn the LED on
    delay(100);
    digitalWrite(LEDG, HIGH);       // will turn the LED off
    digitalWrite(LEDR, LOW);       // will turn the LED on
    delay(100);
    digitalWrite(LEDB, HIGH);         // will turn the LED off 
    delay(100);
    digitalWrite(LEDR, HIGH);         // will turn the LED off

    // PADL-33 LED's
    YELLOW_LED.begin();
    GREEN_LED.begin();
    BLUE_LED.begin();
    BLUE_LED.begin();

    YELLOW_LED.on();
    delay(100);
    BLUE_LED.off();
    GREEN_LED.on();
    delay(100);
    YELLOW_LED.off();
    RED_LED.on();
    delay(100);
    GREEN_LED.off();
    BLUE_LED.on();
    delay(100);
    RED_LED.off();
    YELLOW_LED.on();
    delay(100);
    BLUE_LED.off();
    delay(100);
    YELLOW_LED.off();
    
    pinMode(HEATER_PIN,OUTPUT);

    delay(20);
    //delay(100);

    ////////////////////////////////////// SD Card Set Up //////////////////////////////////////
    //SPI.setClockDivider(SPI_CLOCK_DIV64);

    _print = (sdSetup(filename, dataFileN1, dataFileN2, directory) && sdActive) ? "Card initialized.\nCreating File... Logging to: " + String(filename) : (sdActive) ? "No available file names; clear SD card to enable logging" : "Card failed, or not present";  
    Serial.println(_print);
    _print = (sdStatus && sdActive) ? "Logging:\n\n" + String(filename) : (sdStatus) ? "NAFM\nClear SD\n\n\n\nSkip: B" : "SD\nFailed\n\n\n\nSkip: B";
    OLED.update(_print);
    delay(2000);
    if (!sdActive || !sdStatus) {
        digitalWrite(LEDR, LOW);       // will turn the LED on
        RED_LED.on();
        //if (usingBuzzer) tone(TONE_PIN, 400);
    }
    else greenLedCounter++;

    ////////////////////////////////////// Thermistor Set Up //////////////////////////////////////
    if (thermExtOrADC){
        _print = (ThermistorExt.begin(10)) ? "Therm Ext Online!" : "Therm Ext Offline...";
        Serial.println(_print);
        OLED.update(_print);
        if (ThermistorExt.getStatus()) greenLedCounter++;
    }
    else greenLedCounter++;

    delay(1000);

    _print = (ThermistorInt.begin(10)) ? "Therm Int Online!" : "Therm Int Offline...";
    Serial.println(_print);
    OLED.update(_print);
    if (ThermistorExt.getStatus()) greenLedCounter++;

    delay(1000);

    pressureSensorSetUp();
    imuSetUp();
    gpsSetUp();

    ////////////////////////////////////// Final System Set Up //////////////////////////////////////
    analogReadResolution(10);

    Serial.print("Green LED Count = ");
    Serial.println(greenLedCounter);
    if (greenLedCounter >= 7){
        GREEN_LED.on();
        Serial.println("All Systems GO!");
        delay(500);
        GREEN_LED.off();
    }

    SD.chdir("/" + directory);
    //datalog = SD.open(filename, O_CREAT | O_APPEND | O_WRITE); // Apparently calls sync() aswell?
    //datalog = SD.open(filename, O_WRITE | O_CREAT | O_APPEND | O_SYNC); // Apparently calls sync() aswell?
    //datalog = SD.open(filename, FILE_WRITE);
    datalog = SD.open(filename, O_CREAT | O_APPEND | O_WRITE);
    datalog.println(header);
    datalog.close();

    Serial.println(header);

    //delay(250);
    delay(20);
    //SPI.setClockDivider(32);

    //OLED.update("All Done!");
    SD.chdir("/" + directory);
    datalog = SD.open(filename, O_APPEND | O_WRITE);
    //datalog = SD.open(filename, O_CREAT | O_APPEND | O_WRITE | O_SYNC);
    //datalog = SD.open(filename, O_READ | O_WRITE | O_CREAT);
    //datalog = SD.open(filename, FILE_WRITE);
    //beep();


    flightTimerString = "NA:NA:NA";
    gpsPrevTime = millis()/1000.0;
    msPrevTime = millis()/1000.0;
    //SafeString::setOutput(Serial); // enable full debugging error msgs
}

void systemUpdate(){
    YELLOW_LED.on();

    timerSec = timer/1000.0;

    timer2 = millis(); ///////////// Timer 2 Start ///////////// 
    
    gpsUpdate();

    timer2 = millis() - timer2; ///////////// Timer 2 End ///////////// 

    timer3 = millis(); ///////////// Timer 3 Start /////////////
    //if (usingBuzzer) updateBeep();

    frequencyHz = updateFrequency(timer);

    // voltage_5v = analogRead(VOLTAGE_READER_PIN_5V);
    // voltage_5v = voltage_5v/1023 * 3.3 * 4;
    // voltage_3v7 = analogRead(VOLTAGE_READER_PIN_3V7);
    // voltage_3v7 = voltage_3v7/1023 * 3.3 * 2;

    // if (voltage_3v7 < 3.5) RED_LED.on();
    // else RED_LED.off();

    if (thermExtOrADC && ThermistorExt.getStatus()) ThermistorExt.update();
    else photoresistorADCValue = analogRead(EXTERNAL_THERMISTOR_PIN);
    
    if (ThermistorInt.getStatus()) ThermistorInt.update();

    pressureSensorUpdate();

    timer3 = millis() - timer3; /////////////////////////


    timer4 = millis(); ///////////// Timer 3 End ///////////// 
    imuUpdate();
    timer4 = millis() - timer4; /////////////////////////

    YELLOW_LED.off();

    //////////////////////////////////////////// UPDATE SERIAL MONITOR ////////////////////////////////////////////
    timer7 = millis(); ///////////// Timer 5 Start ///////////// 

    /////////////// UPDATE FLIGHT TIMER ///////////////
    long timer8 = timerSec; // Format: "00:00:00" -> h:m:s
    strTimer = timer8/3600;
    flightTimer = strTimer;
    flightTimer += ":";
    timer8 %= 3600;
    strTimer = timer8/600;
    flightTimer += strTimer;
    timer8 %= 600;
    strTimer = timer8/60;
    flightTimer += strTimer;
    flightTimer += ":";
    timer8 %= 60;
    strTimer = timer8/10;
    flightTimer += strTimer;
    strTimer = timer8 % 10;
    flightTimer += strTimer;
    ///////////// UPDATE FLIGHT TIMER ///////////////
}


bool sdSetup(char *dataFile, byte const &N1, byte const &N2, String dir){
  pinMode(CHIP_SELECT,OUTPUT);
  //if(!SD.begin(CHIP_SELECT, SD_SCK_MHZ(50))) sdStatus = 0;   /////SD_SCK_MHZ(25)
  if(!SD.begin(CHIP_SELECT, SPI_FULL_SPEED)) sdStatus = 0;
  //if(!SD.begin(CHIP_SELECT)) sdStatus = 0;
  else {
      sdStatus = 1;
      //SD.chdir("");
      SD.mkdir(dir);
      SD.chdir("/" + dir);
      for (byte i = 0; i < 100; i++) {
          dataFile[N1] = '0' + i/10; 
          dataFile[N2] = '0' + i%10;
          if (!SD.exists(dataFile)) {
              //datalog = SD.open(dataFile, O_CREAT | O_APPEND | O_WRITE);
              sdActive = true;
              //datalog.close();
              break;
          }
          else sdActive = false;
      }
  } 
  //SD.mkdir("");
  return sdStatus;
}

float updateFrequency(unsigned long _time){
  float frequency = 1.0 / ((_time - hzPrevTime)/1000.0);
  hzPrevTime = _time;
  return frequency;
}

// void setHeaterState(){
//   if(pressureSensor[4] <= minTemp && heaterStatus != true){ 
//       digitalWrite(HEATER_PIN, HIGH);
//       heaterStatus = true;
//       digitalWrite(LEDB, LOW);         // will turn the LED on
//   }
//   else if(pressureSensor[4] >= maxTemp && heaterStatus != false){ 
//       digitalWrite(HEATER_PIN, LOW);
//       heaterStatus = false;
//       digitalWrite(LEDB, HIGH);         // will turn the LED off
//   }
// }

void beep(){
  tone(TONE_PIN, 400);
  delay(200);
  noTone(TONE_PIN);
  delay(200);
}

byte beepTimer = 0;

void updateBeep(){
    if (beepTimer == 10) beepTimer = 0;
    if (frequencyHz > 9.0){
      if (beepTimer == 0) tone(TONE_PIN, 300);
      else if (beepTimer == 1) noTone(TONE_PIN);
      else if (beepTimer == 2) tone(TONE_PIN, 600);
      else if (beepTimer == 3) noTone(TONE_PIN);
      else {}
      beepTimer++;
    }
}