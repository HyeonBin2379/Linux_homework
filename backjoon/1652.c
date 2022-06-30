#include <stdio.h>
#include <stdlib.h>

void initArray(int (*arr)[100]) 
{
	int i, j;

	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++)
			arr[i][j] = -1;
	}
}
void printArray(int (*arr)[100], int size) 
{
	int i, j;

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++)
			printf("%d ", arr[i][j]);
		printf("\n");
	}
}

int getRow(int (*arr)[100], int size)
{
	int res = 0;
	int i, j, idx, last;
	int notNull[100][100];

	initArray(notNull);

	for (i = 0; i < size; i++) {
		idx = 0;
		last = 0;
		for (j = 0; j < size; j++) {
			if (arr[i][j] == 1) {
				notNull[i][idx] = j;
				if (notNull[i][idx] - last > 1) res++;
				last = notNull[i][idx];
				idx++;
			}
		}
	}

	printArray(notNull, size);
	return res;
}
int getCol(int (*arr)[100], int size)
{
	int res = 0;
	int col, i;
	int cnt;
	
	return res;
}

int main(void)
{
	int size;
	int i, j;
	char ch;
	int room[100][100];

	initArray(room);

	printf("정방형 방의 크기 입력: ");
	scanf("%d", &size);
	getchar();

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if ((ch = getchar()) == '.')
				room[i][j] = 0;
			else
				room[i][j] = 1;	
		}
		getchar();
	}
	printArray(room, size);

	printf("%d %d\n", getRow(room, size), getCol(room, size));
}
