/************************************************
      PADL-33 GPS Code
      Created by: Ashton Posey
************************************************/
//Purpose: General functions to run the NEO-M9N, NEO-M8N, & ZED-F9P GPS

////////////////////////////////////// GPS Set Up //////////////////////////////////////
// void gpsSetUp();
// void gpsUpdate();
// void printPVTdata(UBX_NAV_PVT_data_t *ubxDataStruct);// ZED-F9P Function
// void printRXMCOR(UBX_RXM_COR_data_t *ubxDataStruct); // ZED-F9P Function

void gpsSetUp(){
    UBLOX_SERIAL.begin(gpsBaud);
    for (int i=0; i<10; i++){
        if (sparkFunGNSS.begin(UBLOX_SERIAL)){
            Serial.println(F("GPS Online!"));
            OLED.update("GPS\nOnline!");
            greenLedCounter++;
            break;
        }
        else {
            Serial.println(F("GPS Offline..."));
            OLED.update("GPS\nOffline...");
            //if (usingBuzzer) tone(TONE_PIN, 400);
            digitalWrite(LEDR, LOW);       // will turn the LED on
            RED_LED.on();
        }
        delay(20);
    }
    delay(1000);

    //if (ecefEnabled) sparkFunGNSS.setSerialOutput(COM_TYPE_NMEA | COM_TYPE_UBX); // Turn on both UBX and NMEA sentences on I2C. (Turn off RTCM and SPARTN)
    //else sparkFunGNSS.setSerialOutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    sparkFunGNSS.setPortOutput(COM_PORT_UART2, COM_TYPE_UBX | COM_TYPE_NMEA);
    // sparkFunGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_I2C); // Several of these are on by default on ublox board so let's disable them
    // sparkFunGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_I2C);
    // sparkFunGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_I2C);
    // sparkFunGNSS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_I2C);
    // sparkFunGNSS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_I2C);
    // sparkFunGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_I2C); // Leave only GGA enabled at current navigation rate

    
    sparkFunGNSS.setNavigationFrequency(GPS_FREQUENCY); //Set output to 5 times a second
    byte rate = sparkFunGNSS.getNavigationFrequency(); //Get the update rate of this module
    Serial.println("Current Frequency: " + String(rate));
    if(gpsStatus != GPS_FREQUENCY){
        for(int i=0; i<50; i++){ //reference list of dynamic models can be found at the bottom of this tab. 0 is pedestrian (default), 7 is airborne2g, 8 is airborne4g
            sparkFunGNSS.setNavigationFrequency(GPS_FREQUENCY); //Set output to 5 times a second
            rate = sparkFunGNSS.getNavigationFrequency(); //Get the update rate of this module
            if(rate == GPS_FREQUENCY) break;
            Serial.println("Current Frequency: " + String(rate));
        }
    }

    sparkFunGNSS.setDynamicModel(dynamicModel); //if returns 255 then that is an error, unable to communicate with gps
    gpsStatus = sparkFunGNSS.getDynamicModel();
    Serial.println("Current Airmode: " + String(gpsStatus) + ", attempting to switch to airborne4g/8");
    OLED.update("Airborne\nMode:\n" + String(gpsStatus) + "\n\nAttempt:\n1");
    if(gpsStatus != 8){
        for(int i=0; i<50; i++){ //reference list of dynamic models can be found at the bottom of this tab. 0 is pedestrian (default), 7 is airborne2g, 8 is airborne4g
            sparkFunGNSS.setDynamicModel(dynamicModel); //if returns 255 then that is an error, unable to communicate with gps
            gpsStatus  = sparkFunGNSS.getDynamicModel();
            OLED.update("Airborne\nMode:\n" + String(gpsStatus) + "\n\nAttempt:\n" + String(i));
            Serial.println("Current Airmode: " + String(gpsStatus) + ", attempting to switch to airborne4g/8");
            if(gpsStatus == 8) break;
            
        }
    }
    delay(1000);

    sparkFunGNSS.setMeasurementRate(GPS_DATA_DELAY); //Produce a measurement every 1000ms
    sparkFunGNSS.setAutoPVTrate(1); //Tell the GNSS to send the PVT solution every measurement
    // if (ecefEnabled) sparkFunGNSS.setAutoNAVPOSECEFrate(1); //Tell the GNSS to send each POSECEF solution every 5th measurement
    // sparkFunGNSS.setHighPrecisionMode(false); // Enable High Precision Mode - include extra decimal places in the GGA messages
    
}

