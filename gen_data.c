#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SEED 21
#define NUM_JOBS 2000

int main() {
	srand(SEED);
	//time_t t;
	//srand((unsigned) time(&t));
	FILE *file;
	file = fopen("data.txt", "w");
	if (file == NULL) {
		printf("fopen err\n");
		exit(1);
	}

	fprintf(file, "%d\n", rand() % 32 + 1); // for mem mgmt, initializing page frames
	for (int i=0; i<NUM_JOBS; i++) {
		//fprintf(file, "%d %d %d\n", i, rand() % 3 + i, rand() % 10 + 2); // for cpu scheduling
		fprintf(file, "%d\n", rand() % 32 + 1); // for mem mgmt, generating page calls
	}

	fclose(file);
	return 0;
}
