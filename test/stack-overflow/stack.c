#include<stdlib.h>
#include<string.h>
int main(int argc,char* argv[]){
    char* __attribute__((annotate("sensitive"))) first;
    char* __attribute__((annotate("sensitive"))) second;
    first = malloc(100);
    second = malloc(12);
    if(argc>1){
        strcpy(first,argv[1]);
    }
    free(first);
    free(second);
    return 0;
}
