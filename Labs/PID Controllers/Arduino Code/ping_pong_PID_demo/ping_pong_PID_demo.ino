/*
 * PID Demo - Ping Pong Ball Float
 * Leeman Geophysical LLC
 * 
 * MOSFET for fans on D3
 * VL53L0X distance sensor
 */

#include "PID_v1.h"
#include "Adafruit_VL53L0X.h"

double set_point = 100;
double process_variable = 0;
double output = 0;

double Kp = 2;
double Ki = 0;
double Kd = 0;

const uint8_t PIN_FANS = 3;

PID controller(&process_variable, &output, &set_point, Kp, Ki, Kd, DIRECT);
Adafruit_VL53L0X dist_sensor = Adafruit_VL53L0X();

float readDistance()
{
  float distance = 0;
  uint8_t navg = 1;
  VL53L0X_RangingMeasurementData_t measurement;
  for (uint8_t i=0; i<navg; i++)
  {
    dist_sensor.rangingTest(&measurement, false);
    if (measurement.RangeStatus != 4)
    {
      distance += measurement.RangeMilliMeter;
    }
  }
  return distance / navg;
}

void setup()
{
  Serial.begin(115200);
  
  if (!dist_sensor.begin())
  {
    Serial.println(F("Distance sensor failure"));
    while(1);
  }
  
  // Setup the fans and make sure they are stopped
  pinMode(PIN_FANS, OUTPUT);
  analogWrite(PIN_FANS, 0);
  
  // Read the current distance
  process_variable = readDistance();

  controller.SetMode(MANUAL);
  output = 255;
  controller.Compute();
  
  // Turn the PID on - this is not automatic tuning!
  controller.SetMode(AUTOMATIC);

  controller.SetSampleTime(100);
}

void loop()
{
  // Check if we need to update parameters
  if (Serial.available())
  {
    for (int x = 0; x < 5; x++)
    {
      switch (x)
      {
        case 0:
          Kp = Serial.parseFloat();
          break;
        case 1:
          Ki = Serial.parseFloat();
          break;
        case 2:
          Kd = Serial.parseFloat();
          break;
        case 3:
          set_point = Serial.parseFloat();
        case 4:
          while(Serial.available())
          {
            Serial.read(); //Clear out any residual junk
          }
          break;
      }
    }
    controller.SetTunings(Kp, Ki, Kd);
  }
      
  // Update and do the control loop
  process_variable = readDistance();
  controller.Compute();
  analogWrite(PIN_FANS, output);

  static uint8_t serial_counter = 0;

  if (serial_counter % 20 == 0)
  {
    // Send the current settings
    Serial.print(millis());
    Serial.print("\t");
    Serial.print(set_point);
    Serial.print("\t");
    Serial.print(process_variable);
    Serial.print("\t");
    Serial.print(output);
    Serial.print("\t");
    Serial.print(Kp, 4);
    Serial.print("\t");
    Serial.print(Ki, 4);
    Serial.print("\t");
    Serial.println(Kd, 4);
  }
  
}
