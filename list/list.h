#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Generic node from a list.
/// It stores the data and a pointer to the next item on list
typedef struct node {
  void *data; /// data stored in the node

  struct node *next; /// pointer to next element on list

  struct node *prev; /// pointer to previous element on list
} Node;

/// List that stores the header and the tail of the list
typedef struct list {
  Node *header;

  Node *tail;

  size_t data_size;

  int (*cmp)(void *, void *); /// compare function
} List;

///  @brief Creates an empty list with the given data type and compare function
///  @param data_size size of data (sizeof)
///  @param compare function that compares a node and an id
///  @return the new created list
List *create_list(size_t data_size, int (*compare)(void *, void *));

///  @brief Adds a new node on the end of the list
///  @param list The list to be added
///  @param new_data  The new data
///  @details Creates a node, associates data with it, and adds it
/// to the end of the list
void push(List *list, void *new_data);

///  @brief Pops a node from the list
///  @param list the list
///  @return the first node of the list
///
Node *pop(List *list);

///  @brief  Search in the list given a key
///  @param list The list to be searched for
///  @param data The key to identify the node within the list
///  @return Returns the NODE in the list that matches the given key
///
///  \warning This function does not convert the Node to the corresponding
/// representation. The casting must be done by the function caller after
/// receiving the node
Node *find(List *list, void *data);

///  @brief  Deletes a node from the list
///  @param list list to be searched
///  @param data the key of the node to be deleted
///  @details This function finds the node that matches the given key and
/// deletes it from the list.
void delete_list(List *list, void *data);

/// @brief Deletes the list, freeing the memory
/// @param list the list to be deleted
void free_list(List *list);

#endif