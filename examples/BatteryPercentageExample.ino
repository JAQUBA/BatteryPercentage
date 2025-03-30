#include <BatteryPercentage.h>

BatteryPercentage battery(5.0, 1023, 2.0, BatteryType::LiPo);

void setup() {
    Serial.begin(9600);
}

void loop() {
    uint16_t adcValue = analogRead(A0); // Odczyt z pinu analogowego
    float voltage = battery.getVoltage(adcValue);
    float percentage = battery.getPercentage(adcValue);

    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" V, Percentage: ");
    Serial.print(percentage);
    Serial.println(" %");

    delay(1000);
}