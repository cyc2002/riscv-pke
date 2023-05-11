#ifndef _SCHED_H_
#define _SCHED_H_

#include "process.h"

//length of a time slice, in number of ticks
#define TIME_SLICE_LEN  2
#define SEM_MAX 32
int semused;
int semval[SEM_MAX];
void insert_to_ready_queue( process* proc );
void insert_to_sem_queue(process *proc,int semid);
int semschedule(int semid);
void schedule();

#endif
