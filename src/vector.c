#include "header.h"
#include "../include/vector.h"

void vector_init(vector *v) {

    v->data = NULL;
    v->size = 0;

}


int vector_size(vector *v) {

    return v->size;

}

char vector_get(vector *v, int idx) {

    if (idx >= v->size || idx < 0) {
        printf("Index %d out of bounds for vector of size %d\n", idx, v->size);
        return -1;
    }

    return v->data[idx];

}

void vector_set(vector *v,  char c,int idx) {

    if (idx >= v->size || idx < 0) {
        printf("Index %d out of bounds for vector of size %d\n", idx, v->size);
        return;
    }

    v->data[idx] = c; 

}

void vector_set_size(vector *v, int size) {

    v->size = size;

}

void vector_cpy(vector *v, int size, char* data) {

    vector_init(v);
    vector_set_size(v, size);
    memcpy(v->data, data, size);

}

void vector_push(vector *v, char c, int idx) {

    vector_set_size(v, v->size + 1);

    if (idx > v->size || idx < 0) {
        printf("Index %d out of bounds for vector of size %d\n", idx, v->size);
        return;
    }

    for(int i = v-> size-1; i > idx; i--) {
        v->data[i] = v->data[i-1];
    }
    v->data[idx] = c;

}

void vector_remove(vector *v, int idx) {

    if (idx >= v->size || idx < 0) {
        printf("Index %d out of bounds for vector of size %d\n", idx, v->size);
        return -1;
    }

    for(int i = idx; i < v->size - 1; i++) {
        v->data[i] = v->data[i+1];
    }

    vector_set_size(v, v->size - 1);

}

void vector_delete(vector *v) {

    free(v->data);

}