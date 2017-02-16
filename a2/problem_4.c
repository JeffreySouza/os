struct spin_lock_t {
  volatile int lock;
}

/*
 * atomic_cmpxchg
 *
 * equivalent to atomic execution of this code:
 *
 * if (*ptr == old) {
 *   *ptr = new;
 *   return old;
 * } else {
 *   return *ptr;
 * }
 *
 */
static inline int atomic_cmpxchg (volatile int *ptr, int old, int new) {
  int ret;
  asm volatile ("lock cmpxchgl %2,%1"
    : "=a" (ret), "+m" (*ptr)
    : "r" (new), "0" (old)
    : "memory");
  return ret;
}

void spin_lock (struct spin_lock_t *s) {
  while(atomic_cmpxchg(s,s->lock,1));
}

void spin_unlock (struct spin_lock_t *s) {
  s->lock = 0;
}
