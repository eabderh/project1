#include <stdlib.h>

//contains 1 leak
int main(int argc, char **argv)
{
	void *p1, *p2, *p3, *p4;

	p1 = malloc(1345);
	p2 = malloc(2);
	free(p1);

	p1 = malloc(134);
	free(p2);

	p3 = malloc(1);
	p4 = malloc(981);
	free(p4);
	free(p3);
return 0;
}


