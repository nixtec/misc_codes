/*
 * queue.c
 */
#include "queue.h"

/* insert a job to the queue */
lpos_t qinsert(queue_t *q, lelement_t e)
{
  if (!q) {
    fprintf(stderr, "No queue supplied!\n");
    return NULL;
  }

  lnode_t *newnode; /* New node to be inserted */
  newnode = malloc(sizeof(lnode_t));
  newnode->e = e;
  newnode->next = NULL;
  if (q->front == NULL) {
    q->front = newnode;
    q->rear = newnode;
  } else {
    q->rear->next = newnode;
    q->rear = newnode;
  }

  return q->rear; /* always rear */
}

/* delete node from queue, return the element */
lelement_t qdelete(queue_t *q)
{
  lpos_t delnode; /* Node to be deleted */
  lelement_t e = NULL;

  if (!q) {
    return NULL;
  }

  if (q->front) {
    delnode = q->front; /* saving the pointer to free() it */
    e = delnode->e;
    q->front = q->front->next; /* update the front of the queue */
    free(delnode);
  }

  return e;
}

/* destroy the Queue, free element if 'free_element' is true */
void qdestroy(queue_t *q, bool free_element)
{
  lpos_t delnode; /* Node to be deleted */

  if (!q) {
    return;
  }

  while (q->front) {
    delnode = q->front; /* saving the pointer to free() it */
    q->front = q->front->next; /* update the front of the queue */
    if (free_element == true && delnode->e != NULL) {
      free(delnode->e);
    }
    free(delnode);
  }
}
/* calculate length of a queue */
size_t qlength(queue_t *q)
{
  int len;
  lpos_t temp;

  if (!q)
    return 0;

  temp = q->front; /* we start from front and iterate until we get NULL */
  len  = 0;
  while (temp) {
    len++;
    temp = temp->next;
  }

  return len;
}

