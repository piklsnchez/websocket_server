#include "linked_list.h"
#include <stdlib.h>

struct linked_list* linked_list_new(){
  struct linked_list* this = (struct linked_list*)malloc(sizeof(struct linked_list));
  this->append             = linked_list_append;
  this->delete             = linked_list_delete;
  this->_node              = NULL;
  return this;
}

void linked_list_delete(struct linked_list* this){
  struct linked_list_node* current = this->_node;
  struct linked_list_node* deleteMe;
  while(current != NULL){
    free(current->val);
    deleteMe = current;
    current  = current->next;
    free(deleteMe);
  }
  free(this);
}

struct linked_list* linked_list_append(struct linked_list* this, char* data){
  struct linked_list_node* current = this->_node;
  while(current != NULL){
    current = current->next;
  }
  current->next = (struct linked_list_node*)malloc(sizeof(struct linked_list_node));
  current->next->val = data;
  current->next->next = NULL;
  return this;
}
