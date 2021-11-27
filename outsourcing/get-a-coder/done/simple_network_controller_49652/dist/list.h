/*
 * list.h
 */
#ifndef LIST_H
#define LIST_H

#include <stdio.h>

struct list_struct {
  void *data; /* data */
  struct list_struct *next; /* points to next entry */
};
typedef struct list_struct list_t;

/* list specific functions */
void list_add(list_t **head, void *data);
void list_del(list_t **head, void *data);

#endif
