// Claude dynamic array, lets see
#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Define a generic dynamic array structure */
typedef struct {
    void *data;           /* Pointer to the array data */
    size_t element_size;  /* Size of each element in bytes */
    size_t size;          /* Number of elements currently in the array */
    size_t capacity;      /* Total capacity of the array */
    float growth_factor;  /* Factor by which to grow the array when needed */
    float threshold;      /* Threshold percentage to trigger reallocation */
} Array;

bool array_init(Array *arr, size_t element_size, size_t initial_capacity, float growth_factor, float threshold);
void array_free(Array *arr);
void *array_get(Array *arr, size_t index);
bool array_set(Array *arr, size_t index, const void *element);
bool array_check_realloc(Array *arr);
bool array_push(Array *arr, const void *element);
bool array_pop(Array *arr, void *element);
bool array_insert(Array *arr, size_t index, const void *element);
bool array_remove(Array *arr, size_t index, void *element);
size_t array_size(const Array *arr);
size_t array_capacity(const Array *arr);
void array_clear(Array *arr);
bool array_shrink_to_fit(Array *arr, float buffer);

#endif
