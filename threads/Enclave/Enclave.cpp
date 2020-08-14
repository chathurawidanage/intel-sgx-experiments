#include "Enclave_t.h"

int fib(int n) {
  if (n <= 1)
    return n;
  return fib(n - 1) + fib(n - 2);
}

void ecall_compute_fib(const int *term) {
  fib(*term);
}