#include"vector.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

unsigned push_Vector(const char* str, Vector* vector, unsigned size){
    unsigned sz;
    if(!size) sz=strlen(str);
    else sz=size;
    if(!vector){
        if(!(vector=new_Vector(size, vector))){
            EXCEPT(MEMORY, "Failed to push vector!");
            return 0;
        }
    }
    if(vector->size<sz) vector=resize_Vector(sz-vector->size, vector);
    for(unsigned i=0;i<sz;++i){
        vector->string[vector->size++]=str[i];
    }
    return sz;
}

Vector* new_Vector(unsigned sz, Vector* vector){
    vector=(Vector*)malloc(sz*sizeof(Vector));
    CHECK_VECTOR(vector)
    return vector;
}

Vector* resize_Vector(unsigned sz, Vector* vector){
    Vector* new_vector=new_Vector(sz+vector->size, new_vector);
    copy_Vector(vector, new_vector, vector->size);
    delete_Vector(vector);
    return new_vector;
}

void copy_Vector(const Vector* const src_vector, Vector* dest_vector, unsigned size){
    unsigned sz;
    if(size==0) sz=src_vector->size>=dest_vector->size? dest_vector->size : src_vector->size;
    else sz=size;

    if(sz>dest_vector->size) dest_vector=resize_Vector(sz-dest_vector->size, dest_vector);
    for(unsigned i=0;i<sz;++i){
        dest_vector->string[i]=src_vector->string[i];
    }
}

bool compare_Vector(const Vector* const vector1, const Vector* const vector2){
    if(vector1->size!=vector2->size) return 1;
    for(unsigned i=0;i<vector1->size;++i){
        if(vector1->string[i]!=vector2->string[i]) return 1;
    }
    return 0;
}

void print_Vector(const Vector* const vector){
    CHECK_REACH(vector)
    printf("%s\n", vector->string);
}

void delete_Vector(Vector* vector){
    free(vector->string);
    free(vector);
    vector=NULL;
}