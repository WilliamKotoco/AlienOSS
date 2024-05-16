#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// /// TEMP
// struct person {
//   int age;
//   int cpf;
// };

/// Generic node from a list. It stores the data and a pointer to the next item
/// on list
typedef struct node {
  void *data;        /// data stored in the node
  struct node *next; /// pointer to next element on list
  struct node *prev; /// pointer to previous element on list
} Node;

/// List that stores the header and the tail of the list
typedef struct list {
  Node *header;

  Node *tail;

  size_t data_size;

  int (*cmp)(void *, void *); // compare
                                          //

} List;

/// @brief Create a empty list with the given data type and compare function
/// @param data_size
/// @param compare
/// @return the new created list
List *create_list(size_t data_size,
                  int (*compare)(void *, void *));

/// @brief Adds a new node on the end of the list
/// @param list The list to be added
/// @param new_data  The new data to be added
///
/// This function creates a node, associates data with it, and adds
/// the recently created node to the end of the list
void push(List *list, void *new_data);

/// @brief Pop a node from the list
/// @param list the list
/// @return the first node of the list
Node *pop(List *list);

/// @brief  Search in the list given a key
/// @param list The list to be searched for
/// @param data The key to identify the node within the list
/// @return Returns the NODE in the list that matches the given key
///
/// \warning This function does not convert the Node to the corresponding
/// representation. It should be done by the function caller after receiving
/// the node
Node *find(List *list, void *data);

/// TEMP
///
int compare_person(void *d1, void *d2);

/// @brief  Delete a node from the list
/// @param list list to be searched
/// @param data the key of the node to be deleted
///
/// @details This function finds the node that matches the given key and
/// deletes it from the list.
void delete_list(List *list, void *data);
