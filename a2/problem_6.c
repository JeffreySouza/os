#include "pthread.h"
#include "stdio.h"
#include "assert.h"
#include <stdlib.h>
#include <unistd.h>

#define MEMFENCE __sync_synchronize()

int num_threads;
int run_time;
int max_random;

unsigned int* globalSeed;

int volatile* tickets;
int volatile* entering;

int volatile continue_running;
int volatile in_cs;

int volatile trials_in_circle;
int volatile total_trials;

void lock(int pid) {
  entering[pid] = 1;

  MEMFENCE;

  int max = 0;
  int i;
  for (i=0; i<num_threads; ++i) {
    int ticket = tickets[i];
    max = ticket > max ? ticket : max;
  }

  tickets[pid] = max + 1;
  MEMFENCE;
  entering[pid] = 0;
  MEMFENCE;

  for (i=0; i<num_threads; ++i) {
    while (entering[i]) { sched_yield(); }
    MEMFENCE;
    while((tickets[i] != 0) &&
        ((tickets[i] < tickets[pid]) ||
        (tickets[pid] == tickets[i] && i < pid ))) { sched_yield(); }
  }
}

void unlock(int pid) {
  MEMFENCE;
  tickets[pid] = 0;
}

void critical_section(int in_circle ) {
  trials_in_circle += in_circle;
  total_trials++;

  MEMFENCE;
}

void* thread_start(void *arg) {
  int pid = (int)(size_t)arg;

  unsigned int seed = *globalSeed+pid;

  int x = rand_r(&seed)%max_random;
  int y = rand_r(&seed)%max_random;

  int in_circle = (x*x+y*y) < max_random*max_random;

  MEMFENCE;
  while(continue_running) {
    lock(pid);
    // CRITICAL SECTION
    critical_section(in_circle);

    unlock(pid);
    // NON-CRITICAL SECTION
  }

  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Wrong number of arguments\n");
    return 1;
  }

  globalSeed = malloc(sizeof(unsigned int));
  *globalSeed = (unsigned int)time(NULL);

  num_threads = atoi(argv[1]);
  run_time = atoi(argv[2]);

  continue_running = 1;
  max_random = 250;

  tickets  = malloc (num_threads * sizeof (tickets[0]));
  entering = malloc (num_threads * sizeof (entering[0]));

  pthread_t threads[num_threads];

  int i;
  for (i=0; i<num_threads; ++i) {
    pthread_create(&threads[i], NULL, &thread_start, (void*)(size_t)i);
  }

  sleep(run_time);
  continue_running = 0;

  for (i=0; i<num_threads; ++i) {
    pthread_join(threads[i], NULL);
  }

  float pi= 4*((float)trials_in_circle/(float)total_trials);

  printf("Pi approximation: %f\n",pi);

  return 0;
}
