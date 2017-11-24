#include <stdlib.h>
#include <stdbool.h>
#include "backoff.h"
#include "main.h"
#include "ccpool.h"

#define  GUARD          INT_MIN

typedef struct ListNode {
    int64_t value;
    struct ListNode *next;
} ListNode;

volatile ListNode *top CACHE_ALIGN = NULL;

__thread PoolStruct pool;
__thread BackoffStruct backoff;

static void push(ArgVal arg) {
    ListNode *node = alloc_obj(&pool);
    reset_backoff(&backoff);
    node->value = arg;
    do {
        ListNode *old_top = (ListNode*)top;
        node->next = old_top;
        if (CASPTR(&top, old_top, node) == true)
            break;
        else backoff_delay(&backoff);
    } while(true);
}

static RetVal pop() {
    reset_backoff(&backoff);
    do {
        ListNode *old_top = (ListNode*) top;
        if (old_top == NULL)
            return (RetVal) 0;
        if(CASPTR(&top, old_top, old_top->next))
            return old_top->value;
        else backoff_delay(&backoff);
    } while (true) ;
}

void global_init(void)
{   
}

void thread_init(void)
{
    init_pool(&pool, sizeof(ListNode));
}

void user_iteration(int i)
{
	if (i % 2)
        pop();
    else
        push(id*i);
}

bool is_server(int id)
{
	return false;
}

int server_count(void)
{
	return 0;
}

int run_server(void)
{
	return 0;
}

void stop_server(int id) {}

void thread_cleanup() {}

void global_cleanup() {}
