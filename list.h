//list.h
#include "basic.h"

// ############## change it if you need more bit ##############
typedef LISTTYPE list_type;
typedef LISTTYPE2 list_type2;
typedef LISTTYPE3 list_type3;
typedef LISTTYPE4 list_type4;
// -.-.-.-.-.-.-.-..-.-.


typedef struct node {
	struct node *next;
	struct node *prev;
	void *dataptr;
	void *data2ptr;
	void *data3ptr;
	void *data4ptr;
} node;

typedef struct list {
	node *head, *tail;
} list;

void list_init(list *myroot);
void list_put(list *myroot, void* dataptr, void* data2ptr,void* data3ptr,void* data4ptr);
// node *list_get(list *myroot);
void list_remove(list *myroot, node* mynode);
// void list_pop(list *myroot);
void list_pop_min(list *myroot, list_type** dataptr, list_type2** data2ptr, list_type3** data3ptr, list_type4** data4ptr);
//unsigned char list_pop_min_by_value2(list *myroot, list_type** dataptr, list_type2** data2ptr, list_type2 temp);
//unsigned char list_pop_first_by_value2(list *myroot, list_type** dataptr, list_type2** data2ptr, list_type3** data3ptr, list_type2 temp);
//unsigned char list_pop_Nth_by_value2(list *myroot, list_type** dataptr, list_type2** data2ptr, list_type3** data3ptr, list_type2 temp,long n);
unsigned char list_pop_max_by_value2(list *myroot, list_type** dataptr, list_type2** data2ptr, list_type3** data3ptr,list_type4** data4ptr, list_type2 temp);
unsigned char list_empty(list *myroot);
// void* list_popvalue2ptr(list *myroot);
// void* list_getvalueptr(list *myroot);
void list_stamp(list *myroot);
// unsigned char checklist(list * myroot, int n);
