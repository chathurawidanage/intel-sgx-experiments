#include "Enclave_t.h"

void ecall_data_in(const char *str) {
  ocall_data_out(str);
}