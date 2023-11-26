
#ifndef _ALLOCATION_H
#define _ALLOCATION_H

#include<stdio.h>
#include<stdint.h>


#define HEAP_SIZE 128//128 байт паямяти - фиксированный размер кучи
#define HEADER 4

uint8_t static heap[HEAP_SIZE];

typedef	struct binder // связующая штука чтобы пользоваться памятью
{
	uint8_t* ptr;
	size_t size;

}binder;

void* my_malloc(size_t size);

void my_free(void* ptr);

void* my_realloc(void* ptr, size_t size);

binder* newBinder(size_t size);

void* my_memcpy(void* dst, const void* src, size_t cnt);

void printLog();

#endif // !

