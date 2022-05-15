#include <MQ135.h>
#include <Adafruit_BME280.h>

/* MQ135 + DHT Temp Sensor

   Combination of the MQ135 air quality sensor and a Bosch BME280 sensor to accurately measure ppm values through the library correction.
   Uses the Adafruit BME 280 library https://github.com/adafruit/Adafruit_BME280_Library

   Written by: https://github.com/MaxQ22/MQ135 based on the DHT11/22 example by https://github.com/Phoenix1747/MQ135
*/

//Pin Definitions
#define SDA 23
#define SCL 22
#define PIN_MQ135 35
#define PIN_VREF 34

//The Values for RZero and RL should be adjusted to the actual setup. My RZero was 130 kOhm and the used Load Resistor 44 kOhm
MQ135 mq135_sensor(PIN_MQ135, PIN_VREF, 130, 44);
//Instance of the BME280
Adafruit_BME280 bme; 

float temperature, humidity, pressure; // Temp, Pres and Humid floats, will be measured by the BME280

void setup() {
  Serial.begin(9600);

  //Initialize the I2C for the BME280 Sensor
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  Wire.begin(SDA, SCL);

  //Initialize the BME280 sensor. If it is not found, then display a error message
  unsigned status;
  status = bme.begin(0x76, &Wire);

  if (!status)
  {
      Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                       "try a different address!"));
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
   }
}

void loop() 
{
  //Read temperature and humidity for the compensation and pressure just out of interest
  humidity = bme.readHumidity();
  temperature = bme.readTemperature();
  pressure = bme.readPressure();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature) || isnan(pressure))
  {
    Serial.println(F("Failed to read from BME280 sensor!"));
    return;
  }

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
  Serial.print("ppm");
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.print("ppm");
  Serial.print("\t Temperature: ");
  Serial.print(temperature);
  Serial.print("°C");
  Serial.print("\t Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("\t Pressure: ");
  Serial.print(pressure);
  Serial.println("Pa");

  delay(300);
}
