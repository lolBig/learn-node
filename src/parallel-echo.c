#include "learn_node.h"

char letters[] = { '!', '@', '#', '$' };
int echo_count = sizeof(letters);
int echoTimes = 5;
int isEnd = FALSE;
int *wait_flags;
pthread_attr_t *attr;
pthread_cond_t *conds;
pthread_cond_t *start_cond;
pthread_t *threads;
pthread_mutex_t *mutexs;
pthread_barrier_t *barrier;
sem_t *sems;

void* echo(void *pvoid) {
  int i = (int)pvoid;
  RLOG("%d enter\n", i);
  RASSERT(!sem_wait(sems + i), "");
  RLOG("%d finished\n", i);
  pthread_exit(NULL);
}

int main(int argc, char **argv) {

  attr = RMALLOC(pthread_attr_t);
  pthread_attr_init(attr);
  pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE);

  wait_flags = RMALLOC_ARR(int, echo_count);
  conds = RMALLOC_ARR(pthread_cond_t, echo_count);
  mutexs = RMALLOC_ARR(pthread_mutex_t, echo_count);
  sems = RMALLOC_ARR(sem_t, echo_count);
  for (int i = 0; i < echo_count; ++i) {
    *(wait_flags + i) = 1;
     sem_open("/", 0);
    pthread_cond_init(conds + i, NULL);
    pthread_mutex_init(mutexs + i, NULL);
  }

  threads = RMALLOC_ARR(pthread_t, echo_count);
  for (int i = 0; i < echo_count; ++i) {
    pthread_create(threads + i, attr, echo, (void*)i);
  }
  for (int i = 0; i < echo_count; ++i) {
    pthread_join(*(threads + i), NULL);
  }

  for (int i = 0; i < echo_count; ++i) {
    sem_destroy(sems + i);
    pthread_mutex_destroy(mutexs + i);
    pthread_cond_destroy(conds + i);
  }
  RFREE(sems);
  RFREE(mutexs);
  RFREE(conds);
  RFREE(wait_flags);

  pthread_attr_destroy(attr);
  RFREE(attr);

  RFREE(threads);

  pthread_exit(NULL);
}
