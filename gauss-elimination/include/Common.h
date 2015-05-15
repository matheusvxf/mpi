#pragma once

typedef unsigned char uint8_t;
#define DEBUG false
#define PRECISION 1e-6

inline int ceil(int n, int d) {
  return n / d + (n % d == 0 ? 0 :1);
}

