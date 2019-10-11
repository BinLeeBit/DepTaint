#include<stdio.h>
#include<stdlib.h>
int main(int argc,char **argv){
    void * __attribute__((annotate("sensitive"))) p1;
    void * __attribute__((annotate("sensitive"))) p2;
    void * __attribute__((annotate("sensitive"))) p3;

    p1=malloc(100);
    printf("malloc:%p\n",p1);
    p2=malloc(100);
    printf("malloc:%p\n",p2);
    p3=malloc(100);
    printf("malloc:%p\n",p3);

    printf("free p1\n");
    free(p1);
    printf("free p3\n");
    free(p3);
    printf("free p2\n");
    free(p2);

    printf("Double free\n");
    free(p2);
}
