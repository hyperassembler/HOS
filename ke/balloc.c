#include <kern/balloc.h>
#include <math.h>
#include <kern/clib.h>
#include <kern/bitmap.h>

// for each size
// we have - bitmap representing each frame_size

int32 balloc_init(struct balloc_desc* desc, void* start, usize length, usize frame_size)
{
    // calculate size required
    // calculate the # of levels
    usize aligned_len = ALIGN(usize, length, frame_size);
    usize total_frames = aligned_len / frame_size;
    usize levels = (usize)ceil(log2(total_frames)) + 1; // include the 0th level


}

void* balloc(struct balloc_desc* desc, usize num_frames)
{

}

void bfree(struct balloc_desc* desc, uintptr ptr)
{

}