
#ifndef VECTOR
#define VECTOR

#include"exception.h"

#define CHECK_VECTOR(vector)                     \
    if(!vector){                                 \
        EXCEPT(MEMORY, "Bad vector allocation"); \
        exit(1);                                 \
    }

typedef unsigned bool;

typedef struct{
    unsigned size;
    char* string;
}Vector;

unsigned push_Vector(const char* str, Vector* vector, unsigned size);
Vector* new_Vector(unsigned sz, Vector* vector);
Vector* resize_Vector(unsigned sz, Vector* vector);
void copy_Vector(const Vector* const src_vector, Vector* dest_vector, unsigned size);
bool compare_Vector(const Vector* const vector1, const Vector* const vector);
void print_Vector(const Vector* const vector);
void delete_Vector(Vector* vector);

#endif
