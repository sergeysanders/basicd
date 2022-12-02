#include "stdlib.h"
uint8_t _pc_mem[64*1024];
void *pvPortMalloc(size_t size)
{
    static uint8_t *lastPtr = _pc_mem;
    uint8_t *ptr = lastPtr;
    lastPtr+=size;
    return ptr;
}
void pvPortFree(void *ptr)
{
    //free(ptr);
    return;
}

