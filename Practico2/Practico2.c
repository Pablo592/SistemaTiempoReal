#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "AHT10.h"

AHT10 aht;
Adafruit_BMP280 bmp;

void setup()
{
    Serial.begin(9600);

    aht.begin();

    if (!bmp.begin())
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1)
            ;
    }

    delay(1000);
    Serial.println("| Temp AHT10 | Temp BMP | Presión BMP | Humedad AHT10 |");
}

void loop()
{

    float humedad = aht.readHumidity();
    float tempaht10 = aht.readTemperature();
    float tempbmp = bmp.readTemperature();
    float presion = (bmp.readPressure() / 100);

    Serial.print("| ");
    Serial.print(tempaht10);
    Serial.print(" ºC  | ");
    Serial.print(tempbmp);
    Serial.print(" ºC | ");
    Serial.print(presion);
    Serial.print(" hPa  |    ");
    Serial.print(humedad);
    Serial.print(" %   |");

    Serial.println();
    delay(1000);
}
