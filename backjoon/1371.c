#include <stdio.h>
#include <stdlib.h>

void initArray(int arr[], int len, int val)
{
	for (int i = 0; i < len; i++) {
		arr[i] = val;
	}
}
	

void findMax(int alpha[], int len)
{
	int res[26];
	int max = alpha[0];
	int idx = 0;

	initArray(res, len, -1);

	for (int i = 0; i < len; i++) 
	{
		if (max < alpha[i]) {
			max = alpha[i];
			res[0] = i;
			idx = 1;
		}
		else if (max == alpha[i] && alpha[i] != 0) 
		{
			res[idx++] = i;
		}
	}

	for (int i = 0; i < len; i++) {
		if (res[i] < 0)
			break;
		else
			printf("%c", res[i] + 'a');
	}
}

int main(void)
{
	int alpha[26] = {0, };
	char ch;
	int len;

	len = sizeof(alpha)/sizeof(alpha[0]);
	initArray(alpha, len, 0);

	while ((ch = getchar()) != EOF) {
		if (ch >= 'a' && ch <= 'z')
			alpha[ch - 'a']++;
	}

	findMax(alpha, len);

	exit(0);
}
