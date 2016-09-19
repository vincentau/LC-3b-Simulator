/* Just some practice with pointers */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void editValueViaPtr(int *nPrt, int newVal);
void addValuesViaPrt(int n1, int n2, int **sumPtr);

int main(int argc, char *argv[]) {
	int n1 = 500;
	int n2 = 1000;
	int sum = 0;
	int *nPtr;

	nPtr = &n1;
	printf("%d", n1);
	editValueViaPtr(nPtr, 750);
	printf("%d", n1);
}

void editValueViaPtr(int *nPtr, int newVal) {
	*nPtr = newVal;
}
