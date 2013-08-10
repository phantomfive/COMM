#include <stdio.h>
#include <CuTest.h>
#include "../src/common/commList.h"  //The unit we are testing

//Checks the values on the list. nums is an array of numbers that
//will be compared against the list. size is the size of nums.
//startIndex can be used to change the starting point of the list,
//but still start from 0 in nums
static void verifyListItems(CuTest *tc, char *testName, COMM_List *list, int size,
                            int*nums, int startIndex) {
	int i;
	int64_t out=0;
	char s[2000];
	for(i=startIndex;i<(startIndex+size);i++) {
		BOOL result = COMM_ListObjectAtIndex(list, (void**)&out, i);
		sprintf(s, "%s: %d: result %d, expected %d, actual %lld",
		           testName, i, result, nums[i-startIndex], out);
		CuAssert(tc, s, result && nums[i-startIndex]==out);
	}
}

static void testInsertRemove(CuTest *tc) {
	int i;
	int size;
	char s[1000];
	int64_t out;
	BOOL result;
	COMM_List *list = allocCOMM_List(7);
	CuAssert(tc, "alloc fail", list!=NULL);

	//insert a bunch
	for(i=0;i<7;i++) {
		void *in = (void*)(int64_t)(i*2);
		result = COMM_ListPushBack(list, in);
		sprintf(s, "%d: result %d, size %d",i, result, COMM_ListSize(list));
		CuAssert(tc,s, result && COMM_ListSize(list)==(i+1));
	}

	//make sure they're on the list correctly
	for(i=0;i<7;i++) {
		result = COMM_ListObjectAtIndex(list, (void**)&out, i);
		size = COMM_ListSize(list);
		sprintf(s, "%d: result %d, size %d, obj %lld", i, result, size, out);
		CuAssert(tc, s, result && out==(i*2) && size==7);
	}

	//make sure they're on the list correctly in reverse
	for(i=6;i>=0;i--) {
		result = COMM_ListObjectAtIndex(list, (void**)&out, i);
		size = COMM_ListSize(list);
		sprintf(s, "%d: result %d, size %d, obj %lld", i, result, size, out);
		CuAssert(tc, s, result && out==(i*2) && size==7);
	}

	//make sure we can access some randomly
	result = COMM_ListObjectAtIndex(list, (void**)&out, 4);
	CuAssert(tc, "result success", result);
	CuAssert(tc, "obj should be 8", out==8);
	result = COMM_ListObjectAtIndex(list, (void**)&out, 1);
	CuAssert(tc, "result should be success 2", result);
	CuAssert(tc, "obj should be 2", out==2);
	
	//test boundary conditions
	result = COMM_ListObjectAtIndex(list, (void**)&out, -1);
	CuAssert(tc, "underflow", result==FALSE);
	result = COMM_ListObjectAtIndex(list, (void**)&out, 7);
	CuAssert(tc, "overflow", result==FALSE);
	result = COMM_ListRemoveAtIndex(list, (void**)&out, -1);
	CuAssert(tc, "underflow 2", result==FALSE);
	result = COMM_ListRemoveAtIndex(list, (void**)&out, 7);
	CuAssert(tc, "overflow 2", result==FALSE);

	//remove from the back
	result = COMM_ListRemoveAtIndex(list, (void**)&out, 6);
	size = COMM_ListSize(list);
	sprintf(s, "%d: result %d, size %d, obj %lld", 6, result, size, out);
	CuAssert(tc, s, result && size==6 && out==12);
	{
		int nums[] = {0, 2, 4, 6, 8, 10};
		verifyListItems(tc, "Remove from back", list, 6, nums, 0);
	}

	//remove from the front
	result = COMM_ListRemoveAtIndex(list, (void**)&out, 0);
	size = COMM_ListSize(list);
	sprintf(s, "%d: result %d, size %d, obj %lld", 6, result, size, out);
	CuAssert(tc, s, result && size==5 && out==0);
	{
		int nums[] = {2, 4, 6, 8, 10};
		verifyListItems(tc, "remove from front", list, 5, nums, 0);
	}
	
	//remove the middle
	result = COMM_ListRemoveAtIndex(list, (void**)&out, 2);
	size = COMM_ListSize(list);
	sprintf(s, "%d: result %d, size %d, obj %lld", 2, result, size, out);
	CuAssert(tc, s, result && size==4 && out==6);
	{
		int nums[] = {2, 4, 8, 10};
		verifyListItems(tc, "remove from middle", list, 4, nums, 0);
	}

	//remove the rest
	for(i=0;i<4;i++) {
		int nums[] = {2, 4, 8, 10};
		result = COMM_ListRemoveAtIndex(list, (void**)&out, 0);
		size = COMM_ListSize(list);
		sprintf(s, "%d: result %d, size %d, obj %lld", i, result, size, out);
		CuAssert(tc, s, result && size==(3-i) && out==nums[i]);
		verifyListItems(tc, "remove the rest", list, 3-i, &nums[i+1], 0);
	}

	//make sure it's empty
	CuAssert(tc, "list empty", COMM_ListSize(list)==0);

	//free it
	freeCOMM_List(&list);
	CuAssert(tc, "list NULL and freed", list==NULL);
}


