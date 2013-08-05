#ifndef COMM_LIST_H
#define COMM_LIST_H

/*****************************************************************************
 * Perhaps the biggest deficiency in the C standard library is the lack      *
 * of a generic list implementation. This is an attempt to remedy that lack, *
 * of only for my own applications.                                          *
 *                                                                           *
 * Copyright 2013 Andrew Thompson Usable under the terms of the GPL.         *
 *****************************************************************************/

#include <notrap/notrap.h>

typedef struct COMM_List_struct COMM_List;

COMM_List *allocCOMM_List(int maxSize);
void freeCOMM_List();

BOOL COMM_ListPushBack(COMM_List *list, void *obj);

BOOL COMM_ListObjectAtIndex(COMM_List *list, void **obj, int index);

BOOL COMM_ListRemoveAtIndex(COMM_List *list, void **obj, int index);

int COMM_ListSize(COMM_List *list);


#endif

