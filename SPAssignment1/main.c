#include "MathIsFun.c"

int main(){
	int choice, x,n,d;
	printf("Welcome to Math Is Fun - beta version\n"
			"Supported operations are:\n"
			"1 - Power calculation\n"
			"2 - Prime test\n"
			"3 - Palindrome test\n"
			"Please enter operation number (1/2/3):\n");
	scanf("%i", &choice);
	switch(choice){
	case 1:
		printf("Please enter three space separated numbers:\n");
		scanf("%i %i %i", &x,&n,&d);
		int res = funPow(x, n, d);
		printf("res = %d\n", res);
		break;
	case 2:
		printf("Please enter an integer:\n");
		scanf("%i", &x);
		if (funPrimeCheck(x)){
			printf("res = true\n");
		}else{
			printf("res = false\n");
		}
		break;
	case 3:
		printf("Please enter an integer:\n");
		scanf("%i", &x);
		if (funPalindromeCheck(x)){
			printf("res = true\n");
		}else{
			printf("res = false\n");
		}
		break;
	}


}
