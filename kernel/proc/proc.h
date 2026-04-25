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
    uint64_t ra;
    uint64_t sp;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
} context_t;

typedef struct
{
    int pid;
    proc_state_t state;
    pagetable_t *pagetable;
    uint8_t kernel_stack[KERNEL_STACK_SIZE];
    context_t context;
} proc_t;

extern proc_t procs[PROC_MAX];
extern proc_t *current_proc;

void proc_init(void);
proc_t *proc_create(void);
void proc_switch(context_t *old_c, context_t *new_c);
void yield(void);

#endif