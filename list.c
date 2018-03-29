//list.c
#include "list.h"

void list_init(list * myroot)
{
	myroot->head = NULL;
	myroot->tail = NULL;
}

void list_put(list * myroot, void* valueptr,void* value2ptr, void* value3ptr, void* value4ptr)
{
	if(VERBOSELIST==1){
		printf("before put\n");
		list_stamp(myroot);
	}

	node *mynode = (node*)malloc(sizeof(node));
    if(mynode==NULL){
        perror("list_put malloc error");
        exit(EXIT_FAILURE);
    }
	// OLD errore particolare... sto cercando di aggiungere nella lista un elemento, ma nell array risulta che in quel particolare indice dell array ci sia gia un altro elemento e che non sia impostato tutto a NULL...
/*
	if(mynode->dataptr!=NULL && mynode->data2ptr!=NULL){
		perror("errore in list put");
		exit(EXIT_FAILURE);
	}
*/
	mynode->dataptr = valueptr;
	mynode->data2ptr = value2ptr;
	mynode->data3ptr = value3ptr;
	mynode->data4ptr = value4ptr;
	mynode->next = NULL;
	if (myroot->tail != NULL) {
		mynode->prev = myroot->tail;
		myroot->tail->next = mynode;
	}
	myroot->tail = mynode;
	if (myroot->head == NULL) {
		mynode->prev = NULL;
		myroot->head = mynode;
	}
	if(VERBOSELIST==1){
		printf("after put\n");
		list_stamp(myroot);	
	}
}

void list_pop_min(list * myroot, list_type **dataptr, list_type2 **data2ptr, list_type3 **data3ptr, list_type4 **data4ptr)
{
	if(VERBOSELIST==1){
		printf("before pop min\n");
		list_stamp(myroot);
	}
	node *mynode = myroot->head;
	*dataptr = NULL;
	*data2ptr = NULL;
	*data3ptr = NULL;
	*data4ptr = NULL;
	//dataptr = dataptr;
	//data2ptr = data2ptr;
	node *myminnode = NULL;
	if(mynode!=NULL){
		for(;;){
			if(myminnode==NULL)
				myminnode=mynode;
			else{
				if (mynode->dataptr!=NULL && mynode->data2ptr!=NULL && myminnode->dataptr!=NULL && myminnode->data2ptr!=NULL && (*(list_type*)myminnode->dataptr > *(list_type*)mynode->dataptr)){
					myminnode=mynode;
				}
				if(mynode->next!=NULL){
					mynode=mynode->next;
				}
				else
					break;
			}
		}
	}	
	*dataptr = myminnode->dataptr;
	*data2ptr = myminnode->data2ptr;
	*data3ptr = myminnode->data3ptr;
	*data4ptr = myminnode->data4ptr;
	// min node removing
	list_remove(myroot,myminnode);
	if(VERBOSELIST==1){
		printf("after pop min\n");
		list_stamp(myroot);
	}

}


