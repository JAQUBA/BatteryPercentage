#ifndef BATTERYPERCENTAGE_H
#define BATTERYPERCENTAGE_H

#include <Arduino.h>

enum class BatteryType {
    LiPo,
    LiIon,
    NiMH,
    NiCd,
    LeadAc,
    Alkaline,
    ZincCarbon
};

class BatteryPercentage {
private:
    float vRef;       // Napięcie referencyjne ADC
    float adcMax;     // Maksymalna wartość odczytu ADC
    float kDivider;   // Współczynnik dzielnika napięcia
    BatteryType type; // Typ baterii
    int numBatteries; // Liczba baterii połączonych szeregowo

    const float* voltageLUT;
    const float* percentLUT;
    size_t lutSize;

    void setLUTs();

public:
    BatteryPercentage(float vRef, float adcMax, float kDivider, BatteryType type, int numBatteries = 1);

    float getVoltage(uint16_t adcValue);
    float getPercentage(uint16_t adcValue);
};

#endif // BATTERYPERCENTAGE_H
