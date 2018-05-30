/*
 Started from: Weather Shield Example by Nathan Seidle (SparkFun Electronics)
 By: Robrecht Wellens

 */
#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFun_Si7021_Breakout_Library.h" //Humidity sensor - Search "SparkFun Si7021" and install from Library Manager
#include <ArduinoJson.h>

MPL3115A2 myPressure; //Create an instance of the pressure sensor
Weather myHumidity; //Create an instance of the humidity sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte STAT_BLUE = 7;
const byte STAT_GREEN = 8;

const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool started = false;
long lastSecond; //The millis counter to see when a second rolls by
long elapsed;
long interval;

int pressureArray[10] = {0};
byte counter = 0;
byte deltaTime = 0;
int month = 5; // TODO: Fetch from rtc or something
int altitude = 32; // meters above sea level

/*
 * station2sealevel verified, working correctly *
 */
int station2sealevel(int p, int height, int t){
  return (double) p*pow(1-0.0065*(double)height/(t+0.0065*(double)height+273.15),-5.275);
}

/*
 * calcZambretti
 */
int calcZambretti(int curr_pressure, int prev_pressure, int mon){
        if (curr_pressure<prev_pressure){
          //FALLING
          if (mon>=4 and mon<=9)
          //summer
          {
            if (curr_pressure>=1030)
              return 2;
            else if(curr_pressure>=1020 and curr_pressure<1030)
              return 8;
            else if(curr_pressure>=1010 and curr_pressure<1020)
              return 18;
            else if(curr_pressure>=1000 and curr_pressure<1010)
              return 21;
            else if(curr_pressure>=990 and curr_pressure<1000)
              return 24;
            else if(curr_pressure>=980 and curr_pressure<990)
              return 24;
            else if(curr_pressure>=970 and curr_pressure<980)
              return 26;
            else if(curr_pressure<970)
              return 26;
          }
          else{
          //winter
            if (curr_pressure>=1030)
              return 2;
            else if(curr_pressure>=1020 and curr_pressure<1030)
              return 8;
            else if(curr_pressure>=1010 and curr_pressure<1020)
              return 15;
            else if(curr_pressure>=1000 and curr_pressure<1010)
              return 21;
            else if(curr_pressure>=990 and curr_pressure<1000)
              return 22;
            else if(curr_pressure>=980 and curr_pressure<990)
              return 24;
            else if(curr_pressure>=970 and curr_pressure<980)
              return 26;
            else if(curr_pressure<970)
              return 26;
          }
        }
        else if (curr_pressure>prev_pressure){
          //RAISING
          if (mon>=4 and mon<=9){
            //summer
            if (curr_pressure>=1030)
              return 1;
            else if(curr_pressure>=1020 and curr_pressure<1030)
              return 2;
            else if(curr_pressure>=1010 and curr_pressure<1020)
              return 3;
            else if(curr_pressure>=1000 and curr_pressure<1010)
              return 7;
            else if(curr_pressure>=990 and curr_pressure<1000)
              return 9;
            else if(curr_pressure>=980 and curr_pressure<990)
              return 12;
            else if(curr_pressure>=970 and curr_pressure<980)
              return 17;
            else if(curr_pressure<970)
              return 17;
          }
          else
            //winter
           {
            if (curr_pressure>=1030)
              return 1;
            else if(curr_pressure>=1020 and curr_pressure<1030)
              return 2;
            else if(curr_pressure>=1010 and curr_pressure<1020)
              return 6;
            else if(curr_pressure>=1000 and curr_pressure<1010)
              return 7;
            else if(curr_pressure>=990 and curr_pressure<1000)
              return 10;
            else if(curr_pressure>=980 and curr_pressure<990)
              return 13;
            else if(curr_pressure>=970 and curr_pressure<980)
              return 17;
            else if(curr_pressure<970)
              return 17;
           }
        }
        else{
            if (curr_pressure>=1030)
              return 1;
            else if(curr_pressure>=1020 and curr_pressure<1030)
              return 2;
            else if(curr_pressure>=1010 and curr_pressure<1020)
              return 11;
            else if(curr_pressure>=1000 and curr_pressure<1010)
              return 14;
            else if(curr_pressure>=990 and curr_pressure<1000)
              return 19;
            else if(curr_pressure>=980 and curr_pressure<990)
              return 23;
            else if(curr_pressure>=970 and curr_pressure<980)
              return 24;
            else if(curr_pressure<970)
              return 26;

        }

}

void setup()
{
  Serial.begin(9600);
  Serial.println("Weather station");

  pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
  pinMode(STAT_GREEN, OUTPUT); //Status LED Green

  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);

  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
 // myPressure.setModeAltimeter(); // Measure altitude above sea level in meters
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();
  lastSecond = millis();
  interval = 30000;//(1000 * 60); // * 5;

  

  Serial.println("Weather Shield online!");
}

