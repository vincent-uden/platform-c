#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Example usage:
   struct Lifetime lt = createLt();
   int *testInt = malloc(sizeof(int));
   if (testInt == NULL) {
     unwindLt(&lt);
     exit(1);
   }
   pushLt(&lt, testInt, &free);
   popLt();
   printLt(lt);
   pushLt(&lt, testInt, &free);
   popPtrLt(&lt, testInt);
*/

/* Lifetime is a system to aid in tracking all memory allocations 
 * as to guarantee memory safety
 */
typedef struct {
    void **ptrs;
    void (**deAllocators) (void *ptr);
    int stackBound;
    int stackSize;
} Lifetime ;

Lifetime createLt();
void rescaleLt(Lifetime *lt, int newStackBound);
void pushLt(Lifetime* lt, void* ptr, void (*deAllocatorc) (void* p));
void rescaleLt(Lifetime* lt, int newStackBound);
void popLt(Lifetime *lt);
void popPtrLt(Lifetime* lt, void* ptr);
void printLt(Lifetime lt);
void unwindLt(Lifetime* lt);

/* These macros should be used to interact with the Lifetime struct,
 * not the functions. They include some nice casting which reduces
 * boilerplate code.
 */
#define PUSH_LT(lt, ptr, deAllocator) \
    pushLt(&lt, ptr, (void (*) (void*)) deAllocator)

#define POP_LT(lt) \
    popLt(&lt)

#define POP_LT_PTR(lt, ptr) \
    popPtrLt(&lt, ptr)

#define UNWIND_LT(lt) \
    unwindLt(&lt)
