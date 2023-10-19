#ifndef VECTOR_H
#define VECTOR_H

typedef struct {

    int size;
    unsigned char* data;

} vector;

void vector_init(vector* v);
int vector_size(vector* v);
char vector_get(vector* v, int idx);
void vector_set(vector* v, unsigned char c, int idx);

void vector_set_size(vector* v, int size);
void vector_cpy(vector* v, int size, unsigned char* arr);
void vector_push(vector* v, unsigned char c, int idx);
void vector_stuff(vector* v);
void vector_remove(vector* v, int idx);
void vector_delete(vector* v);


#endif // VECTOR_H