#ifndef VECTOR_H
#define VECTOR_H

// Vector struct:
// size: size of vector
// data: pointer to data array
typedef struct
{

    int size;
    unsigned char *data;

} vector;

// Initialize vector
void vector_init(vector *v);

// Return size of vector
int vector_size(vector *v);

// Return the element at index idx
char vector_get(vector *v, int idx);

// Set the element at index idx to c
void vector_set(vector *v, unsigned char c, int idx);

// Set the size of the vector
void vector_set_size(vector *v, int size);

// Copy data from array to vector
void vector_cpy(vector *v, int size, unsigned char *arr);

// Push element to position idx, shifting all elements after it to the right
void vector_push(vector *v, unsigned char c, int idx);

// Stuff data
void vector_stuff(vector *v);

// Remove element at index idx, shifting all elements after it to the left
void vector_remove(vector *v, int idx);

// Free memory
void vector_delete(vector *v);

#endif // VECTOR_H