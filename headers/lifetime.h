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

#define PUSH_LT(lt, ptr, deAllocator) \
    pushLt(&lt, ptr, (void (*) (void*)) deAllocator)

#define POP_LT(lt) \
    popLt(&lt)

#define POP_LT_PTR(lt, ptr) \
    popPtrLt(&lt, ptr)

#define UNWIND_LT(lt) \
    unwindLt(&lt)
