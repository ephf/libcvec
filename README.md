# libcvec

implementation of a growable array or vector in c

```c
#include "vec.h"
#include <assert.h>

int main() {
    vec_t(int) numbers = vecof(1, 2, 3);

    vpush(&numbers, 4);

    assert(numbers[0] == 1);
    assert(numbers[3] == 4);
}
```

The `vec_t(t)` macro is just syntactic sugar for `t*`, both work the same and therefore you can use array indexing as you would any other array.

The total memory for `n` bytes of data in a vector is `sizeof(void*) + sizeof(size_t) + n` bytes. When a vector is initialized using the `vec()` macro, it is initialized as `NULL` and does not carry anymore data unless it is modified using a function that *increases its size*, in which case it will be allocated to the heap where `n` is the size of the item being added to the vector. It may be important to note, if you use the `vecw()` macro to initialize a vector, it will be allocated to the heap even if the capacity is set to `0`.

## Macros / Functions

```c
vec_t(t) vecw(<t>, size_t capacity, v_allocator_t* allocator);
```

Returns a vector based on the type `t` and capacity `capacity`, and using the allocator `allocator`.

```c
vec_t(?) vec();
```

Returns `NULL`, an un-initialized vector which will use the `V_DEFAULT_ALLOCATOR` when used in a function that increases the vectors size.

```c
vec_t(t) vecof(<t>... items);
```

Returns a vector based on the type of the supplied items `items` and a capacity equal to the size of all of the items combined, and using the `V_DEFAULT_ALLOCATOR`.

```c
int vresv(vec_t(<t>)* vec, size_t size);
```

Reserves `size` bytes of `vec` and resizes `vec` if it needs more space. Returns `1` if its allocator's `realloc` fails and `0` if not.

```c
int vpush(vec_t(<t>)* vec, t item);
```

Pushes an item `item` to the end of `vec` and resizes `vec` if it needs more space. Returns `1` if its allocator's `realloc` fails and `0` if not.

```c
int vpop(vec_t(<t>)* vec);
```

Removes the last value from `vec`. If the size of the removed item is greater than the size of the vector `vec`, the size of `vec` will be set to `0`. Returns `1` if the removed item's size is greater than `vec`'s size and `0` if not.

```c
int vins(vec_t(<t>)* vec, size_t i, t item);
```

Inserts an item `item` at index `i` into `vec` and resizes `vec` if it needs more space. If the index `i` is out of bounds, it will be set as the vector's length and the value will be pushed on to the vector `vec`, *inserting a value at `3` on a vector with a length of `1` will just push the value on to the vector*. Returns `1` if its allocator's `realloc` fails, `2` if the index `i` is out of bounds, and `0` if there is no error.

```c
int vrem(vec_t(<t>)* vec, size_t i);
```

Removes the item at index `i` from `vec`. If the index `i` is out of bounds, it will be set as the vector's length and the value will be popped from the vector `vec`, *removing a value at `3` on a vector with a length of `1` will just pop the last value from the vector*. Returns `1` if the removed item's size is greater than `vec`'s size, `2` if the index `i` is out of bounds, and `0` if there is no error.

```c
size_t vsize(vec_t(<t>) vec);
```

Returns the size of vector `vec`.

```c
v_allocator_t vallocr(vec_t(?) vec);
```

Returns the allocator of vector `vec`.

```c
size_t vcap(vec_t(<t>) vec);
```

Returns the capacity of vector `vec`.

```c
void vfree(vec_t(?) vec);
```

Frees vector `vec` if it is not `NULL`.

## Types / Variables

```c
vec_t(t)
```

Syntactic sugar for `t*`.

```c
typedef struct {
    void* (*malloc)(size_t);
    void* (*realloc)(void*, size_t);
    size_t (*size)(void*);
    void (*free)(void*);
} v_allocator_t;
```

Allocator struct passed into the `vecw()` macro when you need to specify certain allocation functions.

```c
v_allocator_t _V_DEFAULT_ALLOCR;
```

The default allocation functions which use the `stdlib.h`'s `malloc`, `realloc`, and `free`. The os specific `size_t (*size)(void*)` function is defined as `malloc_usable_size` for linux, `malloc_size` for macos, and `_msize` for windows. Other operating systems have completely new set of functions which provide a `size_t` hint at the beginning of each allocation.

```c
typedef struct {
    v_allocator_t* allocator;
    size_t size;
} vec_header_t;
```

The metadata associated with each vector follows this structure. This is where the `sizeof(void*) + sizeof(size_t)` bytes of padding come from.

```c
#define V_DEFAULT_ALLOCR &_V_DEFAULT_ALLOCR
```

This is the default used by `vec()` and `vecof()`