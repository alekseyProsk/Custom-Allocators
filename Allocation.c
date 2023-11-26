#include "Allocation.h"
#include<stdio.h>
#include<stdint.h>

static binder ListFree[228]; //для памяти, которая освобождена my_free

static uint16_t inUse = 0;

void* my_malloc(size_t size)
{	
	if (size >= HEAP_SIZE)
	{
		return NULL; // Если пытаемся выделить места больше, чем у нас в куче
	}

	size += HEADER;

	binder* bind = newBinder(size);

	uint8_t* begin = bind->ptr;
	uint8_t* userPtr = begin + HEADER;

	*begin = size; // доступ в первому байту данных, куда ссылается bind

	bind->ptr += size; //данные + хедер
	bind->size -= size; // вычитаем  размер данных из кучи

	printLog();

	return userPtr;
}

void printLog() // для отладки
{
	for (unsigned i = 0; i < inUse; i++)
	{
		printf("DATA + header [%p]. Free memory on heap: [%d]\n", (uint8_t*) ListFree[i].ptr, (int) ListFree[i].size);
	}
}

binder* newBinder(size_t size)
{
	if (ListFree[0].ptr == NULL && ListFree[0].size == 0)
	{
		
		ListFree[0].ptr = heap;
		ListFree[0].size = HEAP_SIZE;
		inUse++;
		printLog();
	}

	binder* fragment = ListFree;

	for (unsigned i = 0; i < inUse; i++)
	{
		if (ListFree[i].size >= size && ListFree[i].size < fragment->size) //подбираем наиболее подходящий по размеру фрагмент
		{
			fragment = &ListFree[i];
		}
	}

	return fragment;
}

void my_free(void* ptr) 
{
	uint8_t* begin = (uint8_t*)ptr - HEADER;

	binder* prev = 0, *next = 0;
	for (size_t i = 0; i < inUse; i++)
	{
		if (ListFree[i].ptr + ListFree[i].size == begin)
			prev = &ListFree[i];
		if (begin + *begin == ListFree[i].ptr)
			next = &ListFree[i];
	}
	if (next != NULL)
	{
		next->ptr = begin;
		next->size += *begin;
		*begin = next->size;
		inUse--;
	}
	if (prev != NULL)
	{
		prev->size += *begin;
		if (next)
		{
			next->ptr = ListFree[inUse].ptr;
			next->size = ListFree[inUse].size;
		}
		inUse--;
	}
	if (!(prev || next))
	{
		ListFree[inUse].ptr = begin;
		ListFree[inUse].size = *begin;
	}
	inUse++;
	printLog();
}

void* my_realloc(void* ptr, size_t size)
{
	void* newptr;

	size_t memsize = sizeof(ptr);

	newptr = my_malloc(size);

	newptr = my_memcpy(newptr, ptr, memsize);

	my_free(ptr);

	printLog();

	return newptr;

}

void* my_memcpy(void* dst, const void* src, size_t cnt) // 
{
	uint8_t* dest = (uint8_t*)dst;
	const uint8_t* sourc = (const uint8_t*)src;

	while (cnt--)
		*dest++ = *sourc++;

	return dst;
}