#pragma once
#include <cstdint>

struct Constraint {
  uint32_t idx1;
  uint32_t idx2;
  float length;

  Constraint(uint32_t idx1_, uint32_t idx2_, float length_)
      : idx1(idx1_), idx2(idx2_), length(length_) {}
};
