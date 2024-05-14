#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List *create_list(size_t data_size,
                  void (*compare)(const void *,
                                  const void *)) { // se der erro é o compare
  List *new_list = malloc(sizeof(List));

  new_list->header = new_list->tail = NULL;
  new_list->cmp = compare;
  new_list->data_size = data_size;

  return new_list;
}

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

// /// @brief  TEMP
// /// @param d1
// /// @param d2
// /// @return
// int compare_person(void *d1, void *d2) {

//   struct person *p1 = (struct person *)d1;

//   int d11 = p1->cpf;

//   int *d22 = (int *)d2;

//   if (d11 == *d22) {
//     return 1;
//   } else {
//     return 0;
//   }
// }

Node *find(List *list, void *data) {
  Node *tmp = list->header;

  while (tmp) {
    /// use compare function passed to the list struct.
    if (list->cmp(tmp->data, data))
      return tmp;
    tmp = tmp->next;
  }

  return NULL;
}

void delete_list(List *list, void *data) {
  Node *place = find(list, data);

  /// if the element to be removed is the head of the list
  if (list->header == place) {
    place->prev = NULL;
    list->header = place->next;

    /// if the element to be removed is the tail of the list
  } else if (list->tail == place) {
    place->prev->next = NULL;
    list->tail = place->prev;
  } else {
    place->prev->next = place->next;
    place->next->prev = place->prev;
  }
}

Node *pop(List *list) {
  Node *tmp = list->header; /// first element in the list

  list->header = list->header->next; /// new header is the second element
  list->header->prev = NULL;

  return tmp;
}