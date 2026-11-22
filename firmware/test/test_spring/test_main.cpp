// Host-side tests for the spring-to-minimum haptic: pio test -e native
#include <unity.h>
#include "haptic_spring.h"

void setUp() {}
void tearDown() {}

// The pull at the bottom is the fader's own breakaway plus a margin, so even a
// stiff fader comes all the way back to 0 when let go.
void test_bottom_is_breakaway_plus_margin() {
  TEST_ASSERT_EQUAL(86 + SPRING_MARGIN, spring_cap(0, 86, 254));
  TEST_ASSERT_EQUAL(100 + SPRING_MARGIN, spring_cap(0, 100, 254));
}

// At the top (255) it is the full strength limit.
void test_strongest_at_the_top() {
  TEST_ASSERT_EQUAL(254, spring_cap(255, 86, 254));
}

// In between it grows with the distance from the bottom, about linearly.
void test_grows_with_distance() {
  int16_t lo = 86 + SPRING_MARGIN;
  int16_t a = spring_cap(64, 86, 254), b = spring_cap(128, 86, 254), c = spring_cap(192, 86, 254);
  TEST_ASSERT_TRUE(a > lo);
  TEST_ASSERT_TRUE(b > a);
  TEST_ASSERT_TRUE(c > b);
  TEST_ASSERT_INT_WITHIN(2, lo + (254 - lo) / 2, b);
}

// A strength limit below the floor never makes it weaker than the floor.
void test_limit_below_floor() {
  TEST_ASSERT_EQUAL(86 + SPRING_MARGIN, spring_cap(200, 86, 90));
}

// A nonsense breakaway (unit never characterised: 0) still gives a usable floor.
void test_missing_breakaway() {
  TEST_ASSERT_EQUAL(SPRING_FLOOR_MIN, spring_cap(0, 0, 254));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_bottom_is_breakaway_plus_margin);
  RUN_TEST(test_strongest_at_the_top);
  RUN_TEST(test_grows_with_distance);
  RUN_TEST(test_limit_below_floor);
  RUN_TEST(test_missing_breakaway);
  return UNITY_END();
}
