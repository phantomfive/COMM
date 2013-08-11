#ifndef COMM_LIST_H
#define COMM_LIST_H

/*****************************************************************************
 * Perhaps the biggest deficiency in the C standard library is the lack      *
 * of a generic list implementation. This is an attempt to remedy that lack, *
 * if only for my own applications.                                          *
 *                                                                           *
 * Copyright 2013 Andrew Thompson Usable under the terms of the GPL.         *
 *****************************************************************************/

#include <notrap/notrap.h>

typedef struct COMM_List_struct COMM_List;


//methods to free and release the list. For an unlimited max size,
//pass in a negative number. freeCOMM_List() sets 'list' to NULL
COMM_List *allocCOMM_List(int maxSize);
void freeCOMM_List(COMM_List **list);


//Add an object to the end of the list, return TRUE or FALSe
BOOL COMM_ListPushBack(COMM_List *list, void *obj);

//Retrieve the object from the list and store it in *obj.
//There is an optimization for when each item is accessed one after another.
BOOL COMM_ListObjectAtIndex(COMM_List *list, void **obj, int index);

//Removes the object at index from the list. If obj is not NULL,
//stores the removed value in *obj.
BOOL COMM_ListRemoveAtIndex(COMM_List *list, void **obj, int index);

//Returns the current size of the list
int COMM_ListSize(COMM_List *list);


#endif

