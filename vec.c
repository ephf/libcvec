#include "vec.h"

static void* malloc_with_size_hint(size_t s) {
    size_t* d = malloc(sizeof(size_t) + s);
    *d = s;
    return d + 1;
}

static void* realloc_with_size_hint(void* p, size_t s) {
    size_t* d = realloc((size_t*) p - 1, sizeof(size_t) + s);
    *d = s;
    return d + 1;
}

static size_t size_with_hint(void* p) {
    return ((size_t*) p)[-1];
}

static void free_with_size_hint(void* p) {
    free((size_t*) p - 1);
}

// issue with `malloc_usable_size`
// #if defined(__linux__)
// #include <malloc.h>
// v_allocator_t _V_DEFAULT_ALLOCR = {
//     &malloc,
//     &realloc,
//     &malloc_usable_size,
//     &free
// };
#if defined(__APPLE__)
#include <malloc/malloc.h>
v_allocator_t _V_DEFAULT_ALLOCR = {
    &malloc,
    &realloc,
    (void*) &malloc_size,
    &free
};
#elif defined(_WIN32)
#include <malloc.h>
v_allocator_t _V_DEFAULT_ALLOCR = {
    &malloc,
    &realloc,
    &_msize,
    &free
};
#else
v_allocator_t _V_DEFAULT_ALLOCR = {
    &malloc_with_size_hint,
    &realloc_with_size_hint,
    &size_with_hint,
    &free_with_size_hint
};
#endif

void* bvecw(size_t capacity, v_allocator_t* allocator) {
    vec_header_t* h = allocator->malloc(sizeof(vec_header_t) + capacity);
    *h = (vec_header_t) { allocator, 0 };
    return h + 1;
}

int bvresv(void** vec, size_t size) {
    if(!*vec) *vec = bvecw(size, V_DEFAULT_ALLOCR);
    vec_header_t* h = (vec_header_t*)(*vec) - 1;
    size_t c = h->allocator->size(h);
    if(size > SIZE_T_MAX - h->size) return 2;
    if((size += h->size) <= c) return 0;
    if(c <= 0) c = size;
    else do {
        if(c > SIZE_T_MAX / 2) c = SIZE_T_MAX;
        else c *= 2;
    } while(c < size);
    h = h->allocator->realloc(h, sizeof(vec_header_t) + c);
    if(!h) return 1;
    *vec = h + 1;
    return 0;
}

int bvcat(void** vec, void* data, size_t data_size) {
    int e = bvresv(vec, data_size);
    if(e) return e;
    vec_header_t* h = (vec_header_t*)(*vec) - 1;
    memcpy(*vec + h->size, data, data_size);
    h->size += data_size;
    return 0;
}

int bvcut(void** vec, size_t cut_size) {
    if(!*vec) return 1;
    vec_header_t* h = (vec_header_t*)(*vec) - 1;
    if(cut_size > h->size) {
        h->size = 0;
        return 1;
    }
    h->size -= cut_size;
    return 0;
}

int bvins(void** vec, size_t i, void* data, size_t data_size) {
    int e = bvresv(vec, data_size);
    if(e) return e;
    vec_header_t* h = (vec_header_t*)(*vec) - 1;
    e = (i > h->size) * 3;
    if(i >= h->size) {
        memcpy(*vec + h->size, data, data_size);
        h->size += data_size;
        return e;
    }
    memmove(*vec + i + data_size, *vec + i, h->size - i);
    memcpy(*vec + i, data, data_size);
    h->size += data_size;
    return e;
}

int bvrem(void** vec, size_t i, size_t rem_size) {
    if(!*vec) return 1;
    vec_header_t* h = (vec_header_t*)(*vec) - 1;
    if(rem_size > h->size) {
        h->size = 0;
        return 1;
    }
    int e = (i > h->size) * 3;
    if(i >= h->size) {
        h->size -= rem_size;
        return e;
    }
    memmove(*vec + i, *vec + i + rem_size, h->size - i - rem_size);
    h->size -= rem_size;
    return e;
}

void bvfree(void* vec) {
    if(!vec) return;
    free((vec_header_t*)(vec) - 1);
}