#ifndef _POOL_H_
#define _POOL_H_

#include "ccprimitives.h"





//#define ALLOCATOR_FAKE


// -----------------------------------------
// Fake allocator
// -----------------------------------------
	#define PSIZE	4096
	#ifndef CACHE_LINE_SIZE
	#	define CACHE_LINE_SIZE 64
	#endif

	typedef struct PoolStruct
	{
		void	*p[PSIZE];
		int		index;
		int		obj_size;
	} PoolStruct;

	inline static void fake_initAllocator(PoolStruct *pool, int obj_size)
	{
		void *objects;
		int i;
		objects = getAlignedMemory(CACHE_LINE_SIZE, PSIZE * obj_size);
		pool->obj_size = obj_size;
		pool->index = 0;
		for (i = 0; i < PSIZE; i++)
			pool->p[i] = (void *)(objects + (int)(i * obj_size));
	}

	inline static void *fake_malloc(PoolStruct *pool)
	{
		if (pool->index == PSIZE)
		{
			int size = pool->obj_size;
			fake_initAllocator(pool, size);
		}

		return pool->p[pool->index++];
	}

	inline static void fake_free(PoolStruct *pool, void *obj)
	{
		if (pool->index > 0)
			pool->p[--pool->index] = obj;
	}


// -----------------------------------------
// Allocator chooser
// -----------------------------------------
	inline static void init_pool(PoolStruct *pool, int obj_size)
	{
		#ifdef ALLOCATOR_FAKE
			fake_initAllocator(pool, obj_size);
		#endif
	}

	inline static void *alloc_obj(PoolStruct *pool)
	{
		#ifdef ALLOCATOR_FAKE
			return fake_malloc(pool);
		#else
//TODO check the size of malloc
			void *res = malloc(200);
			return res;
		#endif
	}

	inline static void free_obj(PoolStruct *pool, void *obj)
	{
		#ifdef ALLOCATOR_FAKE
			fake_free(pool, obj);
		#else
			free(obj);
		#endif
	}

/*
TODO This function is never used
inline static void rollback(PoolStruct *pool, int num_objs) {
    if (pool->index - num_objs >= 0)
        pool->index -= num_objs;
    else
        pool->index = 0;
}
*/
#endif
