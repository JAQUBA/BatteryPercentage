#include "BatteryPercentage.h"

// LUT: wartości napięcia w V dla różnych typów baterii
static const float lipoVoltageLUT[] = {2.2, 2.5, 2.8, 3.0, 3.3, 3.6, 3.9, 4.2};
static const float liionVoltageLUT[] = {2.5, 2.8, 3.0, 3.3, 3.6, 3.9, 4.1, 4.2};
static const float nimhVoltageLUT[] = {0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6};
static const float nicdVoltageLUT[] = {0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6};
static const float leadAcVoltageLUT[] = {1.75, 1.8, 1.85, 1.9, 1.95, 2.0, 2.1, 2.2};
static const float alkalineVoltageLUT[] = {0.8, 1.0, 1.2, 1.3, 1.4, 1.5, 1.6, 1.65};
static const float zincCarbonVoltageLUT[] = {0.8, 1.0, 1.2, 1.3, 1.4, 1.5, 1.6, 1.65};

// LUT: odpowiadające wartości procentowe
static const float defaultPercentLUT[] = {0, 10, 20, 40, 60, 80, 90, 100};

BatteryPercentage::BatteryPercentage(float vRef, float adcMax, float kDivider, BatteryType type, int numBatteries)
    : vRef(vRef), adcMax(adcMax), kDivider(kDivider), type(type), numBatteries(numBatteries) {
    setLUTs();
}

void BatteryPercentage::setLUTs() {
    switch (type) {
        case BatteryType::LiPo:
            voltageLUT = lipoVoltageLUT;
            lutSize = sizeof(lipoVoltageLUT) / sizeof(lipoVoltageLUT[0]);
            break;
        case BatteryType::LiIon:
            voltageLUT = liionVoltageLUT;
            lutSize = sizeof(liionVoltageLUT) / sizeof(liionVoltageLUT[0]);
            break;
        case BatteryType::NiMH:
            voltageLUT = nimhVoltageLUT;
            lutSize = sizeof(nimhVoltageLUT) / sizeof(nimhVoltageLUT[0]);
            break;
        case BatteryType::NiCd:
            voltageLUT = nicdVoltageLUT;
            lutSize = sizeof(nicdVoltageLUT) / sizeof(nicdVoltageLUT[0]);
            break;
        case BatteryType::LeadAc:
            voltageLUT = leadAcVoltageLUT;
            lutSize = sizeof(leadAcVoltageLUT) / sizeof(leadAcVoltageLUT[0]);
            break;
        case BatteryType::Alkaline:
            voltageLUT = alkalineVoltageLUT;
            lutSize = sizeof(alkalineVoltageLUT) / sizeof(alkalineVoltageLUT[0]);
            break;
        case BatteryType::ZincCarbon:
            voltageLUT = zincCarbonVoltageLUT;
            lutSize = sizeof(zincCarbonVoltageLUT) / sizeof(zincCarbonVoltageLUT[0]);
            break;
        default:
            voltageLUT = lipoVoltageLUT;
            lutSize = sizeof(lipoVoltageLUT) / sizeof(lipoVoltageLUT[0]);
            break;
    }
    percentLUT = defaultPercentLUT;
}

float BatteryPercentage::getVoltage(uint16_t adcValue) {
    // Oblicz napięcie na podstawie wartości ADC, napięcia referencyjnego i liczby baterii
    return (adcValue / adcMax) * vRef * kDivider;
}

float BatteryPercentage::getPercentage(uint16_t adcValue) {
    float voltage = (adcValue / adcMax) * vRef * kDivider / numBatteries;

    // Interpolacja liniowa na podstawie LUT
    for (size_t i = 0; i < lutSize - 1; i++) {
        if (voltage >= voltageLUT[i] && voltage <= voltageLUT[i + 1]) {
            float v1 = voltageLUT[i];
            float v2 = voltageLUT[i + 1];
            float p1 = percentLUT[i];
            float p2 = percentLUT[i + 1];
            return p1 + (voltage - v1) * (p2 - p1) / (v2 - v1);
        }
    }

    // Poza zakresem LUT
    if (voltage < voltageLUT[0]) return 0.0f;
    if (voltage > voltageLUT[lutSize - 1]) return 100.0f;
    return 0.0f; // Domyślna wartość, jeśli coś pójdzie nie tak
}