void loop()
{
  char zambretti[10]="N/A";
  int Z=0;
  char pressureHistory[57];
  int seaPressure = 0;
  String forecast = "";
  String output = "";

  DynamicJsonBuffer jsonBuffer;
  JsonObject& outputData = jsonBuffer.createObject();
  
  
  // Print readings every minute
  elapsed = millis() - lastSecond;
  if(started == false or elapsed >= interval) {
    
    started = true;
    lastSecond += interval;

    digitalWrite(STAT_BLUE, HIGH); //Blink stat LED

    //Check Humidity Sensor
    float humidity = myHumidity.getRH();

    if (humidity == 998) {
      // Humidty sensor failed to respond
      Serial.println("I2C communication to sensors is not working. Check solder connections.");

      //Try re-initializing the I2C comm and the sensors
      myPressure.begin(); 
      myPressure.setModeBarometer();
      myPressure.setOversampleRate(7);
      myPressure.enableEventFlags();
      myHumidity.begin();
    }
    else {
      float temp_h = myHumidity.getTemp();
      //float altitude = myPressure.readAltitude();
      float pressure = myPressure.readPressure()/100;
      float temp = myPressure.readTemp();
      float light_lvl = get_light_level();
      float batt_lvl = get_battery_level();

      // Get pressure based on sealevel
      seaPressure = station2sealevel(pressure, altitude, temp);
      //Serial.println(seaPressure);
      
      // Keep pressure history
      deltaTime++;
      if(deltaTime > 10) {
        deltaTime = 0;

        // If 10 saves, delete the oldest
        if(counter == 10) {
          for(int i=0;i<9;i++) {
            pressureArray[i] = pressureArray[i+1];
          }
          pressureArray[counter-1] = seaPressure; 
        }
        else {
          pressureArray[counter] = seaPressure;  
          counter++;
        } 
      }

      // Get rate with Zambretti algorithm
      Z = calcZambretti((pressureArray[9]+pressureArray[8]+pressureArray[7])/3,(pressureArray[0]+pressureArray[1]+pressureArray[2])/3, month);
      //Serial.print(" Z = ");
      //Serial.println(Z);

      // Try doing weather forecast
      if(pressureArray[9] > 0 and pressureArray[0] > 0) {
        if(pressureArray[9]+pressureArray[8]+pressureArray[7]-pressureArray[0]-pressureArray[1]-pressureArray[2] >= 3) {
          //RAISING
          forecast += "De luchtdruk is aan het stijgen. ";
          if(Z<3) {
            forecast += "Er zijn brede opklaringen. ";
          }
          else if(Z >= 3 and Z <= 9) {
            forecast += "Het is zonnig en matig bewolkt. ";
          }
          else if(Z > 9 and Z <= 17) {
            forecast += "Het is eerder bewolkt. ";
          }
          else if(Z > 17) {
            forecast += "Het is regenachtig. ";
          }
        }
        else if(pressureArray[0]+pressureArray[1]+pressureArray[2]-pressureArray[9]-pressureArray[8]-pressureArray[7] >= 3) {
          //FALLING
          forecast += "De luchtdruk is aan het dalen. ";
          if (Z<4) {
            forecast += "Er zijn brede opklaringen. ";
          }
          else if(Z>=4 and Z<14) {
            forecast += "Het is zonnig en matig bewolkt. ";
          }
          else if(Z>=14 and Z<19) {
            forecast += "Het weer verslechterd. ";
          }
          else if(Z>=19 and Z<21) {
            forecast += "Het is eerder bewolkt. ";
          }
          else if(Z>=21) {
            forecast += "Het is regenachtig. ";
          }
        }
        else {
          //STEADY
          forecast += "De luchtdruk is stabiel. ";
          if(Z<5) {
            forecast += "Er zijn brede opklaringen. ";
          }
          else if (Z>=5 and Z<=11) {
            forecast += "Het is zonnig en matig bewolkt. ";
          }
          else if (Z>11 and Z<14) {
            forecast += "Het is eerder bewolkt. ";
          }
          else if (Z>=14 and Z<19) {
            forecast += "Het weer verslechterd. ";
          }
          else if (Z>19) {
            forecast += "Het is regenachtig. ";
          }
        }
      } 
      else {
        forecast += "De huidige luchtdruk bedraagt " + String(pressure) + " HPa. ";
        //forecast += "Het station heeft " + String(counter*10) + "% van de nodige gegevens verzameld. ";
        if (seaPressure < 1005) {
          forecast += "Het is regenachtig. ";
        }  
        else if (seaPressure >= 1005 and seaPressure <= 1014) {
          forecast += "Het is eerder bewolkt. ";
        } 
        else if (seaPressure > 1014 and seaPressure < 1025) {
          forecast += "Het is zonnig en matig bewolkt. ";
        }
        else {
          forecast += "Het is regenachtig. ";
        }
      }
      Serial.print("[");
      // Building JSON output
      outputData["humidity"] = humidity;
      outputData["tempH"] = temp_h;
      outputData["temp"] = temp;
      outputData["seaPressure"] = seaPressure;
      outputData["pressure"] = pressure;
      outputData["altitude"] = altitude;
      outputData["lightLvl"] = light_lvl;
      outputData["batteryLvl"] = batt_lvl;
      outputData["Z"] = Z;

      outputData.prettyPrintTo(Serial);

      jsonBuffer.clear();
      
      Serial.println(",");

      JsonObject& pHistory = jsonBuffer.createObject();
      JsonArray& data = pHistory.createNestedArray("data");
      for (int j = 0; j < 10; j++) {
        data.add(pressureArray[j]);
      }

      // Print to Serial
      pHistory.prettyPrintTo(Serial);

      jsonBuffer.clear();
      
      Serial.println(",");

      JsonObject& text = jsonBuffer.createObject();
      text["forecast"] = forecast;

      // Print to Serial
      text.prettyPrintTo(Serial);
      
      Serial.println("]");

       jsonBuffer.clear();
    }
    
    Serial.println();
    
    digitalWrite(STAT_BLUE, LOW); //Turn off stat LED

    
  } // End interval code

  delay(100);
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float lightSensor = analogRead(LIGHT);

  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

  lightSensor = operatingVoltage * lightSensor;

  return (lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float rawVoltage = analogRead(BATT);

  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

  return (rawVoltage);
}