void list_remove(list * myroot, node* mynode)
{
	if(VERBOSELIST==1){
		printf("prima remove\n");
		list_stamp(myroot);
	}
	if(mynode->next==NULL){
		if(mynode->prev==NULL){
			myroot->tail=NULL;
			myroot->head=NULL;
		}
		else{
			myroot->tail=mynode->prev;
			mynode->prev->next=NULL;
		}
	}
	else{
		if(mynode->prev==NULL){
			mynode->next->prev=NULL;
			myroot->head=mynode->next;
        	}
		else{
			mynode->next->prev=mynode->prev;
			mynode->prev->next=mynode->next;
		}
	}
	free(mynode);
	if(VERBOSELIST==1){
		printf("dopo remove\n");
		list_stamp(myroot);
	}

}
/*
unsigned char list_pop_min_by_value2(list* myroot, list_type** dataptr, list_type2** data2ptr,list_type2 value2){
	if(VERBOSELIST==1){
		printf("before pop min by value2\n");
		list_stamp(myroot);
	}
	node *mynode = myroot->head;
	*dataptr = NULL;
	*data2ptr = NULL;
//	dataptr = dataptr;
//	data2ptr = data2ptr;
	node *myminnode = NULL;
	if(mynode!=NULL){
		for(;;){
			if(myminnode==NULL)
				myminnode=mynode;
			else{
				if (mynode->dataptr!=NULL && mynode->data2ptr!=NULL && myminnode->dataptr!=NULL && myminnode->data2ptr!=NULL){
					if(((*(list_type*)myminnode->dataptr > *(list_type*)mynode->dataptr) && ((*(list_type2*)mynode->data2ptr )==value2)) || (((*(list_type2*)myminnode->data2ptr)!=value2)&&((*(list_type2*)mynode->data2ptr)==value2))){
					myminnode=mynode;
					}
				}
				if(mynode->next!=NULL){
					mynode=mynode->next;
				}
				else
					break;
			}
		}
	}	
	*dataptr = myminnode->dataptr;
	*data2ptr = myminnode->data2ptr;
	if((*(list_type2*)myminnode->data2ptr)!=value2)
		return	(unsigned char)0;
	// min node removing
	list_remove(myroot,myminnode);
	if(VERBOSELIST==1){
		printf("after pop min by value2\n");
		list_stamp(myroot);
	}

	return (unsigned char)1;
}

unsigned char list_pop_first_by_value2(list* myroot, list_type** dataptr, list_type2** data2ptr,list_type3** data3ptr, list_type2 value2){
	if(VERBOSELIST==1){
		printf("before pop min by value2\n");
		list_stamp(myroot);
	}
	node *mynode = myroot->head;
	*dataptr = NULL;
	*data2ptr = NULL;
	*data3ptr = NULL;
//	dataptr = dataptr;
//	data2ptr = data2ptr;
	node *myminnode = NULL;
	if(mynode!=NULL){
		for(;;){
			if(myminnode==NULL)
				myminnode=mynode;
			else{
				if (mynode->dataptr!=NULL && mynode->data2ptr!=NULL && myminnode->dataptr!=NULL && myminnode->data2ptr!=NULL){
					if(((*(list_type2*)mynode->data2ptr )==value2)){
					myminnode=mynode;
					break;
					}
				}
				if(mynode->next!=NULL){
					mynode=mynode->next;
				}
				else
					break;
			}
		}
	}	
	*dataptr = myminnode->dataptr;
	*data2ptr = myminnode->data2ptr;
	*data3ptr = myminnode->data3ptr;
	if((*(list_type2*)myminnode->data2ptr)!=value2)
		return	(unsigned char)0;
	// min node removing
	list_remove(myroot,myminnode);
	if(VERBOSELIST==1){
		printf("after pop min by value2\n");
		list_stamp(myroot);
	}

	return (unsigned char)1;
}
*/
/*
unsigned char list_pop_Nth_by_value2(list* myroot, list_type** dataptr, list_type2** data2ptr,list_type3** data3ptr, list_type2 value2, long n){
	if(VERBOSELIST==1){
		printf("before pop Nth by value2\n");
		list_stamp(myroot);
	}
	node *mynode = myroot->head;
	*dataptr = NULL;
	*data2ptr = NULL;
	*data3ptr = NULL;
//	dataptr = dataptr;
//	data2ptr = data2ptr;
	node *myminnode = NULL;
	if(mynode!=NULL){
		for(;;){
			if(myminnode==NULL)
				myminnode=mynode;
			else{
				if (mynode->dataptr!=NULL && mynode->data2ptr!=NULL && myminnode->dataptr!=NULL && myminnode->data2ptr!=NULL){
					if(((*(list_type2*)mynode->data2ptr )==value2)){
						myminnode=mynode;
						if(n>1){
							n--;
						}
						else{
							break;
						}
					}
				}
				if(mynode->next!=NULL){
					mynode=mynode->next;
				}
				else
					break;
			}
		}
	}	
	*dataptr = myminnode->dataptr;
	*data2ptr = myminnode->data2ptr;
	*data3ptr = myminnode->data3ptr;
	if((*(list_type2*)myminnode->data2ptr)!=value2)
		return	(unsigned char)0; //errore
	// node removing
	list_remove(myroot,myminnode);
	if(VERBOSELIST==1){
		printf("after pop min by value2\n");
		list_stamp(myroot);
	}

	return (unsigned char)1;
}
*/

