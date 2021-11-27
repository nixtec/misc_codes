/*
 * queue-using-linked-list.c
 */
#include <stdio.h>
#include <stdlib.h>

struct queue_struct {
  position_t front;
  position_t rare;
};
typedef struct queue_struct queue_t;

void qinsert(queue_t *q, element_t e);
void qdelete(queue_t *q);

void qinsert(queue_t *q, element_t e)
{
  node_t *newnode; /* New node to be inserted */
  newnode = malloc(sizeof(node_t));
  newnode->next = NULL;
  if (q->front == NULL && q->rear == NULL) {
    front = newnode;
    rear = newnode;
  } else {
    q->rear->next = newnode;
    q->rear = newnode;
  }
}

void qdelete(queue_t *q)
{
  node_t *delnode; /* Node to be deleted */
  if (q->front == NULL && q->rear == NULL) {
    printf ("\nQueue is empty to delete any element\n");
  } else {
      delnode = q->front;
      q->front = q->front->next;
      free(delnode);
  }
}
