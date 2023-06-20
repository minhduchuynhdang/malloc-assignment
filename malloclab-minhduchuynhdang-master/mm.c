/*
 * mm.c
 *
 * Name: [Duc Huynh]
 *
 * defined struct for header containing size and prev/next free block to implement explicit list
 * helper function find_mem() to find suitable free space for requested size
 * 
 * initialize heap with only 1 header that points towards itself and will act as the head of the free list
 * malloc will extend the heap if no space available or allocate space if there exists suitable block
 * free will mark the block as free and manipulate it to points to first free block in the list and set it as the first free block
 * realloc implemented using malloc() and free()
 *
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include "mm.h"
#include "memlib.h"

/*
 * If you want to enable your debugging output and heap checker code,
 * uncomment the following line. Be sure not to have debugging enabled
 * in your final submission.
 */
//#define DEBUG

#ifdef DEBUG
// When debugging is enabled, the underlying functions get called
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_assert(...) assert(__VA_ARGS__)
#else
// When debugging is disabled, no code gets generated
#define dbg_printf(...)
#define dbg_assert(...)
#endif // DEBUG

// do not change the following!
#ifdef DRIVER
// create aliases for driver tests
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define memset mm_memset
#define memcpy mm_memcpy
#endif // DRIVER

#define ALIGNMENT 16
#define hdr_size align(sizeof(hdr))



// rounds up to the nearest multiple of ALIGNMENT
static size_t align(size_t x)
{
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

typedef struct header hdr;

/* header struct */
struct header {
    size_t size;
    struct header *next;
    struct header *prev;
};



/*
 * mm_init: returns false on error, true on success.
 */
bool mm_init(void)
{
    // IMPLEMENT THIS
    hdr *header = mm_sbrk(hdr_size);
    header->size = hdr_size;
    header->next = header;
    header->prev = header;

    return true;
}

/*Check if a block is free
static bool isfree(void *mem_ptr){
    mem_header_t *header = (mem_header_t *)mem_ptr;
    if((header->size & 0x000000000000000F) == 0x0){
        return true;
    }
    return false;
}*/


// Look for suitable memory block, return NULL if not found
static void *find_free_mem(size_t req_size) {
    hdr *ptr;

    //Check for suitable free space (free and bigger than req_size)
    for(ptr = ((hdr *)mm_heap_lo())->next; ptr != mm_heap_lo() && ptr->size < req_size; ptr = ptr->next);
    if(ptr != mm_heap_lo()){
        return ptr;
    }
    else{
        return NULL;
    }
}

/*
 * malloc
 */
void* malloc(size_t size)
{
    // IMPLEMENT THIS
    if (size == 0) {
        return NULL;
    }
    //get size neede to allocate
    size_t req_size = align(size + hdr_size);
    //find ptr to suitable free header
    hdr *ptr = find_free_mem(req_size);

    //If free space is available
    if(ptr != NULL){
        ptr->size |= 1; //mark as allocated
        ptr->prev->next = ptr->next; //prev free space points to next free space
        ptr->next->prev = ptr->prev; //next free space points to prev free space
    }
    else{
        //Extend heap for more space
        ptr = mm_sbrk(req_size);
        if(ptr == (void *)-1){
            return NULL;
        }
        else{
            ptr->size = req_size | 1; //mark as allocated

        }
    }

    //return ptr to alloced space
    return (char *)ptr+hdr_size;
}

/*
 * free
 */
void free(void* ptr)
{
    // IMPLEMENT THIS
    //set to the current header
    hdr *new_ptr = ptr - hdr_size;
    hdr *head = mm_heap_lo();

    //set current header to free
    new_ptr->size &= ~1;

    //current header next points to the first free block
    new_ptr->next = head->next;
    //current header prev points to head
    new_ptr->prev = head;
    //head points to current header
    head->next = new_ptr;
    //first free block prev points to current header
    new_ptr->next->prev = new_ptr;

}

/*
 * realloc
 */
void* realloc(void* oldptr, size_t size)
{
    // IMPLEMENT THIS
    if(oldptr == NULL){
        return malloc(size);
    }
    if(size == 0){
        free(oldptr);
        return NULL;
    }

    //free oldptr
    free(oldptr);
    hdr *old_header = oldptr - hdr_size;
    //get ptr to new memory block
    void *ptr = malloc(size);
    if(ptr == NULL){
        return NULL;
    }

    //Check number of bytes to copy
    size_t copy_size = old_header->size - hdr_size;
    if(copy_size > size){
        copy_size = size;
    }
    mm_memcpy(ptr, oldptr, copy_size);

    //return new allocated memory
    return ptr;
}

/*
 * calloc
 * This function is not tested by mdriver, and has been implemented for you.
 */
void* calloc(size_t nmemb, size_t size)
{
    void* ptr;
    size *= nmemb;
    ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

/*
 * Returns whether the pointer is in the heap.
 * May be useful for debugging.
 */
static bool in_heap(const void* p)
{
    return p <= mm_heap_hi() && p >= mm_heap_lo();
}

/*
 * Returns whether the pointer is aligned.
 * May be useful for debugging.
 */
static bool aligned(const void* p)
{
    size_t ip = (size_t) p;
    return align(ip) == ip;
}

/*
 * mm_checkheap
 * You call the function via mm_checkheap(__LINE__)
 * The line number can be used to print the line number of the calling
 * function where there was an invalid heap.
 */
bool mm_checkheap(int line_number)
{
#ifdef DEBUG
    // Write code to check heap invariants here
    // IMPLEMENT THIS
#endif // DEBUG
    return true;
}
