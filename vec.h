#ifndef __VEC_H
#define __VEC_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    void* (*malloc)(size_t);
    void* (*realloc)(void*, size_t);
    size_t (*size)(void*);
    void (*free)(void*);
} v_allocator_t;

v_allocator_t _V_DEFAULT_ALLOCR;

typedef struct {
    v_allocator_t* allocator;
    size_t size;
} vec_header_t;

// vec_t<t>
#define vec_t(t) t*

#define V_DEFAULT_ALLOCR &_V_DEFAULT_ALLOCR

void* bvecw(size_t capacity, v_allocator_t* allocator);
// void* bvec();
#define bvec() NULL
int bvresv(void** vec, size_t size);
int bvcat(void** vec, void* data, size_t data_size);
int bvcut(void** vec, size_t cut_size);
int bvins(void** vec, size_t i, void* data, size_t data_size);
int bvrem(void** vec, size_t i, size_t rem_size);
// size_t bvsize(void* vec);
#define bvsize(vec) (((vec_header_t*)(void*)(vec))[-1].size)
// v_allocator_t* bvallocr(void* vec);
#define bvallocr(vec) (((vec_header_t*)(void*)(vec))[-1].allocator)
// size_t bvcap(void* vec);
#define bvcap(vec) (bvallocr(vec)->size((vec_header_t*)(void*)(vec) - 1) - sizeof(vec_header_t))
void bvfree(void* vec);

// vec_t(t) vecw(<t>, size_t capacity, v_allocator_t* allocator);
#define vecw(t, capacity, allocator) ((vec_t(t)) bvecw(sizeof(t) * capacity, allocator))
// vec_t(?) vec();
#define vec() bvec()
// vec_t(t) vecof(<t>... items)
#define vecof(item1, items...) ({\
    typeof(item1) _data[] = { item1, items };\
    typeof(*_data)* _vec = vecw(typeof(*_data), sizeof(_data) / sizeof(*_data), V_DEFAULT_ALLOCR);\
    for(int _i = 0; _i < sizeof(_data) / sizeof(*_data); _i++)\
        vpush(&_vec, _data[_i]);\
    _vec;\
})
// int vresv(vec_t(<t>)* vec, size_t size);
#define vresv(vec, size) bvresv((void**)(void*)(vec), size * sizeof(*(vec)))
// int vpush(vec_t(<t>)* vec, t item);
#define vpush(vec, item) bvcat((void**)(void*)(vec), &(typeof(**(vec))) { item }, sizeof(**(vec)))
// int vpop(vec_t(<t>)* vec);
#define vpop(vec) bvcut((void**)(void*)(vec), sizeof(**(vec)))
// int vins(vec_t(<t>)* vec, size_t i, t item);
#define vins(vec, i, item) bvins((void**)(void*)(vec), i * sizeof(**(vec)), &(typeof(**(vec))) { item }, sizeof(**(vec)))
// int vrem(vec_t(<t>)* vec, size_t i);
#define vrem(vec, i) bvrem((void**)(void*)(vec), i * sizeof(**(vec)), sizeof(**(vec)))
// size_t vsize(vec_t(<t>) vec);
#define vsize(vec) (bvsize(vec) / sizeof(*(vec)))
// v_allocator_t* vallocr(vec_t(?) vec);
#define vallocr(vec) bvallocr(vec)
// size_t vcap(vec_t(<t>) vec);
#define vcap(vec) (bvcap(vec) / sizeof(*(vec)))
// void vfree(vec_t(?) vec);
#define vfree(vec) bvfree(vec)

#endif