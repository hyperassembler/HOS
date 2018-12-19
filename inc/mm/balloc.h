#include <kern/cdef.h>


struct balloc_desc
{
    usize frame_size;
    void* start;
    usize length;
};

int32 balloc_init(struct balloc_desc* desc, void* start, usize length, usize frame_size);

void* balloc(struct balloc_desc* desc, usize num_frames);

void bfree(struct balloc_desc* desc, uintptr ptr);