#include <stdio.h>
#include <iostream>
#include <sgx_tseal.h>

#include <chrono>
#include <thread>
#include <vector>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

int fib(int n) {
  if (n <= 1)
    return n;
  return fib(n - 1) + fib(n - 2);
}

int main(int argc, char const *argv[]) {
  if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
    std::cout << "Fail to initialize enclave." << std::endl;
    return 1;
  }

  int min_threads = std::atoi(argv[1]);
  int max_threads = std::atoi(argv[2]);
  int terms = std::atoi(argv[3]);

  // doing local test
  for (int t = min_threads; t < max_threads; t++) {
    // do local computation
    std::cout << "Starting Local test" << std::endl;
    std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<std::thread>> threads{};
    threads.reserve(t);
    for (int x = 0; x < t; x++) {
      auto trd = std::make_shared<std::thread>([terms]() {
        fib(terms);
      });
      threads.push_back(trd);
    }
    for (const auto &trd:threads) {
      trd->join();
    }
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    long milis = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
    std::cout << "Local Time for  " << t << " threads : " << (milis) << "ms" << std::endl;

    threads.clear();
    // onclave
    std::cout << "Starting Enclave test" << std::endl;
    begin_time = std::chrono::steady_clock::now();
    for (int x = 0; x < t; x++) {
      auto trd = std::make_shared<std::thread>([terms]() {
        ecall_compute_fib(global_eid, &terms);
      });
      threads.push_back(trd);
    }
    for (const auto &trd:threads) {
      trd->join();
    }
    end_time = std::chrono::steady_clock::now();
    milis = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
    std::cout << "Enclave Time for  " << t << " threads : " << (milis) << "ms" << std::endl;
  }
  return 0;
}
