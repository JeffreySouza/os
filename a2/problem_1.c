#include "pthread.h"
#include "stdio.h"
#include "assert.h"
#include <stdlib.h>
#include <unistd.h>

#define MEMFENCE __sync_synchronize()

int num_threads;
int run_time;

int volatile* tickets;
int volatile* entering;
int volatile* access_count;

int volatile continue_running;
int volatile in_cs;

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
    while (entering[i]) { /* nothing */ }
    MEMFENCE;
    while((tickets[i] != 0) &&
        ((tickets[i] < tickets[pid]) ||
        (tickets[pid] == tickets[i] && i < pid ))) { /* nothing */ }
  }
}

void unlock(int pid) {
  MEMFENCE;
  tickets[pid] = 0;
}

void critical_section(int pid) {
  assert(in_cs==0);
  in_cs++;
  assert(in_cs==1);
  in_cs++;
  assert(in_cs==2);
  in_cs++;
  assert(in_cs==3);
  in_cs=0;

  MEMFENCE;
  access_count[pid] = access_count[pid]+1;
}

void* thread_start(void *arg) {
  int pid = (int)(size_t)arg;

  MEMFENCE;
  while(continue_running) {
    lock(pid);
    // CRITICAL SECTION
    critical_section(pid);

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

  num_threads = atoi(argv[1]);
  run_time = atoi(argv[2]);

  continue_running = 1;
  in_cs = 0;

  tickets  = malloc (num_threads * sizeof (tickets[0]));
  entering = malloc (num_threads * sizeof (entering[0]));
  access_count = calloc (num_threads, sizeof (access_count[0]));

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

  for (i=0; i<num_threads; ++i) {
    printf("Thread %d: %d\n",i,access_count[i]);
  }
  return 0;
}
