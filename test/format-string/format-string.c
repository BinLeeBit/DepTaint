#include <stdio.h>
int main(void)
{
    int __attribute__((annotate("sensitive"))) flag = 0;
    int __attribute__((annotate("sensitive"))) a = 0;
    while(a < 5){
        if (flag){
            printf("AAAAAAA%n\n",&a);
            flag = 0;
        }
        else{
            printf("BBBBBBB\n",&a);
            flag = 1;
        }
    }
    printf("%d\n",a);
    return 0;
}
