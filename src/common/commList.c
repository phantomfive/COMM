/*****************************************************************************
 * Copyright 2013 Andrew Thompson Usable under the terms of the GPL.         *
 *****************************************************************************/

#include <notrap/notrap.h>
#include "commList.h"

typedef struct Link_struct {
	struct Link_struct *next;
	struct Link_struct *prev;
	void *obj;
} Link;

struct COMM_List_struct {
	int maxSize;

	int currentSize;
	
	int lastIndex;
	Link *last;

	Link *end;
	Link *start;
};


//-------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------
//Duplicated functionality in a couple other methods.
static Link *findLink(COMM_List *list, int index) {
	Link *l;
	int i;

	//a small optimization: when they access on object, we keep a
	//pointer to it. Then if they access it again, we can just return
	//the same one.
	
	//Here if they access the next one in the list, then we can just
	//go to the next one
	if(index==list->lastIndex+1) {
		list->last = list->last->next;
		list->lastIndex++;
		return list->last;
	}

	
	//if it's the same one they accessed last time, return that
	if(index==list->lastIndex) {
		return list->last;
	}


	//otherwise, start at the beginning and find the index
	//they are looking for
	l = list->start;
	i=0;
	while(index>i) {
		l = l->next;
		i++;
	}

	//keep track of the index for next time
	list->last = l;
	list->lastIndex = i;
	return l;
}



//-------------------------------------------------------------------------
// Public API
//-------------------------------------------------------------------------


COMM_List *allocCOMM_List(int maxSize) {
	COMM_List *rv = (COMM_List*)NTPmalloc(sizeof(COMM_List));
	if(rv==NULL) return NULL;

	rv->maxSize = maxSize;
	rv->end   = NULL;
	rv->start = NULL;
	rv->currentSize = 0;
	rv->lastIndex=-4;
	rv->last = NULL;

	return rv;
}
	

void freeCOMM_List(COMM_List **list) {
	if(list==NULL || *list==NULL) return;

	NTPfree(*list);
	*list = NULL;
}

BOOL COMM_ListPushBack(COMM_List *list, void *obj) {
	if(list->maxSize>0 && list->currentSize>=list->maxSize)
		return FALSE;

	Link *l = (Link*)NTPmalloc(sizeof(Link));
	if(l==NULL) return FALSE;

	l->obj = obj;
	list->currentSize++;
	if(list->end==NULL) {
		list->end   = l;
		list->start = l;
		l->next  = NULL;
		l->prev  = NULL;
		return TRUE;
	}

	l->next = NULL;
	l->prev = list->end;
	list->end->next = l;
	list->end = l;

	list->last=NULL;
	list->lastIndex = -4;
	return TRUE;
}

BOOL COMM_ListObjectAtIndex(COMM_List *list, void **obj, int index) {
	Link *l;
	if(index>=list->currentSize || index<0) 
	{
		*obj = NULL;
		return FALSE;
	}
	
	//find the link
	l = findLink(list, index);
	*obj = l->obj;

	return TRUE;
}


BOOL COMM_ListRemoveAtIndex(COMM_List *list, void **obj, int index) {
	Link *l;
	if(index>=list->currentSize || index<0) return FALSE;

	//find the link
	l = findLink(list, index);
	if(obj!=NULL) *obj = l->obj;

	//remove it
	if(l->prev!=NULL)  l->prev->next = l->next;
	if(l->next!=NULL)  l->next->prev = l->prev;
	if(list->start==l) list->start   = l->next;
	if(list->end==l)   list->end     = l->prev;

	//adjust our saved index
	if(index < list->lastIndex) {
		list->lastIndex--;
	}
	else if(index == list->lastIndex) {
		list->lastIndex--;
		list->last = l->prev;
		if(list->last==NULL) list->lastIndex = -4;
	}

	list->currentSize--;

	//we removed it, now free it
	NTPfree(l);
	return TRUE;
}

int COMM_ListSize(COMM_List *list) {
	return list->currentSize;
}



