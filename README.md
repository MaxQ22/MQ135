# MQ135 GAS SENSOR ESP32 Fork
This is a fork of the incredible Arduino Library for the MQ135 gas sensor by Phoenix1747, adopted for usage with the ESP32. Despite the inferior performance of the ADC (Analog to Digital Converter) of the ESP32 compared to the Atmel Chips on the Arduinos in terms of linearity and measurement accuracy, i wanted to use the MQ135 with the ESP32s ADC and transmit the values via WIFI. This needed two adjustments to the MQ135 library. First adjustment to be made is the change from a 10 bit ADC (as found on most Arduinos) to a 12 bit ADC, as present on the ESP32. The second adjustment to be made is the voltage level. The MQ135 sensor requires a 5V+-0.1V supply voltage. This means, the output signal needs to be adjusted, so that we get a 0 to 3.3V reading for the ESP32. The concept of this adjustment is to dimension the voltage divider in such a way, that the sense signal voltage swings between 0 and 3.3V.

## Application

This type of sensor is used in air quality control equipments for buildings/offices and is suitable for detecting of NH3, NOx, alcohol, Benzene, smoke, CO2, etc.

## Features

This library has:
 - Corrections for temperature and humidity
 - Compatibility with the ESP32 chip, using the Arduino Core
 - Measurement of the Reference voltage to enable Kelvin-Sensing
 - Measurements:
    - getResistance
    - getCorrectedResistance
    - getPPM
    - getCorrectedPPM
    - getRZero
    - getCorrectedRZero

## Library Usage

## Installation
To install the library, just copy the MQ135.cpp and MQ135.h files to your .../arduino/libraries folder.

Then include the MQ135.h file in your project

```cpp
#include <MQ135.h>
```

### Calibration

To get an accurate ppm reading it is important to calibrate the sensor.

To do so put your sensor outside where there is enough fresh air (ideally at 20°C & 33% humidity according to the datasheet) and leave it powered on for 12-24 hours to burn it in and stabilize.

Then you can read out the calibrated resistance value `RZERO` like this (needs to be done when still outside!):

```cpp
float rzero = gasSensor.getRZero();
```

The best way to do this is to average multiple readings to fight ADC noise.
To finish the calibration process you now only need to pass your `RZERO` value to the constructor like this:

```cpp
MQ135 gasSensor(PIN_MQ135, RZERO);
```

### Sensing concept
The MQ135 sensor itself features 4 pins. Two pins are the 5V supply and ground for the heating element, the two are the connections to the variable resistance, that changes with gas concentration. One of those is connected to a 5V reference voltage and the other one is connected to a reference resistor, called RL in the datasheet. This is the actual measurement signal. The internal sense resistance of the sensor, that changes with gas concentration and the RL resistor form a voltage divider. Via backwards calculation of this voltage divider, the resistance of the sense resistor can be calculated. The MQ135 is wired in this application as shown in the following schematic:

 ![MQ135 Schematic](/Schematic.PNG)

### Adjusting the sense signal to 3.3V
For usage with the ESP32 The RL reference resistor has to be determined, so that the measurement signal does not exceed 3.3V for the ESP32. As the minimal resistance of the sensor element is 30 kOhms, and the maximum allowed supply voltage is 5.1 V, we can determine the necessary value of the reference resistor. For this calculation, we have to take into account, that the 30 kOhms value is for 100 ppm NH3 gas concentration and it will go down to a factor of 0.8 of that value for higher gas concentrations.

```
RL = ( 3.3V * 30kOhm*0.8 ) / ( 5.1V - 3.3V ) = 44kOhm
```

While this value of reference resistor should be o.k. to be used with the ESP32, of course, to maximize sensitivity and resolution, it is best to measure each individual MQ135s R0 and calculate the corresponding RL. If the R0 of your particular sensor is at the maximum value given in the datasheet of 200 kOhm, the reference resisstor should be 293kOhm, to still get no output voltage above 3.3V and not waist a lot of the resoluation of the ESPs ADC.

On the Arduino, the 5V supply voltage is also used as the reference voltage for the ADC converter. The original library uses this property, so that the reference voltage does not need to be measured. As the MQ135 needs to be supplied with 5V, according to the datasheet, this trick does not work anymore with the ESP32. So another voltage divider (R1 and R2 in the schematic above) is added, to divide the reference voltage by 2 and subsequently measure it with the ESP32 at Node Vref. 

This gives another possiblity to enhance the measurement. In the MQ135, the H+ and H- pins are used to supply the heating element, while the A and B pins are used to connect to the sense resistor, that changes with gas concentration. This principle is called a "Kelvin measurement" and prevents an influence of the high current draw of the heating element on the measurement. Unfortunatly, in most cheap MQ135 boards, the H+ and A pins are short circuited and connected via one pin only (VCC). This means, the huge current draw of the heating element (~140mA) leads to a voltage drop over the wires and the connectors. Those parasitic resistances are included in the schematic above as RPar+ and Rpar-. It is recommended to solder the R1/R2 divider directly to the A-pin of the actual MQ135 sensor on the break out board, as otherwise, this voltage drop of several millivolts leads to a measurement error. If the reference resistor is also populated on the break out board, it is recommended to unsolder this resistor, solder it only to the pad on the board, connecting to the B pin of the MQ135 and connect the other resistor terminal via a soldered jumper wire directly to GND of the ESP32.

Using this schematic and measureing the VSense and VRef nodes with the ESP32s ADC, the sense resistor (Rsense) value can be calculated as follows:
```
RSense = RL * ( VRef * 2 - VSense ) / VSense
```

### Adjusting for the 12-bit ADC
Adjusting the library to the 12-bit ADC of the ESP32 is trivial. In a 12bit-ADC of the ESP32, the measured voltage is calculated by the following formula:

```
V = 3.3 V/(4096-1) * Code
V = 0.00080586 * Code
```

So the sense resistance is calculated with:

```
RSense = RRef * ( VRefCode * 0.00080586 * 2 - VSenseCode * 0.00080586 ) / ( VSenseCode * 0.00080586 )
```

This formula is implemented in the MQ135::getResistance() function. Additionally, a new parameter is added to the constructor of the MQ135 class, to pass the pin number, the VRef voltage is measured with. From there on, the original Arduino library is used unchanged, to calculate the actual gas concentration based on the measured sense resistance.


## Datasheet

The datasheet of the MQ135 gas sennsor can be found [here](https://www.olimex.com/Products/Components/Sensors/SNS-MQ135/resources/SNS-MQ135.pdf).

The example for the temperature and humidity copensation uses a BME280 to compensate for temperature and humidity effects. The datasheet of the BME280 can be found [here](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf). To read out the values from the BME280, the Adafruit library is used in the example, which can be found [here](https://github.com/adafruit/Adafruit_BME280_Library).

For more information on the original Arduino library visit:
https://hackaday.io/project/3475-sniffing-trinket/log/12363-mq135-arduino-library