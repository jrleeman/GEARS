#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

Adafruit_BMP3XX bmp;
Adafruit_VL6180X vl = Adafruit_VL6180X();

void setup()
{
  Serial.begin(115200);
  if (! vl.begin()) {
    Serial.println("Failed to find VL6180X sensor");
    while (1);
  }

  if (!bmp.begin_SPI(BMP_CS))
  {
    Serial.println("Failed to find BMP388");
    while (1);
  }
  
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("range\tstatus\ttemperature\tpressure");
}

void loop()
{
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  bmp.performReading();
  Serial.print(range);
  Serial.print("\t");
  Serial.print(status);
  Serial.print("\t");
  Serial.print(bmp.temperature);
  Serial.print("\t");
  Serial.println(bmp.pressure / 100.0);
  delay(1000);
}
