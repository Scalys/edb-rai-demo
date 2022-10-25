// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

//#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <openenclave/enclave.h>

#include "edb_rai_demo_t.h"

#include "classify.h"
#include "misc.h"
#include "edb_rai_demo_model_data.h"
#include "common.h"

/* Re-implement missing symbols in accordance with OP-TEE */
void *__stack_chk_guard = (void*)0x0000aff;
extern "C" {
    void *__memcpy_chk(void *dest, const void *src, size_t len, size_t dstlen)
    {
        return memcpy(dest, src, len);
    }
}

int ecall_init()
{
    int retval;

    ocall_log(&retval, "EDB ML init");

    classificator_init();

    return OE_OK;
}

int ecall_rai(float *sample, size_t sample_size)
{
    int retval = 0;
    edb_rai_AICLASS aic;
    const size_t g_vector_size = 1024;
    memset(sample, 0, sample_size * sizeof(sample[0]));

    aic = classify(sample);
    ocall_class_result(&retval, (int)aic);

    return OE_OK;
}


#define TA_UUID                                            \
    { /* c7198089-6313-4ba6-8e3c-47846520df18 */           \
        0xc7198089, 0x6313, 0x4ba6,                        \
        {                                                  \
		    0x8e, 0x3c, 0x47, 0x84, 0x65, 0x20, 0xdf, 0x18 \
        }                                                  \
    }

OE_SET_ENCLAVE_OPTEE(
    TA_UUID,                  // UUID
    2 * 1024 * 1024,          // HEAP_SIZE
    16 * 1024,                // STACK_SIZE
    0,                        // FLAGS
    "1.0.0",                  // VERSION
    "edb-rai-demo")           // DESCRIPTION
