/****************************************************************
* Started     : 03/Jul/2023 - Updated: 05/Jul/2023
* Author      : Andrew Kennedy
* Contributors: Cristina Matsunaga
                Nicole Victory
                Luiz Eduardo
* Description : Line Follower PID with the microcontroller Vespa 
*               from Robocore and the Pololu's QTR-8RC sensor                      
****************************************************************/

#include<RoboCore_Vespa.h> //Library for the Vespa microcontroller
VespaMotors motor;
#include <QTRSensors.h> //Library for the QTR-8A or the QTR-8RC
QTRSensors qtr;

float P=0, I=0, D=0, PID=0, error=0, lastError=0;
int lSpeed, rSpeed;

//Setup of the module of sensors:
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

//------------------PID Control------------------- 
float Kp=1;
float Ki=0.0001;
float Kd=1;

byte maxSpeed = 150; 
bool isLineBlack = false;
//-------------------------------------------------

void setup() {
  qtr.setTypeRC(); //For QTR-8RC      Sensor pins:
  qtr.setSensorPins((const uint8_t[]){17, 16, 18, 5, 23, 19, 22, 21}, SensorCount);

  Serial.begin(9600);
  delay(100);
  pinMode(15, OUTPUT); 
  digitalWrite(15, HIGH); //Turn on the builtin LED to indicate calibration

  for (uint16_t i = 0; i < 160; i++){//4 seconds
    qtr.calibrate();
  }

  for (uint8_t i = 0; i < SensorCount; i++){
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++){
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  digitalWrite(15, LOW); // Turn off Arduino's LED to indicate the end of the calibration
  delay(1000);
}

void loop() {
  //Read sensors
  error = map(readSensors(), 0, 7000, -1000, 1000);

  //Calculate PID
  P = error;
  I = I + error;
  D = error - lastError;
  PID = (Kp*P) + (Ki*I) + (Kd*D);
  lastError = error;

  //Control Motors
  lSpeed = maxSpeed + PID;
  rSpeed = maxSpeed - PID;

  lSpeed = constrain(lSpeed, -maxSpeed, maxSpeed);
  rSpeed = constrain(rSpeed, -maxSpeed, maxSpeed);
  forwardOverride(20);

  motor.turn(lSpeed, rSpeed);
}

int readSensors() {
  if(isLineBlack == true) {
    return qtr.readLineBlack(sensorValues);
  }else{//White line
    return qtr.readLineWhite(sensorValues);
  }
}

void forwardOverride(int margin) {
  if (error >= -margin and error <= margin) {
    motor.turn(maxSpeed, maxSpeed);
  }
}