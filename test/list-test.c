#include "list/list.h"
#include "ui/main-window.h"
int main(int argc, char *argv[]) {

  //  show_and_run();
  //
  List *list = (List *)malloc(sizeof(List));
  list->header = NULL;
  list->tail = NULL;
  list->data_size = sizeof(struct person);
  list->cmp = compare_person;

  struct person teste;
  teste.age = 14;
  teste.cpf = 15;
  push(list, &teste);

  printf("teste 1 created \n");

  struct person teste2;
  teste2.age = 145;
  teste2.cpf = 153;

  push(list, &teste2);

  printf("teste 2 created \n");

  struct person teste3;
  teste3.cpf = 15453;
  teste3.age = 14;

  push(list, &teste3);
  printf("teste 3 created \n");

  int search = 15453;
  delete_list(list, &search);
  Node *found = find(list, &search);

  if (!found) {
    printf("not found");
    return 0;
  }
  struct person *p_found = (struct person *)found->data;

  if (p_found)
    printf("%d", p_found->cpf);
  else
    printf("NULL \n");

  return 0;
}
