

/* calculate length of a queue */
int qlength(queue_t *q)
{
  int len;
  position_t temp;

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