unsigned char list_pop_max_by_value2(list* myroot, list_type** dataptr, list_type2** data2ptr,list_type3** data3ptr,list_type4** data4ptr, list_type2 value2){
	if(VERBOSELIST==1){
		printf("before pop min by value2\n");
		list_stamp(myroot);
	}
	node *mynode = myroot->head;
	*dataptr = NULL;
	*data2ptr = NULL;
	*data3ptr = NULL;
	*data4ptr = NULL;
//	dataptr = dataptr;
//	data2ptr = data2ptr;
	node *mymaxnode = NULL;
	if(mynode!=NULL){
		for(;;){
			if(mymaxnode==NULL)
				mymaxnode=mynode;
			else{
				if (mynode->dataptr!=NULL && mynode->data2ptr!=NULL && mynode->data3ptr!=NULL && mymaxnode->dataptr!=NULL && mymaxnode->data2ptr!=NULL && mymaxnode->data3ptr!=NULL){
					if(((*(list_type*)mymaxnode->data3ptr < *(list_type*)mynode->data3ptr) && ((*(list_type2*)mynode->data2ptr )==value2)) || (((*(list_type2*)mymaxnode->data2ptr)!=value2)&&((*(list_type2*)mynode->data2ptr)==value2))){
					mymaxnode=mynode;
					}
				}
				if(mynode->next!=NULL){
					mynode=mynode->next;
				}
				else
					break;
			}
		}
	}	
	*dataptr = mymaxnode->dataptr;
	*data2ptr = mymaxnode->data2ptr;
	*data3ptr = mymaxnode->data3ptr;
	*data4ptr = mymaxnode->data4ptr;
	if((*(list_type2*)mymaxnode->data2ptr)!=value2)
		return	(unsigned char)0;
	// min node removing
	list_remove(myroot,mymaxnode);
	if(VERBOSELIST==1){
		printf("after pop min by value2\n");
		list_stamp(myroot);
	}

	return (unsigned char)1;
}


/*
unsigned char checklist(list * myroot, int n){
	node *mynode = &myroot->array[n];
	if(mynode->dataptr==NULL || mynode->data2ptr==NULL)
		return 0;
	return 1;
}

void* list_getvalueptr(list *myroot){
	return 	myroot->head->dataptr;
}


void* list_popvalue2ptr(list *myroot){
	void* ptr = myroot->head->data2ptr;
	list_pop(myroot);
	return ptr;
}

void list_pop(list * myroot)
{
	if(VERBOSELIST==1){
		printf("prima pop\n");
		list_stamp(myroot);
	}
	node *mynode = myroot->head;
	if(mynode->next==NULL)
		myroot->tail=NULL;
	else
		mynode->next->prev = NULL;
	mynode->dataptr = NULL;
	mynode->data2ptr = NULL;
	mynode->prev = NULL;
	myroot->head = mynode->next;
	mynode->next = NULL;
	if(myroot->head!=NULL && myroot->head->dataptr==NULL){
		printf("errore rilevato in pop\n");
	}
	if(VERBOSELIST==1){
	printf("dopo pop\n");
	list_stamp(myroot);
	}

}
*/
unsigned char list_empty(list *myroot){
	if(myroot->head==NULL){
		return (unsigned char)1;
	}
		return (unsigned char)0;
}

void list_stamp(list *myroot){
	if(myroot->head!=NULL){
		node* mynode=myroot->head;
		for(;;){
			printf("head:%p,tail:%p,data1:%ju,data2:%ju,data3:%ju,next:%p,prev:%p\n",myroot->head,myroot->tail,(uintmax_t)*(list_type*)mynode->dataptr,(uintmax_t)*(list_type2*)mynode->data2ptr,(uintmax_t)*(list_type3*)mynode->data3ptr,mynode->next,mynode->prev);
			if(mynode->next!=NULL){
				mynode=mynode->next;
			}
			else
				break;
		}
	}
}

