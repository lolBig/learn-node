#include "learn_node.h"

//signal is a better choice

char letters[] = { '0', '1', '2', '3', '4', '5', '6', '7' };
int echo_count = sizeof(letters);
int echo_times = 1;
int *wait_flags = NULL;
pthread_attr_t *attr = NULL;
pthread_cond_t *conds = NULL;
pthread_t *threads = NULL;
pthread_mutex_t *mutex = NULL;

void* echo(void *pvoid) {
  int i = (int)pvoid;
  int *wait_flag = wait_flags + i;
  pthread_cond_t *cond = conds + i;
  RLOG("%d lock\n", i);
  pthread_mutex_lock(mutex);
  RLOG("%d get mutex\n", i);
  while(TRUE) {
    while(*wait_flag) {
      RLOG("%d waiting\n", i);
      pthread_cond_wait(cond, mutex);
      RLOG("%d wakeup\n", i);
    }
    if (i < echo_count - 1) {
      *(wait_flag + 1) = FALSE;
      pthread_cond_signal(cond + 1);
    }
    if (echo_times > 0) {
      if (i >= echo_count - 1) {
        --echo_times;
        *wait_flags = FALSE;
        pthread_cond_signal(conds);
      }
      RLOG("[%c]", letters[i]);
      *wait_flag = TRUE;
    } else {
      break;
    }
  }
  pthread_mutex_unlock(mutex);
  RLOG("%d finished\n", i);
  pthread_exit(NULL);
}

int main(int argc, char **argv) {

  attr = RMALLOC(pthread_attr_t);
  pthread_attr_init(attr);
  pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE);

  mutex = RMALLOC(pthread_mutex_t);
  pthread_mutex_init(mutex, NULL);

  wait_flags = RMALLOC_ARR(int, echo_count);
  conds = RMALLOC_ARR(pthread_cond_t, echo_count);
  for (int i = 0; i < echo_count; ++i) {
    *(wait_flags + i) = TRUE;
    pthread_cond_init(conds + i, NULL);
  }

  threads = RMALLOC_ARR(pthread_t, echo_count);
  for (int i = 0; i < echo_count; ++i) {
    pthread_create(threads + i, attr, echo, (void*)i);
  }

  RLOG("start echo \n");
  pthread_mutex_lock(mutex);
  RLOG("start get mutex\n");
  *wait_flags = FALSE;
  pthread_cond_signal(conds);
  pthread_mutex_unlock(mutex);

  for (int i = 0; i < echo_count; ++i) {
    pthread_join(*(threads + i), NULL);
  }

  for (int i = 0; i < echo_count; ++i) {
    pthread_cond_destroy(conds + i);
  }
  RFREE(conds);
  RFREE(wait_flags);
  pthread_mutex_destroy(mutex);
  RFREE(mutex);

  pthread_attr_destroy(attr);
  RFREE(attr);

  RFREE(threads);

  pthread_exit(NULL);
}
