#include <stdlib.h>
#include <stdbool.h>
#include <numa.h>
#include "main.h"
#include "hsynch.h"
#include "ccpool.h"

#define  GUARD          INT_MIN






typedef struct ListNode
{
	int64_t val;
	struct ListNode *next;
} ListNode;




//TODO
//ListNode guard_node CACHE_ALIGN = {GUARD, null};
//volatile ListNode *first CACHE_ALIGN = &guard_node;
//volatile ListNode *last CACHE_ALIGN = &guard_node;

ListNode *guard_node CACHE_ALIGN;
volatile ListNode *first CACHE_ALIGN;
volatile ListNode *last CACHE_ALIGN;

HSynchStruct enqueue_lock CACHE_ALIGN;
HSynchStruct dequeue_lock CACHE_ALIGN;

__thread HSynchThreadState lenqueue_lock;
__thread HSynchThreadState ldequeue_lock;

__thread PoolStruct pool_node;

volatile int a CACHE_ALIGN = 1;

static RetVal enqueue(void* state, ArgVal arg, int pid) {
     ListNode *node = alloc_obj(&pool_node);
     node->next = null;
     node->val = arg;
     last->next = node;
     last = node;
     return -1;
}

static RetVal dequeue(void* state, ArgVal arg, int pid) {
	RetVal ret;
	volatile ListNode *node = first;
	ListNode *new_first = first->next;
	if (new_first == null)
	{
		return -1;
	}
	ret = new_first->val;
	first = new_first;
	free_obj(&pool_node, (void*)node);
	return ret;
}

#define ENQ_ARG 0x1010
#define DEQ_ARG 0x2020
#ifdef INLINE_CS

RetVal inline_cs(ArgVal arg, int pid) {
	if (arg == ENQ_ARG)
		return enqueue(arg, pid);
	else
		return dequeue(arg, pid);
}
#endif

void global_init(void)
{
	HSynchStructInit(&enqueue_lock);
	HSynchStructInit(&dequeue_lock);

//TODO ASK
	init_pool(&pool_node, CACHE_LINE_SIZE);

	ListNode gn CACHE_ALIGN = {GUARD, null};

	guard_node	= alloc_obj(&pool_node);
	*guard_node	= gn;
	first		= guard_node;
	last		= guard_node;
}

void thread_init(void)
{
	HSynchThreadStateInit(&lenqueue_lock, id);
    HSynchThreadStateInit(&ldequeue_lock, id);
    init_pool(&pool_node, CACHE_LINE_SIZE);
}

void user_iteration(int i)
{
	if (i % 2)
        applyOp(&dequeue_lock, &ldequeue_lock, dequeue, null, (ArgVal) DEQ_ARG, id);
    else
        applyOp(&enqueue_lock, &lenqueue_lock, enqueue, null, (ArgVal) ENQ_ARG, id);
}

bool is_server(int id)
{
	return false;
}

int server_count(void)
{
	return 0;
}

void warmup_server(int warmup)
{
}

int run_server(void)
{
	return 0;
}

void stop_server(int id) {}

void thread_cleanup() {}

void global_cleanup() {}
