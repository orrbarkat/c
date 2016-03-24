#include "MathIsFun.h"
#include<stdio.h>

/*
 * Calculates the largest integer less or equal than the square root of x.
 * funSqrt(10) = 3
 * funSqrt(16) = 4
 * @param x - An integer for which the function applies
 * @return the value of |_sqrt(x)_|, if x is negative then the function
 * returns -1.
 */
int funSqrt(int x); // ==> Declaration is always in the beginning of the file.

int funPow(int x, int n, int d) {
	if (n==1){
		int ret = x%d;
		if (ret<0){ret+= d;}
		return ret;
	}
	int temp;
	// n is even
	if (n%2 == 0){
		temp = funPow(x, n/2,d);
		return temp*temp%d;
	}else{
		int y = x%d;
		if (y<0){y+= d;}
		temp = funPow(x, (n-1)/2,d);
		return (y*temp*temp)%d;
	}
}

int funSqrt(int x) {
	//Your implementation
	//Declaration + Variabl initializing at the beginning of the function
}

bool funPrimeCheck(int x) {
	//Your implementation
	//Declaration + Variabl initializing at the beginning of the function
}

bool funPalindromeCheck(int x) {
	//Your implementation
	//Declaration + Variabl initializing at the beginning of the function
}

int main(){
	int r = funPow(-7,5,9);
//	printf("%d",(-40%3));
	printf("\n%d \n",r );
}
