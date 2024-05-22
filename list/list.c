#include "list.h"

List *create_list(size_t data_size, int (*compare)(void *, void *)) {
  /// alocates space
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

Node *find(List *list, void *data) {
  Node *tmp = list->header;

  /// searches through all nodes
  while (tmp) {
    /// uses the compare function passed on the list creation
    if (list->cmp(tmp->data, data)) { /// found
      return tmp;
    }

    tmp = tmp->next;
  }

  /// node with data not found
  return NULL;
}

void delete_list(List *list, void *data) {
  /// searches for the node
  Node *place = find(list, data);

  /// if the element to be removed is the head of the list
  if (list->header == place) {
    if (place->prev) {
      place->prev = NULL;
    }
    list->header = place->next;

    /// if the element to be removed is the tail of the list
  } else if (list->tail == place) {
    place->prev->next = NULL;
    list->tail = place->prev;

    /// in the middle of the list
  } else {
    place->prev->next = place->next;
    place->next->prev = place->prev;
  }

  free(place);
}

Node *pop(List *list) {
  if (list->header) {
    Node *tmp = list->header; /// first element in the list

    list->header = list->header->next; /// new header is the second element

    if (list->header)
      list->header->prev = NULL;

    return tmp;
  }

  return NULL;
}