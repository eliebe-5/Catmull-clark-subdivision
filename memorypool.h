#pragma once
#include <assert.h>
#include <vector>
#include <stdexcept>

#include <stdio.h>

#define BITSHIFT 14
#define BLOCKSIZE (1 << BITSHIFT)

#define uint unsigned int
#define ushort unsigned short

//sizeof(T) MUST BE a power of 2

template <class T>
class Memorypool
{
private:
	std::vector<T*> blocks;
	uint nr_of_blocks;
	uint next_element;

	void allocate_new_block();

public:
	Memorypool();
	~Memorypool();

	T* new_element();
	T* get_element_ref(uint);
	void pre_allocate_elements(uint);

	T* operator[](uint);
};

template<class T>
inline void Memorypool<T>::allocate_new_block()
{
	blocks.push_back(new T[BLOCKSIZE/sizeof(T)]);
	nr_of_blocks++;
}

template<class T>
inline Memorypool<T>::Memorypool()
{
	static_assert(BLOCKSIZE % sizeof(T) == 0);
	nr_of_blocks = 0;
	next_element = 0;
}

template<class T>
inline Memorypool<T>::~Memorypool()
{
	for (uint i = 0; i < nr_of_blocks; i++)
		delete[] blocks[i];
}

template<class T>
inline T* Memorypool<T>::new_element()
{
	if (next_element*sizeof(T) >> BITSHIFT == nr_of_blocks)
		allocate_new_block();

	uint element = next_element;

	next_element++;
    
	return get_element_ref(element);
}

template<class T>
inline T * Memorypool<T>::get_element_ref(uint index)
{
    if (index >= next_element)
		throw std::out_of_range("Out of range\n");
    
	uint block_index = index*sizeof(T) >> BITSHIFT;
	uint element_index = (index & (BLOCKSIZE/sizeof(T) - 1));
	return &(blocks[block_index][element_index]);
}

template<class T>
inline void Memorypool<T>::pre_allocate_elements(uint elements)
{
	uint e_amount = elements + next_element - 1;
	while (e_amount*sizeof(T) >> BITSHIFT >= nr_of_blocks)
		allocate_new_block();

}

template<class T>
inline T * Memorypool<T>::operator[](uint index)
{
	return get_element_ref(index);
}
