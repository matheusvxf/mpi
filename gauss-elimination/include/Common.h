#pragma once

typedef unsigned char uint8_t;
#define DEBUG false

int ceil(int n, int d) {
  return n / d + (n % d == 0 ? 0 :1);
}
