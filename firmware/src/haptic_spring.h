#pragma once
#include <stdint.h>
// Spring-to-minimum haptic (HAPTIC_SPRING_TO_MIN, local test build 1.56+): while
// the user holds the fader it is pulled toward its bottom end, and the pull is
// capped at a duty that grows with the distance from the bottom - gentle near 0,
// up to the strength limit at the top - so it feels like an elastic band.
// Released, it glides back to the bottom. Host tests: test/test_spring.

#define SPRING_MARGIN (15)      // above the fader's own breakaway, so it always gets back to 0
#define SPRING_FLOOR_MIN (95)   // floor if the breakaway is unknown/implausible

// Drive cap for a fader at user position `pos` (0 = bottom, 255 = top).
// `breakaway`: this fader's measured breakaway duty going down; `limit`: the cap
// at the top (the haptic strength). No division: the app has almost no flash left.
static inline int16_t spring_cap(uint8_t pos, int16_t breakaway, int16_t limit) {
  int16_t floor_ = breakaway + SPRING_MARGIN;
  if (floor_ < SPRING_FLOOR_MIN) floor_ = SPRING_FLOOR_MIN;
  if (limit <= floor_) return floor_;
  return floor_ + (int16_t)(((uint16_t)(limit - floor_) * (uint16_t)(pos + 1)) >> 8);
}
