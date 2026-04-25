#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include "../mm/vm.h"

#define PROC_MAX 16
#define KERNEL_STACK_SIZE 4096

typedef enum
{
    PROC_UNUSED = 0,
    PROC_RUNNABLE,
    PROC_RUNNING,
    PROC_SLEEPING,
    PROC_DEAD
} proc_state_t;

typedef struct
{
    int pid;
    proc_state_t state;
    pagetable_t *pagetable;
    uint8_t kernel_stack[KERNEL_STACK_SIZE];
    uint8_t trap_stack[KERNEL_STACK_SIZE];
    uint64_t sp;
} proc_t;

extern proc_t procs[PROC_MAX];
extern proc_t *current_proc;
extern volatile int need_yield;

void proc_init(void);
proc_t *proc_create(void (*entry)(void));
void proc_switch(uint64_t *prev_sp, uint64_t *next_sp);
void yield(void);

#endif