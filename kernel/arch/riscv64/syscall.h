#ifndef SYSCALL_H
#define SYSCALL_H

#include "trap.h"
#include <stddef.h>

void syscall_dispatch(trap_frame_t *frame);

#endif