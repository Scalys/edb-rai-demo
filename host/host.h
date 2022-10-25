// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#ifndef __HOST_H_
#define __HOST_H_


#define ENCLAVE_MESSAGE_SIZE 512

extern int g_enclave_debug;
extern int g_enclave_simulate;
extern int g_logo;

int create_enclave(const char *ta_name);
int terminate_enclave();
int call_enclave();


#endif /* __HOST_H_ */
