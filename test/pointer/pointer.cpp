#include <stdio.h>
int main(){
    int    a = 10;   
    int __attribute__((annotate("tainted"))) *pa;
    int __attribute__((annotate("tainted"))) b;
    int *pb;
    int *pc;

    pa = &a;
    pb = &b;
    pc = pa;

    return 0;
}
