#include "Allocation.h"
#include<stdio.h>
#include<stdint.h>

static binder ListFree[228]; //äëÿ ïàìÿòè, êîòîðàÿ îñâîáîæäåíà my_free

static uint16_t inUse = 0;

void* my_malloc(size_t size)
{	
	if (size >= HEAP_SIZE)
	{
		return NULL; // Åñëè ïûòàåìñÿ âûäåëèòü ìåñòà áîëüøå, ÷åì ó íàñ â êó÷å
	}

	size += HEADER;

	binder* bind = newBinder(size);

	uint8_t* begin = bind->ptr;
	uint8_t* userPtr = begin + HEADER;

	*begin = size; // äîñòóï â ïåðâîìó áàéòó äàííûõ, êóäà ññûëàåòñÿ bind

	bind->ptr += size; //äàííûå + õåäåð
	bind->size -= size; // âû÷èòàåì  ðàçìåð äàííûõ èç êó÷è

	printLog();

	return userPtr;
}

void printLog() // äëÿ îòëàäêè
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
		if (ListFree[i].size >= size && ListFree[i].size < fragment->size) //ïîäáèðàåì íàèáîëåå ïîäõîäÿùèé ïî ðàçìåðó ôðàãìåíò
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
