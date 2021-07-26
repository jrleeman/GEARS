/*
 * PID Demo - Thermal Chamber
 * Leeman Geophysical LLC
 * 
 * MOSFET for lamp on D3
 * Thermistor temperature sensor with 10k divider setup on A0
 */

#include "PID_v1.h"

double set_point = 15;
double process_variable = 0;
double output = 0;

double Kp = 1000;
double Ki = 0;
double Kd = 0;

const uint8_t PIN_HEATER = 3;
const uint8_t PIN_THERMISTOR = A0;

PID controller(&process_variable, &output, &set_point, Kp, Ki, Kd, DIRECT);
  
double readTemperature()
{
  // Get an average of 50 readings
  uint16_t adc_bits = 0;
  for (uint8_t i=0; i<50; i++)
  {
    adc_bits += analogRead(PIN_THERMISTOR); 
  }
  adc_bits /= 50;
  
  // Convert the bits to resistance of the thermistor
  float resistance = 10000. / (1023. / adc_bits - 1);
  // Convert the resistance to a temperature in degC
  float temperature = resistance / 10000; // (R/Ro)
  temperature = log(temperature); // ln(R/Ro)
  temperature /= 3950; // 1/B * ln(R/Ro)
  temperature += 1.0 / (25 + 273.15); // + (1/To)
  temperature = 1.0 / temperature; // Invert
  temperature -= 273.15; // convert absolute temp to degC
  return temperature;
}

void setup()
{
  Serial.begin(115200);
  
  // We've connected AREF to 3.3V to reduce the noise
  analogReference(EXTERNAL);
  
  // Setup the heater and make sure it is off
  pinMode(PIN_HEATER, OUTPUT);
  analogWrite(PIN_HEATER, 0);
  
  // Read the current temperature
  process_variable = readTemperature();
  
  controller.SetMode(MANUAL);
  output = 100;
  controller.Compute();
  
  // Turn the PID on - this is not automatic tuning!
  controller.SetMode(AUTOMATIC);

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
    Serial.println("SET");
    Serial.println(Kp, 4);
    Serial.println(Ki, 4);
    Serial.println(Kd, 4);
    Serial.println(set_point, 4);
    controller.SetTunings(Kp, Ki, Kd);
  }
      
  // Update and do the control loop
  process_variable = readTemperature();
  controller.Compute();
  analogWrite(PIN_HEATER, output);

  static uint8_t serial_counter = 0;

  if (serial_counter % 10 == 0)
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
  serial_counter++;
}
