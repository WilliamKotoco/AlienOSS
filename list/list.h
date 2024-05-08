#include <stdio.h>
#include <stdlib.h>

/// TEMP
struct person {
  int age;
  int cpf;
};

/// Generic node from a list. It stores the data and a pointer to the next item
/// on list
typedef struct Node {

  void *data;        /// data stored in the node
  struct Node *next; /// pointer to next element on list
  struct Node *prev; /// pointer to previous element on list
} Node;

/// List that stores the header and the tail of the list
typedef struct List {
  Node *header;

  Node *tail;

  size_t data_size;

  int (*cmp)(const void *, const void *); // compare
                                          //

} List;

List *create_list(size_t data_size,
                  void (*compare)(const void *, const void *));

/// Function to add a new Node on the end of the list.
/// Receives the list, the new_data and the size of the data type
///
/// This function supports pushing in the generic list created by receiving a
/// pointer to the data to be createdd
void push(List *list, void *new_data);

/// TEMP
Node *find(List *list, void *data);

/// TEMP
///
int compare_person(void *d1, void *d2);
/// TEMP
void delete_list(List *list, void *data);
