/*
 * tree.c
 */
#include "tree.h"

tree_t tree_make_empty(tree_t t)
{
  if (t) {
    tree_make_empty(t->left);
    tree_make_empty(t->right);
    free(t);
  }
  return NULL;
}

position_t tree_find(element_t e, tree_t t)
{
  int result = 0;

  if (!t)
    return NULL;

  result = element_compare(e, t->e);
  if (result < 0)
    return tree_find(e, t->left);
  if (result > 0)
    return tree_find(e, t->right);

  /* else */
  return t;
}

position_t tree_find_min(tree_t t)
{
  if (!t)
    return NULL;

  if (!t->left)
    return t;

  return tree_find_min(t->left);
}

position_t tree_find_max(tree_t t)
{
  if (!t) {
    while (t->right) {
      t = t->right;
    }
  }

  return t;
}

tree_t tree_insert(element_t e, tree_t t)
{
  int result = 0;

  if (!t) {
    /* create one-node tree */
    t = malloc(sizeof(node_t));
    if (!t) {
      perror("malloc");
      exit(1); /* we can't progress, no memory available */
    } else {
      t->e = e;
      t->left = t->right = NULL;
    }
  } else {
    result = element_compare(e, t->e);
    if (result < 0) {
      t->left = tree_insert(e, t->left);
    } else if (result > 0) {
      t->right = tree_insert(e, t->right);
    }
    /* otherwise element is in the tree, do nothing */
  }

  return t;
}

tree_t tree_delete(element_t e, tree_t t)
{
  int result = 0;
  position_t tmp;

  if (!t) {
    fprintf(stderr, "Tree empty!\n");
  } else {
    result = element_compare(e, t->e);

    if (result < 0) { /* go left */
      t->left = tree_delete(e, t->left);
    } else if (result > 0) { /* go right */
      t->right = tree_delete(e, t->right);
    } else { /* found element */
      if (t->left && t->right) { /* both children exist */
	tmp = tree_find_min(t->right);
	t->e = tmp->e;
	t->right = tree_delete(t->e, t->right);
      } else { /* one or zero children */
	tmp = t;
	if (!t->left) {
	  t = t->right;
	} else if (!t->right) {
	  t = t->left;
	}
	free(tmp);
      }
    }
  }

  return t;
}

element_t tree_get_element(position_t p)
{
  return p->e;
}
