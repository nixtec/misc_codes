
/* ******************* LINKED LIST FUNCTIONS ************** */
/* inserts an element at the head of the list: O(1) */
list_t list_insert(list_t l, element_t e)
{
  list_t node;

  node = malloc(sizeof(list_t));
  if (!node) {
    perror("malloc");
    exit(1);
  }
  node->e = e;
  node->next = l;
  l = node;

  return l;
}

/* deletes e from the list, fee if 'free_element' is true: O(n) */
list_t list_delete(list_t l, element_t e, bool free_element)
{
  list_t prev, cur;

  if (!l) {
    fprintf(stderr, "List empty!\n");
    return l;
  }

  prev = NULL;
  cur = l;
  while (cur) {
    if (element_compare(e, cur->e) == 0) {
      if (!prev) { /* no previous, head to be deleted */
	l = cur->next;
      } else {
	prev->next = cur->next;
      }
      if (free_element) {
	free(cur->e);
      }
      free(cur);
      break;
    }
    prev = cur;
    cur = cur->next;
  }

  return l;
}

/* find an element in the list */
position_t list_find(list_t l, element_t e)
{
  if (!l)
    return NULL;

  while (l) {
    if (element_compare(e, l->e) == 0)
      break; /* found */
    l = l->next;
  }

  return l;
}

/* destroy the list, free element if 'free_element' is true */
list_t list_destroy(list_t l, bool free_element)
{
  list_t temp1, temp2;

  temp2 = l;

  while (temp2) {
    temp1 = temp2;
    temp2 = temp1->next;
    if (free_element == true) {
      free(temp1->e);
    }
    free(temp1);
  }

  return NULL;
}

