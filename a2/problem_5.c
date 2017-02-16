struct spin_lock_t {
  volatile int lock;
}

/*
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
  //while(atomic_xadd(s,s->lock,1));
}

void spin_unlock (struct spin_lock_t *s) {
  s->lock = 0;
}
