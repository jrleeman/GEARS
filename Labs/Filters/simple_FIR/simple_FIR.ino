#include <FIR.h>

#define FILTER_TAP_NUM 8

FIR<float, FILTER_TAP_NUM> fir;


void setup()
{
  Serial.begin(115200); // Start a serial port

  float coef[FILTER_TAP_NUM] = { 1., 1., 1., 1., 1., 1., 1., 1.};

  // Set the coefficients
  fir.setFilterCoeffs(coef);
  fir.getGain();
}

void loop()
{
  int start_time = millis();
  int reading = analogRead(A0);
  Serial.print(reading);
  Serial.print("\t");
  Serial.println(fir.processReading(reading));
  while ((millis() - start_time) < 10);
  {}
}
