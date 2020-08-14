make clean
make SGX_MODE=HW
/home/chathura/Code/SGX/sgxsdk/bin/x64/sgx_sign sign -key Enclave/Enclave_private.pem  -enclave enclave.so -out enclave.signed.so -config Enclave/Enclave.config.xml
./app 8 9 50