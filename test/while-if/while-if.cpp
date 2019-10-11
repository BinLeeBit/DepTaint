int main () {
    int b, c, d, e, f, g, N = 100;
    int __attribute__((annotate("sensitive"))) i;
    while (i < N) {
        if (i % 2 == 0)
            b = 5;
        else
            c = b;
        i++;
        d = 20;
    }

    while(N--){
        if (i % 2 == 0)
            e = 5;
        else
            e = 10;
        if(N % 2 == 0){
            e = 15;
            f = i; 
            g = 0;
        }     
    }

}
