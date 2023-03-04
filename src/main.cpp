#include<Arduino.h>
#include <Stepper.h>

#pragma region (Stepper Motor Coils)
    #define COIL_1 8
    #define COIL_2 9
    #define COIL_3 10
    #define COIL_4 11
#pragma endregion

#pragma region (Gearing/Step Vars)
    #define STEP_ANGLE 0.17578125
#pragma endregion

#pragma region (Stepper Control Vars)
    const int stepPerRotation = 360/STEP_ANGLE;
    Stepper stepperControl = Stepper(stepPerRotation, COIL_1, COIL_3, COIL_2, COIL_4);
    int stepperSpeed = 10;
    int deltaSteps = 0;
#pragma endregion

#define TORCH_PIN 2

void StepMotor(int steps){
    //Move stepper
    stepperControl.step(steps);
    //Track global position
    deltaSteps += steps;
}

void SetMotorPosition(int degrees){

    //Calculate steps required for angle
    int stepCount = degrees/STEP_ANGLE;
    //Apply global position
    stepCount -= deltaSteps;

    //Output instruction
    Serial.print("Position: ");
    Serial.print(degrees);
    Serial.print(" Steps: ");
    Serial.print(stepCount);
    Serial.println();

    //Step the motor
    StepMotor(stepCount);
}

//Return to starting position
void MotorReturn(){

    Serial.print("Returning...");
    Serial.print(deltaSteps);
    Serial.println("steps.");

    StepMotor(-deltaSteps);
}

void SetTorchStatus(PinStatus status){
    digitalWrite(TORCH_PIN, status);
}

void setup() {
    //Serial Comms Setup
    Serial.begin(9600);
    Serial.println("Starting...");
    
    //Assign Torch Pin
    pinMode(TORCH_PIN, OUTPUT);

    //Set Stepper Speed
    stepperControl.setSpeed(stepperSpeed);
}

void loop() {
    SetMotorPosition(360);
    delay(1000);
    MotorReturn();
    delay(1000);

    SetTorchStatus(HIGH);
    delay(1000);
    SetTorchStatus(LOW);
    delay(1000);
}