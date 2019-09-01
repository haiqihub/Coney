#include "stdio.h"

int main(int argc, char * argv[])
{
	int i;
	for (i = 1; i < argc - 1; i++)
		printf("%s%s", i == 1 ? "" : " ", argv[i]);
	printf("\n");
	scanf("%d",&i);
	printf("i: %d",i);

	return 0;
}
