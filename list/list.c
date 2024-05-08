#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void push(List *list, void *new_data) {
  /// Memory for the new node
  Node *new_node = malloc(sizeof(Node));

  /// memory space allocated for receive the type
  new_node->data = malloc(list->data_size);

  memcpy(new_node->data, new_data, list->data_size);

  /// if the list is empty
  if (!list->header) {
    list->header = new_node;
    list->tail = new_node;
    new_node->prev = NULL;
    new_node->next = NULL;
  }

  else {
    list->tail->next = new_node;
    new_node->prev = list->tail;
    new_node->next = NULL;
    list->tail = new_node;
  }
}

/// compares between a person and a cpf, if the cpf is found
int compare_person(void *d1, void *d2) {

  struct person *p1 = (struct person *)d1;

  int d11 = p1->cpf;

  int *d22 = (int *)d2;

  if (d11 == *d22) {
    return 1;
  } else {
    return 0;
  }
}

Node *find(List *list, void *data) {
  Node *tmp = list->header;

  while (tmp) {
    if (list->cmp(tmp->data, data))
      return tmp;
    tmp = tmp->next;
  }

  return NULL;
}

void delete_list(List *list, void *data) {
  Node *place = find(list, data);

  if (list->header == place) {
    place->prev = NULL;
    list->header = place->next;

  } else if (list->tail == place) {
    place->prev->next = NULL;
    list->tail = place->prev;
  } else {
    place->prev->next = place->next;
    place->next->prev = place->prev;
  }
}
