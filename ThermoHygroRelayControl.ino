/*************************************************** 
  This is an example for the SHT31-D Humidity & Temp Sensor

  Designed specifically to work with the SHT31-D sensor from Adafruit
  ----> https://www.adafruit.com/products/2857

  These sensors use I2C to communicate, 2 pins are required to  
  interface
 ****************************************************/
 
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <math.h>


bool enableAC = false;
uint8_t loopCnt = 0;
int humiditySetPoint = 60;
int tempSetPoint = 85;
unsigned long int timeOfLastChange = 0;
unsigned long int millisDelayBetweenChange = 3UL*60UL*1000UL;
const int relayPin = PIN2;


Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  //while (!Serial)
  //  delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
    sht31.heater(false);
  }

  if (! alpha4.begin(0x70) ){
    Serial.println("Couldn't find alpha4");
    while(1) delay(1);
  }

  alpha4.clear();

}


void loop() {
  //Serial.println("loop");
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  // convert to farenheit
  float tf = t*1.8 + 32;

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C/F = "); Serial.print(t); Serial.print("/"); Serial.print(tf); Serial.print("\t\t"); 
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.print(h); Serial.print("\t\t"); Serial.println(enableAC);
  } else { 
    Serial.println("Failed to read humidity");
  }

  String temperatureString = String(round(tf));
  String humidityString = String(round(h));

  // first 2 digits temp
  alpha4.writeDigitAscii(0, temperatureString.charAt(0));
  alpha4.writeDigitAscii(1, temperatureString.charAt(1));
  // last 2 digits humidity
  alpha4.writeDigitAscii(2, humidityString.charAt(0));
  alpha4.writeDigitAscii(3, humidityString.charAt(1));
  // update what is displayed
  alpha4.writeDisplay();

  unsigned long int timeSinceLastChange = millis()-timeOfLastChange;
  if(timeOfLastChange == 0){
    timeSinceLastChange = millisDelayBetweenChange+1;
  }
  if(h > humiditySetPoint && timeSinceLastChange > millisDelayBetweenChange){

    enableAC = true;
    Serial.println("Enabling AC due to humidity");
    timeOfLastChange = millis();
  }
  if(tf > tempSetPoint && timeSinceLastChange > millisDelayBetweenChange){
    enableAC = true;
    Serial.println("Enabling AC due to temperature");
    timeOfLastChange = millis();
  }

  if(h < (humiditySetPoint - 2) && tf < (tempSetPoint - 2) && timeSinceLastChange > millisDelayBetweenChange && enableAC == true){
    enableAC = false;
    Serial.println("Disabling AC");
    timeOfLastChange = millis();
  }
  Serial.println(enableAC ? HIGH : LOW);
  digitalWrite(relayPin, enableAC ? HIGH : LOW);
  //Serial.println(timeSinceLastChange);
  //Serial.println(timeOfLastChange);
  //Serial.println(millisDelayBetweenChange);
  if (loopCnt >= 30) {
    loopCnt = 0;
  }

  loopCnt++;
  delay(1000);
}
