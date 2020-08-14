#include <stdio.h>
#include <iostream>
#include <sgx_tseal.h>

#include <chrono>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

int main(int argc, char const *argv[]) {
  if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
    std::cout << "Fail to initialize enclave." << std::endl;
    return 1;
  }

  std::cout << "Initiating.... " << std::endl;
  sgx_status_t status = ecall_init(global_eid);
  std::cout << "Initiated.... " << std::endl;

  int accessMbStart = std::atoi(argv[1]);
  int accessMbEnd = std::atoi(argv[2]);
  int accessMbStep = std::atoi(argv[3]);


  // benchmark for E Calls
  for (int mb = accessMbStart; mb < accessMbEnd; mb += accessMbStep) {
    //std::cout << "Accessing " << mb << "MB" << std::endl;
    long totalTime = 0;
    for (int it = 0; it < 5; it++) {
      unsigned long failures = 0;
      std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
      status = ecall_access(global_eid, &failures, &mb);

      if (status != SGX_SUCCESS) {
        std::cout << "ecall failed" << std::endl;
      }
      std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();

      if (failures != 0) {
        std::cout << "Failed " << failures << std::endl;
      }

      long milis = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
      totalTime += milis;
    }
    std::cout << mb << "," << (totalTime / 5)<< std::endl;
//    std::cout << "Mean time for  " << mb << "MB : " << (totalTime / 5) << "ms. Time per MB : " << (totalTime / 5 / mb)
//              << std::endl;
  }
  return 0;
}
