#include "array.h"

bool array_init(Array *arr, size_t element_size, size_t initial_capacity, float growth_factor, float threshold) {
    arr->element_size = element_size;
    arr->size = 0;
    arr->capacity = initial_capacity;
    arr->growth_factor = growth_factor;
    arr->threshold = threshold;
    
    arr->data = malloc(element_size * initial_capacity);
    return arr->data != NULL;
}

void array_free(Array *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

void* array_get(Array *arr, size_t index) {
    if (index >= arr->size) return NULL;
    return (char*)arr->data + (index * arr->element_size);
}

bool array_set(Array *arr, size_t index, const void *element) {
    if (index >= arr->size) return false;
    
    memcpy((char*)arr->data + (index * arr->element_size), element, arr->element_size);
    return true;
}

bool array_check_realloc(Array *arr) {
    if (arr->size >= arr->capacity * arr->threshold) {
        size_t new_capacity = (size_t)(arr->capacity * arr->growth_factor);
        if (new_capacity <= arr->capacity) new_capacity = arr->capacity * 2; /* Ensure growth */
        
        void *new_data = realloc(arr->data, arr->element_size * new_capacity);
        if (new_data == NULL) return false;
        
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    return true;
}

bool array_push(Array *arr, const void *element) {
    if (!array_check_realloc(arr)) return false;
    
    memcpy((char*)arr->data + (arr->size * arr->element_size), element, arr->element_size);
    arr->size++;
    return true;
}

bool array_pop(Array *arr, void *element) {
    if (arr->size == 0) return false;
    
    arr->size--;
    if (element) {
        memcpy(element, (char*)arr->data + (arr->size * arr->element_size), arr->element_size);
    }
    return true;
}

bool array_insert(Array *arr, size_t index, const void *element) {
    if (index > arr->size) return false;
    if (!array_check_realloc(arr)) return false;
    
    if (index < arr->size) {
        memmove(
            (char*)arr->data + ((index + 1) * arr->element_size),
            (char*)arr->data + (index * arr->element_size),
            (arr->size - index) * arr->element_size
        );
    }
    
    memcpy((char*)arr->data + (index * arr->element_size), element, arr->element_size);
    arr->size++;
    return true;
}

bool array_remove(Array *arr, size_t index, void *element) {
    if (index >= arr->size) return false;
    
    /* Copy the element data if requested */
    if (element) {
        memcpy(element, (char*)arr->data + (index * arr->element_size), arr->element_size);
    }
    
    /* Move elements after index one position left */
    if (index < arr->size - 1) {
        memmove(
            (char*)arr->data + (index * arr->element_size),
            (char*)arr->data + ((index + 1) * arr->element_size),
            (arr->size - index - 1) * arr->element_size
        );
    }
    
    arr->size--;
    return true;
}

size_t array_size(const Array *arr) {
    return arr->size;
}

size_t array_capacity(const Array *arr) {
    return arr->capacity;
}

void array_clear(Array *arr) {
    arr->size = 0;
}

bool array_shrink_to_fit(Array *arr, float buffer) {
    size_t new_capacity = (size_t)(arr->size * (1.0 + buffer));
    if (new_capacity < 4) new_capacity = 4; /* Minimum capacity */
    
    if (new_capacity >= arr->capacity) return true; /* No need to shrink */
    
    void *new_data = realloc(arr->data, arr->element_size * new_capacity);
    if (new_data == NULL) return false;
    
    arr->data = new_data;
    arr->capacity = new_capacity;
    return true;
}
