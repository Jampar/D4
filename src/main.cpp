#include <Arduino.h>
#include <Wire.h>
#include <vl53lx_class.h>
#include <Stepper.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define LedPin LED_BUILTIN
#define TORCH_PIN 2

#pragma region (Stepper Motor Coils)
   #define COIL_1 8
   #define COIL_2 9
   #define COIL_3 10
   #define COIL_4 11

   #define STEP_ANGLE 0.17578125

   const int stepPerRotation = 360/STEP_ANGLE;
   Stepper stepperControl = Stepper(stepPerRotation, COIL_1, COIL_3, COIL_2, COIL_4);
   int stepperSpeed = 10;
   int deltaSteps = 0;
#pragma endregion


VL53LX sensor_vl53lx_sat(&Wire, 25); // Sensor Addressing
int16_t prevDist = 0;
int distChangeThreshold = 20;

void SetupDistanceFinder(){
   //---Init Sensor procedure---
   sensor_vl53lx_sat.begin();
   sensor_vl53lx_sat.VL53LX_Off();
   sensor_vl53lx_sat.InitSensor(0x12);
   sensor_vl53lx_sat.VL53LX_StartMeasurement();
}

int16_t GetDistance(){

   //Ranging Data variable & pointer
   VL53LX_MultiRangingData_t MultiRangingData;
   VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;

   //Data flag
   uint8_t NewDataReady = 0;

   //Multiple object count
   int no_of_object_found = 0, j;
   //Return status
   int status;

   //Retrieve distance data when available
   do
   {
      status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
   } while (!NewDataReady);

   //Indicator LED
   digitalWrite(LedPin, HIGH);

   //Retrieve Data, if status error - reset interrupts
   if((!status)&&(NewDataReady!=0))
   {
      status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
      if (status==0)
      {
         status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
      }
   }

   //Disable Inidicator
   digitalWrite(LedPin, LOW);

   return pMultiRangingData->RangeData[j].RangeMilliMeter;
}

int16_t GetMeanDistance(){
   int sampleCount = 1;
   int total = 0;
   for(int i = 0; i < sampleCount; i++){
      int dist = GetDistance();
      if(dist > 3000) i--;
      else total += GetDistance();
   }
   int16_t averageDist = total/sampleCount;
   if(abs(averageDist - prevDist) > distChangeThreshold){
      prevDist = averageDist;
      return averageDist;
   }else{
      return prevDist;
   }
}

void setup()
{
   // Led.
   pinMode(LedPin, OUTPUT);
   pinMode(TORCH_PIN, OUTPUT);

   // Initialize serial for output.
   Serial.begin(9600);
   Serial.println("Starting...");

   // Initialize I2C bus.
   Wire.begin();

   SetupDistanceFinder();
}

void loop()
{
   int16_t dist = GetMeanDistance();

   if(dist < 100) digitalWrite(TORCH_PIN,HIGH);
   else digitalWrite(TORCH_PIN,LOW);
}