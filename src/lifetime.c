#include "../headers/lifetime.h"

/* Returns a usable Lifetime */
Lifetime createLt() {
    Lifetime output;
    output.stackBound = 10;
    output.stackSize = 0;

    output.ptrs = malloc(sizeof(void *) * output.stackBound);
    output.deAllocators = malloc(sizeof(void *) * output.stackBound);

    return output;
}

/* Rescales the pointer and function arrays of the lifetime
 * according to a new stack size
 */
void rescaleLt(Lifetime* lt, int newStackBound) {
    void** newPtrs                     = malloc( sizeof(void*) * newStackBound );
    void (**newDeAllocators) (void* p) = malloc( sizeof(void*) * newStackBound );

    // Unwind stack
    if (newPtrs == NULL || newDeAllocators == NULL)
        exit(1);

    int oldBound = (*lt).stackBound;

    memcpy(newPtrs, (*lt).ptrs, fmin(newStackBound, oldBound) * sizeof(void *));
    memcpy(newDeAllocators, (*lt).deAllocators, fmin(newStackBound, oldBound) * sizeof(void*));

    free((*lt).ptrs);
    free((*lt).deAllocators);

    (*lt).ptrs = newPtrs;
    (*lt).deAllocators = newDeAllocators;
    (*lt).stackBound = newStackBound;
}

/* Pushes a new address to the top of the stack */
void pushLt(Lifetime* lt, void* ptr, void (*deAllocator) (void* p)) {
    if ((*lt).stackSize == (*lt).stackBound) {
        rescaleLt(lt, (*lt).stackBound + 10);
    }
    (*lt).ptrs[(*lt).stackSize] = ptr;
    (*lt).deAllocators[(*lt).stackSize] = deAllocator;
    (*lt).stackSize++;
}

/* Pops an address from the top of the stack */
void popLt(Lifetime *lt) {
    int index = (*lt).stackSize - 1;
    void* ptr = (*lt).ptrs[index];
    (*lt).deAllocators[index] (ptr);
    (*lt).ptrs[index] = NULL;
    (*lt).deAllocators[index] = NULL;

    /* Check if the lifetime should be rescaled */
    if (index <= (*lt).stackBound - 10)
        rescaleLt(lt, (*lt).stackBound - 10);

    (*lt).stackSize--;
}

/* Pop a specific pointer */
void popPtrLt(Lifetime* lt, void* ptr) {
    /* Find index of ptr */
    int index = -1;
    for (int i = 0; i < (*lt).stackSize; i++) {
        if ((*lt).ptrs[i] == ptr) {
            index = i;
            break;
        }
    }

    /* Unwind stack */
    if (index == -1)
        exit(1);

    /* Deallocate and remove dangling pointers */
    (*lt).deAllocators[index] (ptr);
    (*lt).ptrs[index] = NULL;
    (*lt).deAllocators[index] = NULL;

    /* Move everything after deleted item back one step */
    memcpy((*lt).ptrs + index, (*lt).ptrs + index + 1, sizeof(void *) * ((*lt).stackSize - index - 1));
    memcpy((*lt).deAllocators + index, (*lt).deAllocators + index + 1, sizeof(void *) * ((*lt).stackSize - index - 1));

    if ((*lt).stackSize - 1 <= (*lt).stackBound - 10)
        rescaleLt(lt, (*lt).stackBound - 10);

    (*lt).stackSize--;
}

/* Pretty print Lifetime */
void printLt(Lifetime lt) {
    printf("Lifetime pointer stack:\n");
    for (int i = lt.stackSize - 1; i >= 0; i--) {
        printf("%i:\t0x%x\n", i, lt.ptrs[i]);
    }
}

/* Traverses the stack, freeing all pointers then the 
 * stack itself
 */
void unwindLt(Lifetime* lt) {
    for (int i = (*lt).stackSize - 1; i >= 0; i--) {
        (*lt).deAllocators[i] ((*lt).ptrs[i]);
    }

    free((*lt).deAllocators);
    free((*lt).ptrs);
    (*lt).stackSize = 0;
    (*lt).stackBound = 0;
}
