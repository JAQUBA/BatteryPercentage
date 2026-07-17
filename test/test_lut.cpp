#include <unity.h>
#include "../src/BatteryPercentage.h"
#include <cmath>

// Tolerancja dla porównań zmiennoprzecinkowych
const float TOLERANCE = 0.5f;  // ±0.5%

// ============================================================================
// Testy ogólne: Inicjalizacja i API
// ============================================================================

void test_constructor_alkaline() {
    BatteryPercentage bat(3.3, 840, 1.0, BatteryType::Alkaline, 2);
    // Konstruktor powinien się nie rzucić
    TEST_PASS();
}

void test_constructor_single_cell() {
    BatteryPercentage bat(3.3, 840, 1.0, BatteryType::LiPo, 1);
    TEST_PASS();
}

// ============================================================================
// Testy interpolacji: Węzły i wartości pośrednie
// ============================================================================

void test_lipo_at_min_voltage() {
    BatteryPercentage bat(3.3, 1023, 1.0, BatteryType::LiPo, 1);
    // Napięcie 2.5V * 1023 / 3.3 ≈ 777
    uint16_t adc_for_2_5v = (uint16_t)(2.5 / 3.3 * 1023);
    float percent = bat.getPercentage(adc_for_2_5v);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, percent);
}

void test_lipo_at_max_voltage() {
    BatteryPercentage bat(3.3, 1023, 1.0, BatteryType::LiPo, 1);
    // Napięcie 4.2V * 1023 / 3.3 ≈ 1307 (ale max ADC to 1023, więc będzie nasycone)
    // Dla tego testu używamy mniejszego VREF lub dzielnika
    BatteryPercentage bat2(5.0, 1023, 1.0, BatteryType::LiPo, 1);
    uint16_t adc_for_4_2v = (uint16_t)(4.2 / 5.0 * 1023);
    float percent = bat2.getPercentage(adc_for_4_2v);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 100.0f, percent);
}

void test_alkaline_at_min() {
    BatteryPercentage bat(3.3, 840, 1.0, BatteryType::Alkaline, 2);
    // 2 ogniwa: 0.8V na jedno = 1.6V total
    // 1.6V / 3.3 * 840 ≈ 406 (dla pojedynczego ogniwa w funkcji dzielone przez 2)
    // getPercentage dzieli voltage przez numBatteries, czyli: 1.6 / 2 = 0.8V
    uint16_t adc = (uint16_t)(1.6 / 3.3 * 840);
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, percent);
}

void test_alkaline_at_max() {
    BatteryPercentage bat(3.3, 840, 1.0, BatteryType::Alkaline, 2);
    // 2 ogniwa: 1.65V na jedno = 3.3V total
    uint16_t adc = (uint16_t)(3.3 / 3.3 * 840);
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 100.0f, percent);
}

void test_nimh_voltage_middle() {
    BatteryPercentage bat(1.5, 1023, 1.0, BatteryType::NiMH, 1);
    // Test napięcia w środku: 1.2V
    uint16_t adc = (uint16_t)(1.2 / 1.5 * 1023);
    float percent = bat.getPercentage(adc);
    // 1.2V w profilu NiMH to 60%
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 60.0f, percent);
}

void test_liion_below_range() {
    BatteryPercentage bat(5.0, 1023, 1.0, BatteryType::LiIon, 1);
    // Napięcie poniżej 2.5V powinno dać 0%
    uint16_t adc = (uint16_t)(2.0 / 5.0 * 1023);  // 2.0V
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, percent);
}

void test_liion_above_range() {
    BatteryPercentage bat(5.0, 1023, 1.0, BatteryType::LiIon, 1);
    // Napięcie powyżej 4.2V powinno dać 100%
    uint16_t adc = (uint16_t)(4.5 / 5.0 * 1023);  // 4.5V
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, percent);
}

// ============================================================================
// Testy wieloogniwowe
// ============================================================================

void test_three_cells_series() {
    BatteryPercentage bat(5.0, 1023, 1.0, BatteryType::LiPo, 3);
    // Pakiet 3 x LiPo: 3 * 3.6V = 10.8V nominal
    // Przy max: 3 * 4.2V = 12.6V
    // Ale żaden problem, getPercentage dzieli przez numBatteries
    uint16_t adc = (uint16_t)(12.6 / 5.0 * 1023);
    float percent = bat.getPercentage(adc);
    // Powinno być ~100%
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 100.0f, percent);
}

// ============================================================================
// Testy wszystkich typów baterii
// ============================================================================

void test_leadac_full() {
    BatteryPercentage bat(3.0, 1023, 1.0, BatteryType::LeadAc, 1);
    uint16_t adc = (uint16_t)(2.27 / 3.0 * 1023);
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 100.0f, percent);
}

void test_nicd_full() {
    BatteryPercentage bat(1.5, 1023, 1.0, BatteryType::NiCd, 1);
    uint16_t adc = (uint16_t)(1.35 / 1.5 * 1023);
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 100.0f, percent);
}

void test_zinc_carbon_full() {
    BatteryPercentage bat(3.3, 840, 1.0, BatteryType::ZincCarbon, 2);
    uint16_t adc = (uint16_t)(3.3 / 3.3 * 840);
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 100.0f, percent);
}

// ============================================================================
// Testy edge cases
// ============================================================================

void test_interpolation_between_points() {
    BatteryPercentage bat(5.0, 1023, 1.0, BatteryType::Alkaline, 1);
    // Alkaline: między 0.85V (5%) a 0.9V (10%)
    // Interpolacja dla 0.875V powinno być ~7.5%
    uint16_t adc = (uint16_t)(0.875 / 5.0 * 1023);
    float percent = bat.getPercentage(adc);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE + 2.0f, 7.5f, percent);  // Większa tolerancja dla interpolacji
}

// ============================================================================
// Uruchomienie testów
// ============================================================================

void setup() {
    UNITY_BEGIN();

    // Testy inicjalizacji
    RUN_TEST(test_constructor_alkaline);
    RUN_TEST(test_constructor_single_cell);

    // Testy węzłów
    RUN_TEST(test_lipo_at_min_voltage);
    RUN_TEST(test_lipo_at_max_voltage);
    RUN_TEST(test_alkaline_at_min);
    RUN_TEST(test_alkaline_at_max);
    RUN_TEST(test_nimh_voltage_middle);

    // Testy granic
    RUN_TEST(test_liion_below_range);
    RUN_TEST(test_liion_above_range);

    // Testy wieloogniwowe
    RUN_TEST(test_three_cells_series);

    // Testy wszystkich typów
    RUN_TEST(test_leadac_full);
    RUN_TEST(test_nicd_full);
    RUN_TEST(test_zinc_carbon_full);

    // Testy edge cases
    RUN_TEST(test_interpolation_between_points);

    UNITY_END();
}

void loop() {
    // Nie używane w testach
}
