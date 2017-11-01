#include "learn_node.h"

#define PRODUCT_MAX_COUNT 10

int isEnd = FALSE;
int product_count = 0;
int produce_times = 0;
pthread_mutex_t mutex;
pthread_cond_t consume_cond;
pthread_cond_t produce_cond;

void* consume(void *nul) {
  while (!isEnd || product_count > 0) {
    if (product_count < 1) {
      RLOG("waiting for produce\n");
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&consume_cond, &mutex);
      pthread_mutex_unlock(&mutex);
      RLOG("continue consume\n");
    }
    pthread_mutex_lock(&mutex);
    --product_count;
    RLOG("consume product left %d\n", product_count);
    pthread_cond_signal(&produce_cond);
    pthread_mutex_unlock(&mutex);
  }
  RLOG("consume finished\n");
  pthread_exit(NULL);
}

void* produce(void *nul) {
  while (!isEnd) {
    if (product_count >= PRODUCT_MAX_COUNT) {
      RLOG("waiting for consume\n");
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&produce_cond, &mutex);
      pthread_mutex_unlock(&mutex);
      RLOG("continue produce\n");
    }
    pthread_cond_signal(&consume_cond);
    pthread_mutex_lock(&mutex);
    ++product_count;
    RLOG("produce product left %d\n", product_count);
    pthread_mutex_unlock(&mutex);
    ++produce_times;
    if (produce_times > 20) {
      RLOG("produce finisehd\n");
      isEnd = TRUE;
    }
  }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&consume_cond, NULL);
  pthread_cond_init(&produce_cond, NULL);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t pconsume;
  pthread_t pproduce;
  pthread_create(&pconsume, &attr, consume, NULL);
  pthread_create(&pproduce, &attr, produce, NULL);

  pthread_join(pconsume, NULL);
  pthread_join(pproduce, NULL);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&consume_cond);
  pthread_cond_destroy(&produce_cond);
  pthread_attr_destroy(&attr);

  pthread_exit(NULL);
}
