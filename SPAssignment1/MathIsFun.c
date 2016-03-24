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
static int funSqrt(int x); // ==> Declaration is always in the beginning of the file.


int funPow(int x, int n, int d) {
	if (n==0){
		return 1;
	}
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
	if (0<=x && x<2){
		return 1;
	}
	else if (x<0){
		return -1;
	}
	else{
		int left = 0;
		int right = x;
		int mid = x/2;
		while (mid*mid!= x){
			if (mid*mid>x){
				right = mid;
				mid = (left+right)/2;
			}
			else{
				if (x<(mid+1)*(mid+1)){
					return mid;
				}
				else{
					left = mid;
					mid= (left+right)/2;
				}
			}
		}
		return mid;
	}


}

bool funPrimeCheck(int x) {
	int index;
	for (index=2; index< funSqrt(x)+1; index+=1){
		if (x%index ==0){
			return 0;
		}
	}
	return 1;
}

bool funPalindromeCheck(int x) {
	if (x<0){return 0;}
	int reverse = 0 ;
	int temp = x;
	while (temp !=0){
		reverse = reverse*10;
		reverse = reverse + (temp%10);
		temp/= 10;
	}
	return reverse == x;
}

