#ifndef LEANUV_LEANUV_H
#define LEANUV_LEANUV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define RLOG(format, ...) fprintf(stdout, format, ##__VA_ARGS__)
#define RERROR(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#define RERROR_CODE(code) RERROR("%s\n", uv_strerror((int)code))
#define RASSERT(exp, msg) do {\
  if(!exp) { \
    RERROR(msg);\
  }\
} while(0)
#define RMALLOC(type) (type*)malloc(sizeof(type))
#define RMALLOC_ARR(type, size) (type*)malloc(sizeof(type) * size)
typedef unsigned int uint;

#endif //LEANUV_LEANUV_H
