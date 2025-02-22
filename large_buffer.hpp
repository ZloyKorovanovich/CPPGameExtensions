#ifndef _LARGE_BUFFER_INCLUDED
#define _LARGE_BUFFER_INCLUDED

#include <iostream>

namespace large_buffer {

	//Buffer for objects of different scale
	struct RawBuffer{
	/*
		o - object, m - memory
		|m|o|m|o|...|m|o|
	*/

		size_t m_iterator;
		size_t m_memory;
		void* m_objectBuffer;

		/// <summary>
		/// creates buffer of full memory size
		/// </summary>
		/// <param name="memory"></param>
		RawBuffer(size_t memory) {
			m_memory = memory;
			m_iterator = 0;
			m_objectBuffer = malloc(m_memory);
			memset(m_objectBuffer, 0, m_memory);
		}
		/// <summary>
		/// creates buffer of memory size and number of objects
		/// </summary>
		/// <param name="memory"></param>
		RawBuffer(size_t memory, unsigned int objectCount) {
			m_memory = memory + sizeof(unsigned short int) * objectCount;
			m_iterator = 0;
			m_objectBuffer = malloc(m_memory);
			memset(m_objectBuffer, 0, m_memory);
		}
		/// <summary>
		/// imports preallocated buffer
		/// </summary>
		/// <param name="memory"></param>
		/// <param name="buffer"></param>
		/// <param name="objectCount"></param>
		RawBuffer(size_t memory, void* buffer, unsigned int objectCount) {
			m_memory = memory;
			m_iterator = 0;
			m_objectBuffer = buffer;
		}

		/// <summary>
		/// disposes buffer
		/// </summary>
		~RawBuffer() {
			free(m_objectBuffer);
		}

		/// <summary>
		/// copies object from pointer to id
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		/// <param name="id"></param>
		void setObject(void* object, unsigned short int memory, size_t id) {
			unsigned char* ptr = (unsigned char*)m_objectBuffer + id;
			*((unsigned short int*)ptr) = memory;

			ptr = ptr + sizeof(unsigned short int);
			memcpy(ptr, object, memory);
		}
		/// <summary>
		/// copies object from pointer to iterator
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		void setObject_iterate(void* object, unsigned short int memory) {
			unsigned char* ptr = (unsigned char*)m_objectBuffer + m_iterator;
			*((unsigned int*)ptr) = memory;

			ptr = ptr + sizeof(unsigned short int);
			memcpy(ptr, object, memory);

			m_iterator += memory + sizeof(unsigned short int);
		}
		/// <summary>
		/// safely copies object from pointer to iterator
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		/// <returns></returns>
		bool setObject_iterate_safe(void* object, unsigned short int memory) {
			if (m_iterator + memory + sizeof(unsigned short int) >= m_memory) {
				return false;
			}

			unsigned char* ptr = (unsigned char*)m_objectBuffer + m_iterator;
			*((unsigned short int*)ptr) = memory;

			ptr = ptr + sizeof(unsigned short int);
			memcpy(ptr, object, memory);

			m_iterator += memory + sizeof(unsigned short int);
			return true;
		}

		/// <summary>
		/// copies object from id to pointer
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		/// <param name="id"></param>
		void getObject(void* object, unsigned short int* memory, size_t id) {
			unsigned char* ptr = (unsigned char*)m_objectBuffer + id;
			*memory = *(unsigned short int*)ptr;
			memcpy(object, ptr + sizeof(unsigned short int), *memory);
		}
		/// <summary>
		/// copies object from iterator to pointer and adjusts iterator
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		void getObject_iterate(void* object, unsigned short int* memory) {
			unsigned char* ptr = (unsigned char*)m_objectBuffer + m_iterator;
			*memory = *(unsigned short int*)ptr;
			memcpy(object, ptr + sizeof(unsigned short int), *memory);

			m_iterator += *memory + sizeof(unsigned short int);
		}
		/// <summary>
		/// copies object from iterator to pointer and adjusts iterator in cycle
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		/// <returns></returns>
		bool getObject_iterate_cycle(void* object, unsigned short int* memory) {
			unsigned char* ptr = (unsigned char*)m_objectBuffer + m_iterator;
			*memory = *(unsigned short int*)ptr;
			memcpy(object, ptr + sizeof(unsigned short int), *memory);

			m_iterator += *memory + sizeof(unsigned short int);
			if (m_iterator >= m_memory) {
				resetIterator();
				return false;
			}

			return true;
		}
		/// <summary>
		/// safely copies object from iterator to pointer and adjusts iterator
		/// </summary>
		/// <param name="object"></param>
		/// <param name="memory"></param>
		/// <returns></returns>
		bool getObject_iterate_safe(void* object, unsigned short int* memory) {
			if (m_iterator >= m_memory) {
				return false;
			}

			unsigned char* ptr = (unsigned char*)m_objectBuffer + m_iterator;

			unsigned short int mem = *(unsigned short int*)ptr;
			unsigned short int moveAmount = mem + sizeof(unsigned short int);
			if (m_iterator + moveAmount > m_memory) {
				return false;
			}

			*memory = mem;
			memcpy(object, ptr + sizeof(unsigned short int), mem);
			m_iterator += moveAmount;

			return true;
		}

