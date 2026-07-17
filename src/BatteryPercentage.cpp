#include "BatteryPercentage.h"

// ============================================================================
// LUT: Napięcie (V/cell) <-> SOC (%) przy 25°C i napięciu spoczynkowym
// ============================================================================

// LiPo: 2.5V (empty) to 4.2V (full)
// Charakterystyka: szybki spadek na końcu pełnego ładowania, długi plateau, kolaps poniżej 3V
static const float lipoVoltageLUT[] = {2.5, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 3.9, 4.0, 4.1, 4.15, 4.2};
static const float lipoPercentLUT[] = {0, 5, 10, 15, 25, 40, 60, 75, 85, 92, 96, 99, 100};

// LiIon: 2.5V (empty) to 4.2V (full)
// Charakterystyka: bardzo podobna do LiPo, lekko bardziej płaska w plateau
static const float liionVoltageLUT[] = {2.5, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 3.9, 4.0, 4.1, 4.15, 4.2};
static const float liionPercentLUT[] = {0, 5, 10, 15, 25, 40, 60, 75, 85, 92, 96, 99, 100};

// NiMH: 0.9V (empty) to 1.45V (full)
// Charakterystyka: bardzo płaska krzywa, niemal liniowe spadanie, brak ostrych przejść
static const float nimhVoltageLUT[] = {0.9, 0.95, 1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45};
static const float nimhPercentLUT[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 95, 100};

// NiCd: 0.9V (empty) to 1.35V (full)
// Charakterystyka: płaska krzywa z nagłym spadkiem na końcu
static const float nicdVoltageLUT[] = {0.9, 0.95, 1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35};
static const float nicdPercentLUT[] = {0, 10, 20, 30, 40, 50, 60, 70, 85, 100};

// LeadAc: 1.75V (empty) to 2.27V (full)
// Charakterystyka: stała napięcie, słaby wskaźnik SOC; gęstość elektrolitu jest bardziej wiarygodna
static const float leadAcVoltageLUT[] = {1.75, 1.8, 1.85, 1.9, 1.95, 2.0, 2.05, 2.1, 2.2, 2.27};
static const float leadAcPercentLUT[] = {0, 10, 20, 30, 40, 50, 60, 70, 90, 100};

// Alkaline (AA/AAA): 0.8V (empty) to 1.65V (full) przy niskim obciążeniu (<100µA)
// Charakterystyka: plateau ~1.2-1.3V, szybki spadek poniżej 1.0V; napięcie silnie zależy od obciążenia
static const float alkalineVoltageLUT[] = {0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.65};
static const float alkalinePercentLUT[] = {0, 5, 10, 15, 25, 40, 60, 75, 85, 92, 96, 100};

// ZincCarbon (cynkowo-węglowe): 0.8V (empty) to 1.65V (full)
// Charakterystyka: podobna do alkaline, ale ogólnie słabsza wydajność i bardziej stromy koniec
static const float zincCarbonVoltageLUT[] = {0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.65};
static const float zincCarbonPercentLUT[] = {0, 5, 10, 15, 25, 40, 60, 75, 85, 92, 96, 100};

BatteryPercentage::BatteryPercentage(float vRef, float adcMax, float kDivider, BatteryType type, int numBatteries)
    : vRef(vRef), adcMax(adcMax), kDivider(kDivider), type(type), numBatteries(numBatteries) {
    setLUTs();
}

void BatteryPercentage::setLUTs() {
    switch (type) {
        case BatteryType::LiPo:
            voltageLUT = lipoVoltageLUT;
            percentLUT = lipoPercentLUT;
            lutSize = sizeof(lipoVoltageLUT) / sizeof(lipoVoltageLUT[0]);
            break;
        case BatteryType::LiIon:
            voltageLUT = liionVoltageLUT;
            percentLUT = liionPercentLUT;
            lutSize = sizeof(liionVoltageLUT) / sizeof(liionVoltageLUT[0]);
            break;
        case BatteryType::NiMH:
            voltageLUT = nimhVoltageLUT;
            percentLUT = nimhPercentLUT;
            lutSize = sizeof(nimhVoltageLUT) / sizeof(nimhVoltageLUT[0]);
            break;
        case BatteryType::NiCd:
            voltageLUT = nicdVoltageLUT;
            percentLUT = nicdPercentLUT;
            lutSize = sizeof(nicdVoltageLUT) / sizeof(nicdVoltageLUT[0]);
            break;
        case BatteryType::LeadAc:
            voltageLUT = leadAcVoltageLUT;
            percentLUT = leadAcPercentLUT;
            lutSize = sizeof(leadAcVoltageLUT) / sizeof(leadAcVoltageLUT[0]);
            break;
        case BatteryType::Alkaline:
            voltageLUT = alkalineVoltageLUT;
            percentLUT = alkalinePercentLUT;
            lutSize = sizeof(alkalineVoltageLUT) / sizeof(alkalineVoltageLUT[0]);
            break;
        case BatteryType::ZincCarbon:
            voltageLUT = zincCarbonVoltageLUT;
            percentLUT = zincCarbonPercentLUT;
            lutSize = sizeof(zincCarbonVoltageLUT) / sizeof(zincCarbonVoltageLUT[0]);
            break;
        default:
            voltageLUT = lipoVoltageLUT;
            percentLUT = lipoPercentLUT;
            lutSize = sizeof(lipoVoltageLUT) / sizeof(lipoVoltageLUT[0]);
            break;
    }
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
