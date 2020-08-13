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

// OCall implementations
void ocall_data_out(const char *str) {

}

int main(int argc, char const *argv[]) {
  std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();;
  if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
    std::cout << "Fail to initialize enclave." << std::endl;
    return 1;
  }
  auto end_time = std::chrono::steady_clock::now();
  long time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
  std::cout << time << ",";
  return 0;
}
