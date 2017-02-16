/*---------------------------------------
/--not complete--------------------------
/----------------------------------------*/

#include "pthread.h"
#include "stdio.h"
#include "assert.h"
#include <stdlib.h>
#include <unistd.h>

int num_threads;
int run_time;

int volatile continue_running;

struct spin_lock_t {
  volatile int lock;
};

 /*
 *
 * atomic_xadd
 *
 * equivalent to atomic execution of this code:
 *
 * return (*ptr)++;
 *
 */
static inline int atomic_xadd(volatile int *ptr) {
  register int val __asm__("eax") = 1;
  asm volatile("lock xaddl %0,%1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );
  return val;
}

void spin_lock (struct spin_lock_t *s) {
  while(atomic_cmpxchg(&(s->lock),s->lock,1));
}

void spin_unlock (struct spin_lock_t *s) {
  s->lock = 0;
}

void* thread_start(void* arg) {
  (void)(arg); // get rid of unused warning

  struct spin_lock_t sl;
  sl.lock = 0;
  spin_lock(&sl);
  spin_unlock(&sl);

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

  pthread_t threads[num_threads];

  int i;
  for (i=0; i<num_threads; ++i) {
    pthread_create(&threads[i], NULL, &thread_start, NULL);
  }

  sleep(run_time);
  continue_running = 0;

  for (i=0; i<num_threads; ++i) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
