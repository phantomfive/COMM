#include <stdio.h>
#include <CuTest.h>

void testMessage(CuTest *tc) {
	CuAssertTrue(tc, 0);
}

CuSuite *getMessageSuite(void) {
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testMessage);

	return suite;
}
