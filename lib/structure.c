typedef struct List {
  struct List *prev;
  struct List *next;
  void *value;
} List;

#define LIST_GET_VALUE(list, t) (t)(list->value)
