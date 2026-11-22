#pragma once
#include <stdint.h>

// Self-calibration stages and the endpoint sweep's sequencing. Pure, so it is
// unit tested on the host (firmware/test/test_selfcal_sweep).

// Self-calibration runs the endpoint sweep first, then (full calibration only)
// characterises the motor (see the MOTORCAL_* constants in motor_cal.cpp).
// Stages 0-2 are the endpoint sweep; their numbering is unchanged.
enum SelfCalStage : uint8_t {
  SELFCAL_ENDPOINT_LOW = 0,   // drive to the low end, record it
  SELFCAL_ENDPOINT_HIGH,      // drive to the high end, record it
  SELFCAL_ENDPOINT_APPLY,     // validate the span, adopt it in RAM
  SELFCAL_MOTOR_PARK,         // drive to the end this run starts from
  SELFCAL_MOTOR_SETTLE,       // coast to a genuine stop before measuring
  SELFCAL_MOTOR_RAMP,         // ramp to breakaway, then hold and capture the jump
  SELFCAL_MOTOR_DWELL_A,      // hold breakaway + A, measure cruise speed
  SELFCAL_MOTOR_DWELL_B,      // hold breakaway + B, measure cruise speed
  SELFCAL_MOTOR_NEXT,         // fold in the run, advance or finish
  SELFCAL_FINISH,             // derive gains, persist, return to the target
};

// An endpoint stage drives at full duty toward its end and ends once the
// carriage has stood still for ENDPOINT_STILL_MS (it is against the stop),
// or at ENDPOINT_TIMEOUT_MS whatever happens. Standing still is ignored for
// the first ENDPOINT_MIN_MS, while the carriage is still getting going.
#define ENDPOINT_TIMEOUT_MS (1500)
#define ENDPOINT_MIN_MS (150)
#define ENDPOINT_STILL_MS (60)

// Full calibration sweeps low then high: the motor characterisation that
// follows starts from the high end. The quick, endpoints-only calibration
// sweeps high then low (up, then down), and nothing follows it.
static inline SelfCalStage endpoint_first_stage(bool up_first) {
  return up_first ? SELFCAL_ENDPOINT_HIGH : SELFCAL_ENDPOINT_LOW;
}

static inline SelfCalStage endpoint_next_stage(SelfCalStage done, bool up_first) {
  if (done == SELFCAL_ENDPOINT_LOW) return up_first ? SELFCAL_ENDPOINT_APPLY : SELFCAL_ENDPOINT_HIGH;
  return up_first ? SELFCAL_ENDPOINT_LOW : SELFCAL_ENDPOINT_APPLY;
}

static inline bool endpoint_reached(uint16_t elapsed_ms, uint16_t still_ms) {
  if (elapsed_ms >= ENDPOINT_TIMEOUT_MS) return true;
  return elapsed_ms >= ENDPOINT_MIN_MS && still_ms >= ENDPOINT_STILL_MS;
}

// The quick calibration sweeps smoothly: a speed-limited closed-loop move aimed
// ENDPOINT_OVERSHOOT ADC counts past the end found last time, so it slows down
// approaching the stop and touches it gently instead of slamming into it at
// full duty. Then it stalls there and endpoint_reached() ends the stage.
#define ENDPOINT_OVERSHOOT (24)

static inline int16_t endpoint_smooth_target(SelfCalStage stage, int16_t calib_min, int16_t calib_max) {
  return (stage == SELFCAL_ENDPOINT_HIGH) ? calib_max + ENDPOINT_OVERSHOOT : calib_min - ENDPOINT_OVERSHOOT;
}
