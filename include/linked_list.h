#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

struct linked_list_node {
  char* val;
  struct linked_list_node* next;
};

struct linked_list {
  struct linked_list_node* _node;
  struct linked_list* (*append)(struct linked_list* this, char* data);
  void (*delete)(struct linked_list* this);
};

struct linked_list* linked_list_new();
void                linked_list_delete(struct linked_list* this);
struct linked_list* linked_list_append(struct linked_list* this, char* data);

#endif
