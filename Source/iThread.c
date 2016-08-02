#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "iThread.h"
#include "ReadyQueue.h"


#define switch_to(prev, next) 								\
{															\
			asm volatile ("movq %%rbp, %%rsp	\t\n"		\
			"popq %[prev_rbp]	\t\n"						\
			"movq %%rsp, %[prev_rsp]	\t\n"				\
			:[prev_rsp] "=r" (prev->rsp),					\
			 [prev_rbp] "=r" (prev->rbp)					\
			 :												\
			 :);											\
	/*Loading states*/										\
	asm volatile ("movq %[next_rsp], %%rbx	\t\n"			\
			"movq %%rbx, %%rsp 		\t\n"					\
			"movq %[next_rbp], %%rbp	\t\n"				\
			"ret							\t\n"			\
			:												\
			:[next_rsp] "r" (next -> rsp),					\
			 [next_rbp] "r" (next -> rbp)					\
			 :												\
	);														\
}															\

const int STACK_SIZE = 1048579; // 1Mb
struct Thread *running_thread;

void set_running_thread(struct Thread * thread)
{
	running_thread = thread;
	thread -> status = running_status;
}

void print_running_thread_id()
{
	printf("* Thread ID: %d ", running_thread -> tid);
}

void exiting_iThread()
{
	printf("\nExiting Thread: %d\n", running_thread -> tid);
	running_thread -> status = exit_status;
	scheduler();
}

void create_iThread(struct Thread *thread, void (* start_routine) (void))
{
	thread -> stack = (void *) calloc(STACK_SIZE, sizeof (char));
	thread -> rsp = thread -> stack + STACK_SIZE - 1;
	thread -> rbp = thread -> rsp;
	//thread -> eip = start_routine;

	thread -> rsp = (int *)thread -> rsp - 2;

	void **exiting_function;
	exiting_function = thread -> rsp;
	*exiting_function = exiting_iThread;

	thread -> rsp = (int *)thread -> rsp - 2;
	exiting_function = thread -> rsp;
	*exiting_function = start_routine;

	thread -> status = ready_status;
	enqueue(thread);

}



void scheduler()
{
	printf("\n* Entered in scheduller\n");
	struct Thread *prev;
	//struct Thread *next;

	prev = running_thread;

	if (prev -> status == running_status && prev != &mainThread)
	{
		enqueue(prev);
		prev -> status = ready_status;
	}

	if (ready_queue.count)
	{
		running_thread = dequeue();
		if (running_thread -> status == exit_status)
		{
			printf("\nXYS\n");
		}
		printf("Next Thread: %p\n\n", running_thread);
	}
	else
	{
		running_thread = &mainThread;
		printf("Next Thread: %p <Main Thread>\n\n", running_thread);
	}


	running_thread -> status = running_status;
	switch_to(prev, running_thread);
///////////////////////////////////////////////////
//	asm volatile ("movq %%rbp, %%rsp	\t\n"
//			"popq %[prev_rbp]	\t\n"
//			"movq %%rsp, %[prev_rsp]	\t\n"
//			:[prev_rsp] "=r" (prev->rsp),
//			 [prev_rbp] "=r" (prev->rbp)
//			 :
//			 :);
//	/*Loading states*/
//	asm volatile ("movq %[next_rsp], %%rbx	\t\n"
//			"movq %%rbx, %%rsp 		\t\n"
//			"movq %[next_rbp], %%rbp	\t\n"
//			"ret							\t\n"
//			:
//			:[next_rsp] "r" (running_thread -> rsp),
//			 [next_rbp] "r" (running_thread -> rbp)
//			 :
//	);
		//////////////////////////////////////////////

	printf("Ishan Varade\n");
}