void gpsUpdate(){
    //sparkFunGNSS.begin(UBLOX_SERIAL);
    if (gpsStatus == 8) PVTstatus = sparkFunGNSS.getPVT();
    if (gpsStatus == 8 && PVTstatus){
        gpsAltM =  sparkFunGNSS.getAltitude() * pow(10.0, -3); //ublox.getAlt_feet();  //older used in OG pterodactyl
        gpsAltFt = gpsAltM * 3.280839895;
        gpsLat = sparkFunGNSS.getLatitude() * pow(10.0, -7); // ublox.getLat();
        gpsLon = sparkFunGNSS.getLongitude() * pow(10.0, -7); //ublox.getLon();

        gpsLatDec = abs(long(gpsLat * pow(10.0, 7))) % 10000000;
        if (gpsLat < 0) gpsLatInt = floor(gpsLat) + 1;
        else gpsLatInt = floor(gpsLat);

        gpsLonDec = abs(long(gpsLon * pow(10.0, 7))) % 10000000;
        if (gpsLon < 0) gpsLonInt = floor(gpsLon) + 1;
        else gpsLonInt = floor(gpsLon);
        
        SIV = sparkFunGNSS.getSIV();

        if (SIV > 0) BLUE_LED.on();
        else BLUE_LED.off();

        gpsMonth = sparkFunGNSS.getMonth();
        gpsDay = sparkFunGNSS.getDay();
        gpsYear = sparkFunGNSS.getYear();

        gpsHour = sparkFunGNSS.getHour() - 5;  // Their time zone is 5h ahead
        if (gpsHour < 0) gpsHour = gpsHour + 24;
        gpsMinute = sparkFunGNSS.getMinute();
        gpsSecond = sparkFunGNSS.getSecond();
        gpsMillisecond = sparkFunGNSS.getMillisecond();

        gpsTimeOfWeek = sparkFunGNSS.getTimeOfWeek();
        //gpsWNO = sparkFunGNSS.time

        gpsGndSpeed = sparkFunGNSS.getGroundSpeed()* pow(10.0, -3); // mm/s to m/s
        gpsHeading = sparkFunGNSS.getHeading() * pow(10.0, -5); // deg * 10^5 to deg
        gpsPDOP = sparkFunGNSS.getPDOP();

        gpsVertVelFt = (gpsAltFt - gpsPrevAltFt) / (timerSec - gpsPrevTime);
        gpsVertVelM = gpsVertVelFt * 0.3047999995367;
        gpsPrevAltFt = gpsAltFt;
        gpsPrevTime = timerSec;

        byte fixType = sparkFunGNSS.getFixType();
        if(fixType == 0) fixTypeGPS = "No Fix";
        else if(fixType == 1) fixTypeGPS = "Dead reckoning";
        else if(fixType == 2) fixTypeGPS = "2D";
        else if(fixType == 3) fixTypeGPS =  "3D";
        else if(fixType == 4) fixTypeGPS = "GNSS + Dead reckoning";
        else if(fixType == 5) fixTypeGPS = "Time only";

        byte RTK = sparkFunGNSS.getCarrierSolutionType();
        if (RTK == 0) fixTypeRTK = "No RTK";
        else if (RTK == 1) fixTypeRTK = "HPF RTK";
        else if (RTK == 2) fixTypeRTK = "HP RTK"; // High po

        velocityNED[0] = sparkFunGNSS.getNedNorthVel() / 1000.0;
        velocityNED[1] = sparkFunGNSS.getNedEastVel() / 1000.0;
        velocityNED[2] = sparkFunGNSS.getNedDownVel() / 1000.0;

        gpsHorizAcc = sparkFunGNSS.getHorizontalAccEst(); // Print the horizontal accuracy estimate
        gpsVertAcc = sparkFunGNSS.getVerticalAccEst();

        // gpsYaw = sparkFunGNSS.getVehicleHeading();
        // gpsPitch = sparkFunGNSS.getVehiclePitch();
        // gpsRoll = sparkFunGNSS.getVehicleRoll();

    }
    if (gpsStatus == 8) sparkFunGNSS.flushPVT();

    if (PVTstatus) PVTcounter = 0;
    else PVTcounter++;
    Serial.println(PVTcounter);

    if (PVTcounter >= 5) {
        RED_LED.on();
        digitalWrite(LEDR, LOW);       // will turn the LED on
        //if (gpsStatus == 8) sparkFunGNSS.begin(UBLOX_SERIAL);
    }

    if (ecefEnabled){
        // Calling getNAVPOSECEF returns true if there actually is a fresh position solution available.
        ecefStatus = sparkFunGNSS.getNAVPOSECEF();
        if (ecefStatus){
            // updateECEF();
            ecefX = sparkFunGNSS.packetUBXNAVPOSECEF->data.ecefX; // convert ecefX to m
            ecefY = sparkFunGNSS.packetUBXNAVPOSECEF->data.ecefY; // convert ecefY to m
            ecefZ = sparkFunGNSS.packetUBXNAVPOSECEF->data.ecefZ; // convert ecefY to m
            sparkFunGNSS.flushNAVPOSECEF(); //Mark all the data as read/stale so we get fresh data next time
        }
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printPVTdata will be called when new NAV PVT data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_PVT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoPVTcallbackPtr
//        /                  _____  This _must_ be UBX_NAV_PVT_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printPVTdata(UBX_NAV_PVT_data_t *ubxDataStruct)
{
  pvtStatus = true;
  double latitude = ubxDataStruct->lat; // Print the latitude
  Serial.print(F("Lat: "));
  Serial.print(latitude / 10000000.0, 7);

  double longitude = ubxDataStruct->lon; // Print the longitude
  Serial.print(F("  Long: "));
  Serial.print(longitude / 10000000.0, 7);

  double altitude = ubxDataStruct->hMSL; // Print the height above mean sea level
  Serial.print(F("  Height: "));
  Serial.print(altitude / 1000.0, 3);

  uint8_t fixType = ubxDataStruct->fixType; // Print the fix type
  Serial.print(F("  Fix: "));
  Serial.print(fixType);
  if (fixType == 0)
    Serial.print(F(" (None)"));
  else if (fixType == 1)
    Serial.print(F(" (Dead Reckoning)"));
  else if (fixType == 2)
    Serial.print(F(" (2D)"));
  else if (fixType == 3)
    Serial.print(F(" (3D)"));
  else if (fixType == 3)
    Serial.print(F(" (GNSS + Dead Reckoning)"));
  else if (fixType == 5)
    Serial.print(F(" (Time Only)"));
  else
    Serial.print(F(" (UNKNOWN)"));

  uint8_t carrSoln = ubxDataStruct->flags.bits.carrSoln; // Print the carrier solution
  Serial.print(F("  Carrier Solution: "));
  Serial.print(carrSoln);
  if (carrSoln == 0)
    Serial.print(F(" (None)"));
  else if (carrSoln == 1)
    Serial.print(F(" (Floating)"));
  else if (carrSoln == 2)
    Serial.print(F(" (Fixed)"));
  else
    Serial.print(F(" (UNKNOWN)"));

  uint32_t hAcc = ubxDataStruct->hAcc; // Print the horizontal accuracy estimate
  Serial.print(F("  Horizontal Accuracy Estimate: "));
  Serial.print(hAcc);
  Serial.print(F(" (mm)"));

  Serial.println();    
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printRXMCOR will be called when new RXM COR data arrives
// See u-blox_structs.h for the full definition of UBX_RXM_COR_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setRXMCORcallbackPtr
//        /                  _____  This _must_ be UBX_RXM_COR_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printRXMCOR(UBX_RXM_COR_data_t *ubxDataStruct)
{
  rxmStatus = true;
  Serial.print(F("UBX-RXM-COR:  ebno: "));
  Serial.print((double)ubxDataStruct->ebno / 8, 3); //Convert to dB

  Serial.print(F("  protocol: "));
  if (ubxDataStruct->statusInfo.bits.protocol == 1)
    Serial.print(F("RTCM3"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 2)
    Serial.print(F("SPARTN"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 29)
    Serial.print(F("PMP (SPARTN)"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 30)
    Serial.print(F("QZSSL6"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  errStatus: "));
  if (ubxDataStruct->statusInfo.bits.errStatus == 1)
    Serial.print(F("Error-free"));
  else if (ubxDataStruct->statusInfo.bits.errStatus == 2)
    Serial.print(F("Erroneous"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgUsed: "));
  if (ubxDataStruct->statusInfo.bits.msgUsed == 1)
    Serial.print(F("Not used"));
  else if (ubxDataStruct->statusInfo.bits.msgUsed == 2)
    Serial.print(F("Used"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgEncrypted: "));
  if (ubxDataStruct->statusInfo.bits.msgEncrypted == 1)
    Serial.print(F("Not encrypted"));
  else if (ubxDataStruct->statusInfo.bits.msgEncrypted == 2)
    Serial.print(F("Encrypted"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgDecrypted: "));
  if (ubxDataStruct->statusInfo.bits.msgDecrypted == 1)
    Serial.print(F("Not decrypted"));
  else if (ubxDataStruct->statusInfo.bits.msgDecrypted == 2)
    Serial.print(F("Successfully decrypted"));
  else
    Serial.print(F("Unknown"));

  Serial.println();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// // First, let's collect the position data
//     int32_t latitude;
//     int8_t latitudeHp;
//     int32_t longitude;
//     int8_t longitudeHp;
//     int32_t ellipsoid;
//     int8_t ellipsoidHp;
//     int32_t msl;
//     int8_t mslHp;
//     //uint32_t accuracy = sparkFunGNSS.getHorizontalAccuracy();

//     // Defines storage for the lat and lon units integer and fractional parts
//     int32_t lat_int; // Integer part of the latitude in degrees
//     int32_t lat_frac; // Fractional part of the latitude
//     int32_t lon_int; // Integer part of the longitude in degrees
//     int32_t lon_frac; // Fractional part of the longitude

//     float f_ellipsoid;
//     float f_msl;
//     float f_accuracy;

// getHighResLatitude: returns the latitude from HPPOSLLH as an int32_t in degrees * 10^-7
    // getHighResLatitudeHp: returns the high resolution component of latitude from HPPOSLLH as an int8_t in degrees * 10^-9
    // getHighResLongitude: returns the longitude from HPPOSLLH as an int32_t in degrees * 10^-7
    // getHighResLongitudeHp: returns the high resolution component of longitude from HPPOSLLH as an int8_t in degrees * 10^-9
    // getElipsoid: returns the height above ellipsoid as an int32_t in mm
    // getElipsoidHp: returns the high resolution component of the height above ellipsoid as an int8_t in mm * 10^-1
    // getMeanSeaLevel: returns the height above mean sea level as an int32_t in mm
    // getMeanSeaLevelHp: returns the high resolution component of the height above mean sea level as an int8_t in mm * 10^-1
    // getHorizontalAccuracy: returns the horizontal accuracy estimate from HPPOSLLH as an uint32_t in mm * 10^-1

    // If you want to use the high precision latitude and longitude with the full 9 decimal places
    // you will need to use a 64-bit double - which is not supported on all platforms

    // To allow this example to run on standard platforms, we cheat by converting lat and lon to integer and fractional degrees

    // The high resolution altitudes can be converted into standard 32-bit float

    // First, let's collect the position data
    //  latitude = sparkFunGNSS.getHighResLatitude();
    //  latitudeHp = sparkFunGNSS.getHighResLatitudeHp();
    //  longitude = sparkFunGNSS.getHighResLongitude();
    //  longitudeHp = sparkFunGNSS.getHighResLongitudeHp();
    //  ellipsoid = sparkFunGNSS.getElipsoid();
    //  ellipsoidHp = sparkFunGNSS.getElipsoidHp();
    //  msl = sparkFunGNSS.getMeanSeaLevel();
    //  mslHp = sparkFunGNSS.getMeanSeaLevelHp();
    //uint32_t accuracy = sparkFunGNSS.getHorizontalAccuracy();

    // // Calculate the latitude and longitude integer and fractional parts
    // lat_int = latitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
    // lat_frac = latitude - (lat_int * 10000000); // Calculate the fractional part of the latitude
    // lat_frac = (lat_frac * 100) + latitudeHp; // Now add the high resolution component
    // if (lat_frac < 0) // If the fractional part is negative, remove the minus sign
    // {
    //   lat_frac = 0 - lat_frac;
    // }
    // lon_int = longitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
    // lon_frac = longitude - (lon_int * 10000000); // Calculate the fractional part of the longitude
    // lon_frac = (lon_frac * 100) + longitudeHp; // Now add the high resolution component
    // if (lon_frac < 0) // If the fractional part is negative, remove the minus sign
    // {
    //   lon_frac = 0 - lon_frac;
    // }

    // // Calculate the height above ellipsoid in mm * 10^-1
    // f_ellipsoid = (ellipsoid * 10) + ellipsoidHp;
    // // Now convert to m
    // f_ellipsoid = f_ellipsoid / 10000.0; // Convert from mm * 10^-1 to m

    // // Calculate the height above mean sea level in mm * 10^-1
    // f_msl = (msl * 10) + mslHp;
    // // Now convert to m
    // f_msl = f_msl / 10000.0; // Convert from mm * 10^-1 to m

    // // Convert the horizontal accuracy (mm * 10^-1) to a float
    // //f_accuracy = accuracy;
    // // Now convert to m
    // f_accuracy = f_accuracy / 10000.0; // Convert from mm * 10^-1 to m
