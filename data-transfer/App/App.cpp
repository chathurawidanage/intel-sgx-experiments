#include <stdio.h>
#include <iostream>
#include <sgx_tseal.h>

#include <chrono>
#include <algorithm>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

std::chrono::steady_clock::time_point begin_time;

int64_t total_time = 0;

// OCall implementations
void ocall_data_out(const char *str) {
  auto end_time = std::chrono::steady_clock::now();
  total_time += std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count();
}

void local_ecall_mimic(const char *str) {
  ocall_data_out(str);
}

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

int main(int argc, char const *argv[]) {
  if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
    std::cout << "Fail to initialize enclave." << std::endl;
    return 1;
  }

  int32_t iterations = std::atoi(argv[1]);
  int32_t string_size = std::atoi(argv[2]);

  //std::cout << "Iterations " << iterations << "," << "Str size " << string_size << std::endl;

  std::string test = random_string(string_size * 1000);

  // benchmark for E Calls
  for (int32_t i = 0; i < iterations; i++) {
    begin_time = std::chrono::steady_clock::now();
    sgx_status_t status = ecall_data_in(global_eid, test.c_str());
    if (status != SGX_SUCCESS) {
      std::cout << "ecall failed" << std::endl;
    }
  }

  double sgx_time = (double) total_time / iterations;

  //std::cout << "SGX E Call Mean time " << sgx_time << "ms" << std::endl;

  total_time = 0;
  // benchmark for local calls
  for (int32_t i = 0; i < iterations; i++) {
    begin_time = std::chrono::steady_clock::now();
    local_ecall_mimic(test.c_str());
  }

  double local_time = (double) total_time / iterations;

  //std::cout << "Local Call Mean time " << local_time << "ms" << std::endl;

  std::cout << string_size << "," << sgx_time << "," << local_time << std::endl;

  return 0;
}
