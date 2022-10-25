#ifndef CLASSIFY_H_
#define CLASSIFY_H_


#include "edb_rai_model.h"

extern "C" {
    edb_rai_AICLASS edb_rai_classifier_prediction(float *input_vector); // not bad
}


int classificator_init();
edb_rai_AICLASS most_frequent_aic(edb_rai_AICLASS *arr, size_t size);
edb_rai_AICLASS classify(float *input_vector);


#endif // CLASSIFY_H_
