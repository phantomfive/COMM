#include <stdio.h>
#include <CuTest.h>

CuSuite *getMessageSuite();
CuSuite *getListSuite();

//returns 1 on failure, 0 on success (like unix command line)
int runAllTests(void) {
	CuString *output = CuStringNew();
	CuSuite *suite   = CuSuiteNew();

	CuSuiteAddSuite(suite, getMessageSuite());
	CuSuiteAddSuite(suite, getListSuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	if(suite->failCount!=0) {
		printf("There were failures\n");
		return 1;
	}
	return 0;
}

int main(void) {
	return runAllTests();
}
