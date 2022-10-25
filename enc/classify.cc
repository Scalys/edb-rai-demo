#include <stdio.h>
#include <stdint.h>
#include <cstdlib>

#include "misc.h"
#include "classify.h"

#define INFERENCE_FILTER_SIZE 5
edb_rai_AICLASS *g_results;


edb_rai_AICLASS most_frequent_aic(edb_rai_AICLASS *arr, size_t size)
{
    edb_rai_AICLASS aic;
    int maxcount = 0;

    for (int i = 0; i < INFERENCE_FILTER_SIZE; i++) {
        int count = 0;
        for (int j = 0; j < INFERENCE_FILTER_SIZE; j++) {
            edb_rai_AICLASS a = (arr[i] == edb_rai_0) ? edb_rai_Silence : arr[i];
            edb_rai_AICLASS b = (arr[j] == edb_rai_0) ? edb_rai_Silence : arr[j];

            if (a == b)
                count++;

            if (count > maxcount) {
                aic = a;
                maxcount = count;
            }
        }
    }

    return aic;
}

int classificator_init()
{
    g_results = (edb_rai_AICLASS *)malloc(INFERENCE_FILTER_SIZE * sizeof(g_results[0]));
    for (int i = 0; i < INFERENCE_FILTER_SIZE; i++)
        g_results[i] = edb_rai_Silence;

    return 0;
}

void classificator_deinit()
{
    free(g_results);
    g_results = 0;
}

edb_rai_AICLASS classify(float *input_vector)
{
    static edb_rai_AICLASS aic;

    /* Shift in the latest inference result to calculate overall */
    for (int i = INFERENCE_FILTER_SIZE - 1; i > 0; i--) {
        g_results[i] = g_results[i-1];
    }
    g_results[0] = edb_rai_classifier_prediction(input_vector);

    aic = most_frequent_aic(g_results, INFERENCE_FILTER_SIZE);

    return aic;
}
