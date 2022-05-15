#include <MQ135.h>

/*  MQ135 gas sensor
    Datasheet can be found here: https://www.olimex.com/Products/Components/Sensors/SNS-MQ135/resources/SNS-MQ135.pdf

    Application
    They are used in air quality control equipments for buildings/offices, are suitable for detecting of NH3, NOx, alcohol, Benzene, smoke, CO2, etc

    Original creator of this library: https://github.com/GeorgK/MQ135, example adapted by https://github.com/MaxQ22/MQ135 for the ESP32
*/

//Define the pins, the MQ135 is connected to and the reference voltage is connected to
#define PIN_MQ135 35
#define PIN_VREF 34

//Instance of the MQ135 sensor. The values for RL and RZero should be adjusted to the actual values of the used sensor and setup. My sensor happend to have a RZero of 130 kOhm and i used a 44 kOhm load resistor.
MQ135 mq135_sensor(PIN_MQ135, PIN_VREF, 130, 44);

float temperature = 21.0; // Assume current temperature. Recommended to measure with DHT22/BME280 or other temperture + humidity sensor
float humidity = 25.0; // Assume current humidity.  Recommended to measure with DHT22/BME280 or other temperture + humidity sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  delay(300);
}
