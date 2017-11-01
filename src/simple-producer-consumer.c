#include "learn_node.h"

#define PRODUCT_MAX_COUNT 10
#define PRODUCE_MAX_TIMES 15
#define CONSUMER_COUNT 5
#define PRODUCER_COUNT 3

int isEnd = FALSE;
int product_count = 0;
int produce_times = 0;
pthread_mutex_t produce_mutex;
pthread_cond_t consume_cond;
pthread_cond_t produce_cond;

typedef void* (*thread_routine)(void *pvoid);

void* consume(void *pvoid) {
  int pid = (int)pvoid;
  while (TRUE) {
    sleep((unsigned int)(random() % 3));
    RLOG("%d consume locking\n", pid);
    pthread_mutex_lock(&produce_mutex);
    if (isEnd && product_count < 1) {
      pthread_mutex_unlock(&produce_mutex);
      break;
    }
    RLOG("%d consume get mutex\n", pid);
    if (product_count < 1) {
      RLOG("%d waiting for produce\n", pid);
      pthread_cond_wait(&consume_cond, &produce_mutex);
      RLOG("%d continue consume\n", pid);
    }
    if (product_count > 0) {
      --product_count;
      pthread_cond_signal(&produce_cond);
      RLOG("%d consume product left %d\n", pid, product_count);
    }
    pthread_mutex_unlock(&produce_mutex);
  }
  RLOG("%d consume finished\n", pid);
  pthread_exit(NULL);
}

void* produce(void *pvoid) {
  int pid = (int)pvoid;
  while (!isEnd) {
    sleep((unsigned int)(random() % 3));
    pthread_mutex_lock(&produce_mutex);
    if (product_count >= PRODUCT_MAX_COUNT) {
      RLOG("%d waiting for consume\n", pid);
      pthread_cond_wait(&produce_cond, &produce_mutex);
      RLOG("%d continue produce\n", pid);
    }
    if (produce_times >= PRODUCE_MAX_TIMES) {
      RLOG("%d produce finisehd left %d\n", pid, product_count);
      isEnd = TRUE;
    } else {
      if (product_count < PRODUCT_MAX_COUNT) {
        ++product_count;
        ++produce_times;
      }
      RLOG("%d produce product left %d total %d\n",
           pid, product_count, produce_times);
    }
    pthread_cond_broadcast(&consume_cond);
    pthread_mutex_unlock(&produce_mutex);
  }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
  srandom((unsigned int)time(NULL));
  pthread_mutex_init(&produce_mutex, NULL);
  pthread_cond_init(&consume_cond, NULL);
  pthread_cond_init(&produce_cond, NULL);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  int totalThreads = CONSUMER_COUNT + PRODUCER_COUNT;
  pthread_t threads[totalThreads];
  for (int i = 0; i < totalThreads; ++i) {
    thread_routine routine = i >= CONSUMER_COUNT ? produce : consume;
    pthread_create(&threads[i], &attr, routine, (void*)i);
  }

  for (int i = 0; i < totalThreads; ++i) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&produce_mutex);
  pthread_cond_destroy(&consume_cond);
  pthread_cond_destroy(&produce_cond);
  pthread_attr_destroy(&attr);

  pthread_exit(NULL);
}
