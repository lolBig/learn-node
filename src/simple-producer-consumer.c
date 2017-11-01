#include "learn_node.h"

#define PRODUCT_MAX_COUNT 10
#define PRODUCE_MAX_TIMES 15

int isEnd = FALSE;
int product_count = 0;
int produce_times = 0;
pthread_mutex_t mutex;
pthread_cond_t consume_cond;
pthread_cond_t produce_cond;

void* consume(void *pVoid) {
  int pid = (int)pVoid;
  while (!isEnd || product_count > 0) {
    sleep((unsigned int)(random() % 3));
    if (product_count < 1) {
      RLOG("%d waiting for produce\n", pid);
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&consume_cond, &mutex);
      pthread_mutex_unlock(&mutex);
      RLOG("%d continue consume\n", pid);
    }
    pthread_mutex_lock(&mutex);
    --product_count;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&produce_cond);
    RLOG("%d consume product left %d\n", pid, product_count);
  }
  RLOG("%d consume finished\n", pid);
  pthread_exit(NULL);
}

void* produce(void *pVoid) {
  int pid = (int)pVoid;
  while (!isEnd) {
    sleep((unsigned int)(random() % 3));
    if (product_count >= PRODUCT_MAX_COUNT) {
      RLOG("%d waiting for consume\n", pid);
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&produce_cond, &mutex);
      pthread_mutex_unlock(&mutex);
      RLOG("%d continue produce\n", pid);
    }
    pthread_mutex_lock(&mutex);
    ++product_count;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&consume_cond);
    ++produce_times;
    RLOG("%d produce product left %d total %d\n", pid, product_count, produce_times);
    if (produce_times >= PRODUCE_MAX_TIMES) {
      RLOG("%d produce finisehd\n", pid);
      isEnd = TRUE;
    }
  }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
  srandom((unsigned int)time(NULL));
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&consume_cond, NULL);
  pthread_cond_init(&produce_cond, NULL);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t pconsume;
  pthread_t pproduce;
  pthread_create(&pconsume, &attr, consume, 0);
  pthread_create(&pproduce, &attr, produce, (void*)1);

  pthread_join(pconsume, NULL);
  pthread_join(pproduce, NULL);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&consume_cond);
  pthread_cond_destroy(&produce_cond);
  pthread_attr_destroy(&attr);

  pthread_exit(NULL);
}
