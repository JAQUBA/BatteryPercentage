# BatteryPercentage - Profil napięciowych LUT

## Wstęp

Biblioteka BatteryPercentage szacuje stan naładowania (State of Charge, SOC) baterii na podstawie zmierzonego napięcia pojedynczego ogniwa. Wszystkie profile zostały opracowane dla temperatury **25°C** i **napięcia spoczynkowego** (bez obciążenia) lub przy bardzo lekkim obciążeniu.

**Ważne ograniczenia:**
- Napięcie jest słabym wskaźnikiem SOC, szczególnie dla baterii NiMH, NiCd i LeadAc
- Szacowanie nie zastępuje coulomb countingu ani dokładnych pomiarów laboratoryjnych
- Dokładność zależy od:
  - Historii ładowania/rozładowania ogniwa
  - Rezystancji wewnętrznej (aging)
  - Temperatury otoczenia (profil na 25°C)
  - Profilu obciążenia (brak obciążenia vs. duże prądy)
  - Producenta i typu ogniwa

## Profile baterii (per single cell/monomer)

### LiPo (Lithium Polymer)
- **Zakres**: 2.5V (pusty) → 4.2V (pełny)
- **Charakterystyka**: Szybki spadek napięcia przy końcu ładowania, długie plateau, kolaps poniżej 3V
- **Warunki**: 25°C, napięcie spoczynkowe, bez obciążenia
- **Zastosowanie**: Drony, elektronika konsumencka
- **LUT**: 13 punktów, zagęszczone w obszarach krytycznych (3.8-4.2V i <3.0V)

```
Napięcie (V): 2.5, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 3.9, 4.0, 4.1, 4.15, 4.2
SOC (%):      0,   5,   10,  15,  25,  40,  60,  75,  85,  92,  96,  99,   100
```

### LiIon (Lithium-Ion)
- **Zakres**: 2.5V (pusty) → 4.2V (pełny)
- **Charakterystyka**: Bardzo podobna do LiPo, lekko bardziej płaska w plateau
- **Warunki**: 25°C, napięcie spoczynkowe
- **Zastosowanie**: Baterie, elektronika przemysłowa
- **LUT**: 13 punktów

```
Napięcie (V): 2.5, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 3.9, 4.0, 4.1, 4.15, 4.2
SOC (%):      0,   5,   10,  15,  25,  40,  60,  75,  85,  92,  96,  99,   100
```

### NiMH (Nickel-Metal Hydride)
- **Zakres**: 0.9V (pusty) → 1.45V (pełny)
- **Charakterystyka**: BARDZO płaska krzywa, niemal liniowe spadanie napięcia
- **Warunki**: 25°C, napięcie spoczynkowe
- **Zastosowanie**: Baterie AA/AAA (starego typu), duże pojemności
- **Ograniczenie**: **Napięcie jest słabym wskaźnikiem SOC dla NiMH** - preferuj coulomb counting
- **LUT**: 12 punktów, równomiernie rozłożone

```
Napięcie (V): 0.9,  0.95, 1.0,  1.05, 1.1,  1.15, 1.2,  1.25, 1.3,  1.35, 1.4,  1.45
SOC (%):      0,    10,   20,   30,   40,   50,   60,   70,   80,   90,   95,   100
```

### NiCd (Nickel-Cadmium)
- **Zakres**: 0.9V (pusty) → 1.35V (pełny)
- **Charakterystyka**: Płaska krzywa z nagłym spadkiem na końcu
- **Warunki**: 25°C, napięcie spoczynkowe
- **Zastosowanie**: Urządzenia przemysłowe, narzędzia (wycofane z powodu toksyczności)
- **Ograniczenie**: **Napięcie nie jest wiarygodnym wskaźnikiem SOC dla NiCd**
- **LUT**: 10 punktów

```
Napięcie (V): 0.9,  0.95, 1.0,  1.05, 1.1,  1.15, 1.2,  1.25, 1.3,  1.35
SOC (%):      0,    10,   20,   30,   40,   50,   60,   70,   85,   100
```

