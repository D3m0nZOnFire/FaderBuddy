// Host-side tests for the self-calibration endpoint sweep: pio test -e native
#include <unity.h>
#include "selfcal_sweep.h"

void setUp() {}
void tearDown() {}

// Full calibration keeps its original order: low end, high end, then apply,
// because the motor characterisation after it starts from the high end.
void test_full_calibration_goes_low_then_high() {
  TEST_ASSERT_EQUAL(SELFCAL_ENDPOINT_LOW, endpoint_first_stage(false));
  TEST_ASSERT_EQUAL(SELFCAL_ENDPOINT_HIGH, endpoint_next_stage(SELFCAL_ENDPOINT_LOW, false));
  TEST_ASSERT_EQUAL(SELFCAL_ENDPOINT_APPLY, endpoint_next_stage(SELFCAL_ENDPOINT_HIGH, false));
}

// The quick (endpoints only) calibration goes up first, then down.
void test_quick_calibration_goes_high_then_low() {
  TEST_ASSERT_EQUAL(SELFCAL_ENDPOINT_HIGH, endpoint_first_stage(true));
  TEST_ASSERT_EQUAL(SELFCAL_ENDPOINT_LOW, endpoint_next_stage(SELFCAL_ENDPOINT_HIGH, true));
  TEST_ASSERT_EQUAL(SELFCAL_ENDPOINT_APPLY, endpoint_next_stage(SELFCAL_ENDPOINT_LOW, true));
}

void test_not_reached_while_still_moving() {
  TEST_ASSERT_FALSE(endpoint_reached(400, 0));
  TEST_ASSERT_FALSE(endpoint_reached(400, ENDPOINT_STILL_MS - 1));
}

void test_reached_once_still_long_enough() {
  TEST_ASSERT_TRUE(endpoint_reached(400, ENDPOINT_STILL_MS));
}

// Right after the motor starts it has not had time to move yet, so standing
// still then must not count as being at the end.
void test_not_reached_before_the_minimum_time() {
  TEST_ASSERT_FALSE(endpoint_reached(ENDPOINT_MIN_MS - 1, ENDPOINT_MIN_MS - 1));
  TEST_ASSERT_TRUE(endpoint_reached(ENDPOINT_MIN_MS, ENDPOINT_STILL_MS));
}

void test_timeout_always_ends_the_stage() {
  TEST_ASSERT_TRUE(endpoint_reached(ENDPOINT_TIMEOUT_MS, 0));
  TEST_ASSERT_FALSE(endpoint_reached(ENDPOINT_TIMEOUT_MS - 1, 0));
}

// The smooth sweep aims a little past the end found last time, so the
// carriage still reaches the real stop, slowing down as it gets there.
void test_smooth_target_is_just_past_the_old_end() {
  TEST_ASSERT_EQUAL_INT16(900 + ENDPOINT_OVERSHOOT, endpoint_smooth_target(SELFCAL_ENDPOINT_HIGH, 100, 900));
  TEST_ASSERT_EQUAL_INT16(100 - ENDPOINT_OVERSHOOT, endpoint_smooth_target(SELFCAL_ENDPOINT_LOW, 100, 900));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_full_calibration_goes_low_then_high);
  RUN_TEST(test_quick_calibration_goes_high_then_low);
  RUN_TEST(test_not_reached_while_still_moving);
  RUN_TEST(test_reached_once_still_long_enough);
  RUN_TEST(test_not_reached_before_the_minimum_time);
  RUN_TEST(test_timeout_always_ends_the_stage);
  RUN_TEST(test_smooth_target_is_just_past_the_old_end);
  return UNITY_END();
}
