#include<stdio.h>
int main(){
    int  __attribute__((annotate("sensitive"))) flag; // tainted
    int  __attribute__((annotate("sensitive"))) x; // tainted
    int y, z;
    z = x;
    if(flag)
	  y = 10;
    else
	  y = 100;
    printf("%d",y);
}