		/// <summary>
		/// resets iterator
		/// </summary>
		void resetIterator() {
			m_iterator = 0;
		}
	};

	//Ordered buffer for uniform objects with activity state representation 
	template <typename T>
	struct StateBuffer_structured {
	/*
		o - object, b - activity section for 8 objects
		|b|o|o|o|o|o|o|o|o|...|b|o|o|
	*/

		unsigned int m_objectCount;
		void* m_objectBuffer;

		/// <summary>
		/// creates a buffer of default 128 length
		/// </summary>
		StateBuffer_structured() {
			m_objectCount = 128;
			size_t memory = m_objectCount * ((sizeof(T) << 3) + 1);
			memory = (memory + 7) >> 3;

			m_objectBuffer = malloc(memory);
			memset(m_objectBuffer, 0, memory);
		}
		/// <summary>
		/// creates a buffer for number of objects
		/// </summary>
		/// <param name="objectCount"></param>
		StateBuffer_structured(unsigned int objectCount) {
			m_objectCount = objectCount;
			size_t memory = m_objectCount * ((sizeof(T) << 3) + 1);
			memory = (memory + 7) >> 3;

			m_objectBuffer = malloc(memory);
			memset(m_objectBuffer, 0, memory);
		}
		/// <summary>
		/// imports preallocated buffer
		/// </summary>
		/// <param name="objectCount"></param>
		/// <param name="buffer"></param>
		/// <param name="memory"></param>
		StateBuffer_structured(unsigned int objectCount, void* buffer, size_t memory) {
			m_objectCount = objectCount;
			m_objectBuffer = buffer;
		}

		/// <summary>
		/// disposes buffer
		/// </summary>
		~StateBuffer_structured() {
			free(m_objectBuffer);
		}

		/// <summary>
		/// copies object from pointer to id and enables it
		/// </summary>
		/// <param name="object"></param>
		/// <param name="id"></param>
		void setObject(T* object, unsigned int id) {
			unsigned int bid = id >> 3;
			unsigned int bpid = bid * (1 + sizeof(T) * 8);
			unsigned int tid = id * sizeof(T) + bid + 1;

			T* place = (T*)((unsigned char*)m_objectBuffer + tid);
			unsigned char* bplace = (unsigned char*)m_objectBuffer + bpid;

			*place = *object;
			*bplace = *bplace | ((unsigned char)0b00000001 << (id - (id >> 3 << 3)));
		}
		/// <summary>
		/// safely copies object from pointer to id and enables it
		/// </summary>
		/// <param name="object"></param>
		/// <param name="id"></param>
		/// <returns></returns>
		bool placeObject_safe(T* object, unsigned int id) {
			if (id > m_objectCount) {
				return false;
			}

			unsigned int bid = id >> 3;
			unsigned int bpid = bid * (1 + sizeof(T) * 8);
			unsigned int tid = id * sizeof(T) + bid + 1;

			unsigned char* bplace = (unsigned char*)m_objectBuffer + bpid;
			unsigned char b = *bplace;
			int d = id - (id >> 3 << 3);

			if (b >> d & (unsigned char)0b00000001) {
				return false;
			}

			T* place = (T*)((unsigned char*)m_objectBuffer + tid);
			*place = *object;
			*bplace = b | (unsigned char)(0b00000001 << (int)d);

			return true;
		}

		/// <summary>
		/// disables object at id
		/// </summary>
		/// <param name="id"></param>
		void freeObject(unsigned int id) {
			unsigned int bid = id >> 3;
			unsigned int bpid = bid * (1 + sizeof(T));

			std::byte* bplace = (std::byte*)m_objectBuffer + bpid;
			*bplace = *bplace & ~((std::byte)0b00000001 << (id - bpid));
		}

		/// <summary>
		/// disables all objects
		/// </summary>
		void freeAll() {
			for (unsigned int i = 0; i < m_objectCount; i += sizeof(T)) {
				((std::byte*)m_objectBuffer)[i] = (std::byte)0b00000000;
			}
		}

		/// <summary>
		/// copies object from id to pointer
		/// </summary>
		/// <param name="object"></param>
		/// <param name="id"></param>
		void getObject(T* object, unsigned int id) {
			*object = *(T*)((unsigned char*)m_objectBuffer + id * sizeof(T) + (id >> 3) + 1);
		}
		/// <summary>
		/// safely copies object from id to pointer
		/// </summary>
		/// <param name="object"></param>
		/// <param name="id"></param>
		/// <returns></returns>
		bool getObject_safe(T* object, unsigned int id) {
			if (id > m_objectCount) {
				return false;
			}

			unsigned int bid = id >> 3;
			unsigned int bpid = bid * (1 + sizeof(T) * 8);
			unsigned int tid = id * sizeof(T) + bid + 1;

			unsigned char b = ((unsigned char*)m_objectBuffer)[bpid];
			if (b >> (id - (id >> 3 << 3)) & (unsigned char)0b00000001) {
				*object = *(T*)((unsigned char*)m_objectBuffer + id * sizeof(T) + bid + 1);
				return true;
			}

			return false;
		}
	};
}
#endif