static void testLastIndexOptimization(CuTest *tc) {
	int i;
	int size;
	char s[1000];
	int64_t out;
	BOOL result;
	COMM_List *list = allocCOMM_List(7);
	CuAssert(tc, "alloc fail", list!=NULL);

	//insert a bunch
	for(i=0;i<7;i++) {
		void *in = (void*)(int64_t)(i*2);
		result = COMM_ListPushBack(list, in);
		sprintf(s, "%d: result %d, size %d",i, result, COMM_ListSize(list));
		CuAssert(tc,s, result && COMM_ListSize(list)==(i+1));
	}
	
	//remove the object that is being pointed to
	result = COMM_ListObjectAtIndex(list, (void**)&out, 3);
	CuAssert(tc, "get object", result && out==6);
	result = COMM_ListRemoveAtIndex(list, (void**)&out, 3);
	size = COMM_ListSize(list);
	CuAssert(tc, "remove object", result && out==6 && size==6);
	result = COMM_ListObjectAtIndex(list, (void**)&out, 2);
	CuAssert(tc, "get object 2", result && out==4);
	{
		int nums[] = {0, 2, 4, 8, 10, 12};	
		verifyListItems(tc, "remove pointed to", list, 6, nums, 0);
	}

	//remove the object before the one being pointed to
	result = COMM_ListObjectAtIndex(list, (void**)&out, 4);
	CuAssert(tc, "get object", result && out==10);
	result = COMM_ListRemoveAtIndex(list, (void**)&out,1);
	size = COMM_ListSize(list);
	CuAssert(tc, "remove object", result && out==2 && size==5);
	result = COMM_ListObjectAtIndex(list, (void**)&out, 3);
	CuAssert(tc, "get object again", result && out==10);
	{
		int nums[] = {0, 4, 8, 10, 12};
		verifyListItems(tc, "remove object before", list, 5, nums, 0);
	}

	//remove thoe one being pointed at 0
	result = COMM_ListObjectAtIndex(list, (void**)&out, 0);
	CuAssert(tc, "get object", result && out==0);
	result = COMM_ListRemoveAtIndex(list, (void**)&out, 0);
	size=COMM_ListSize(list);
	CuAssert(tc, "remove oject", result && out==0 && size==4);
	result = COMM_ListObjectAtIndex(list, (void**)&out, 0);
	CuAssert(tc, "get object again", result && out==4);
	{
		int nums[] = {4, 8, 10, 12};
		verifyListItems(tc, "remove pointed at 0", list, 4, nums, 0);
	}

	//remove the rest
	for(i=0;i<4;i++) {
		int nums[] = {4, 8, 10, 12};
		result = COMM_ListRemoveAtIndex(list, (void**)&out, 0);
		CuAssert(tc, "removing", result && out==nums[i]);
		CuAssert(tc, "size", COMM_ListSize(list)==3-i);
	}

	freeCOMM_List(&list);
	CuAssert(tc, "ptr is null", list==NULL);
}


CuSuite *getListSuite(void) {
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testInsertRemove);
	SUITE_ADD_TEST(suite, testLastIndexOptimization);
	return suite;
}