### LeadAc (Lead-Acid / Sealed Lead Acid)
- **Zakres**: 1.75V (pusty) → 2.27V (pełny)
- **Charakterystyka**: Stosunkowo stałe napięcie, słaby wskaźnik SOC
- **Warunki**: 25°C, napięcie spoczynkowe
- **Zastosowanie**: Baterie awaryjne, zasilacze UPS, pojazdy
- **Ograniczenie**: **Napięcie jest SŁABYM wskaźnikiem SOC** - gęstość elektrolitu jest bardziej wiarygodna
- **Rekomendacja**: Dla dokładnego pomiaru SOC użyj pomiaru gęstości elektrolitu (gravity), nie napięcia
- **LUT**: 10 punktów

```
Napięcie (V): 1.75, 1.8,  1.85, 1.9,  1.95, 2.0,  2.05, 2.1,  2.2,  2.27
SOC (%):      0,    10,   20,   30,   40,   50,   60,   70,   90,   100
```

### Alkaline (AA/AAA)
- **Zakres**: 0.8V (pusty) → 1.65V (pełny)
- **Charakterystyka**: Plateau ~1.2-1.3V, szybki spadek poniżej 1.0V
- **Warunki**: 25°C, napięcie spoczynkowe, NISKIE OBCIĄŻENIE (<100µA)
- **Zastosowanie**: Elektronika konsumencka, czujniki, urządzenia bezprzewodowe
- **Ograniczenie**: **Napięcie SILNIE zależy od obciążenia** - przy dużych prądach (>100mA) profil jest całkowicie różny. Ten profil dotyczy urządzeń o małym, sporadycznym poborze prądu
- **Rekomendacja dla dużych obciążeń**: Opracuj osobny profil lub użyj coulomb countingu
- **LUT**: 12 punktów, zagęszczone w obszarach krytycznych

```
Napięcie (V): 0.8,  0.85, 0.9,  0.95, 1.0,  1.1,  1.2,  1.3,  1.4,  1.5,  1.6,  1.65
SOC (%):      0,    5,    10,   15,   25,   40,   60,   75,   85,   92,   96,   100
```

### ZincCarbon (Cynkowo-węglowe)
- **Zakres**: 0.8V (pusty) → 1.65V (pełny)
- **Charakterystyka**: Bardzo podobna do alkaline, ale słabsza wydajność i bardziej stromy koniec
- **Warunki**: 25°C, napięcie spoczynkowe, NISKIE OBCIĄŻENIE
- **Zastosowanie**: Elektronika konsumencka (starego typu)
- **Ograniczenie**: **Podobne do alkaline** - profil silnie zależy od obciążenia i historii
- **LUT**: 12 punktów

```
Napięcie (V): 0.8,  0.85, 0.9,  0.95, 1.0,  1.1,  1.2,  1.3,  1.4,  1.5,  1.6,  1.65
SOC (%):      0,    5,    10,   15,   25,   40,   60,   75,   85,   92,   96,   100
```

## Użycie API

```cpp
#include "BatteryPercentage.h"

// Dla dwóch ogniw alkalicznych przy 3.3V VREF i kalibracji na 840
BatteryPercentage battery(3.3, 840, 1.0, BatteryType::Alkaline, 2);

// Odczyt napięcia ze zmierzonej wartości ADC
uint16_t adcValue = analogRead(A0);
float percent = battery.getPercentage(adcValue);

// Wynik będzie w zakresie 0-100% reprezentującym szacunkowy SOC pakietu
```

## Rekomendacje dla poprawy dokładności

1. **Kalibracja ADC**: Wylicz współczynnik `adcMax` na podstawie rzeczywistych pomiarów multimetrem
2. **Filtracja**: Uśrednij kilka odczytów ADC, aby zmniejszyć szumy
3. **Temperatura**: Jeśli temperatura znacznie odbiega od 25°C, dokładność spada drastycznie
4. **Historia ogniw**: Stare ogniwa mają zmodyfikowane charakterystyki
5. **Coulomb Counting**: Dla krytycznych aplikacji użyj coulomb countingu zamiast napięciowego SOC
6. **Producent**: Różni producenci mają różne charakterystyki - idealne byłoby dostosowanie profilu do konkretnego producenta

## Referencje

- Wikipedia: Lithium-ion battery, NiMH battery, Alkaline battery, Lead-acid battery
- Energizer & Duracell Technical Manuals
- PowerStream.com AA discharge tests
- IEEE 485-2020 (Lead-acid standard)
- SAFT Industrial batteries documentation
