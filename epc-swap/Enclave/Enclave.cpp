#include "Enclave_t.h"

// create an array of 160MB


unsigned long *arr;
unsigned long length = 512 * 1024 * 1024 / sizeof(unsigned long);

void ecall_init() {
  arr = static_cast<unsigned long *>(malloc(sizeof(unsigned long) * length));
  for (unsigned long i = 0; i < length; i++) {
    arr[i] = i;
  }
}

unsigned long int ecall_access(int *sizeMb) {
  unsigned long accessLength = (*sizeMb) * 1024 * 1024 / sizeof(unsigned long);
  unsigned long int failures = 0;
  for (unsigned long i = 0; i < accessLength; i++) {
    if (arr[i] != i) {
      failures++;
    }
  }
  return failures;
}