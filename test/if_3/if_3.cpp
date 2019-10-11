#include<stdio.h>
int main ()
{

   int a = 100;
   int __attribute__((annotate("sensitive"))) b;
   int c=0,d=0,e=0,f=0,g=0,h=0;
   scanf("%d",&b);
 
   if( a == 100 )
   {
       if( b <= 200 )
       {
          c = 5;
          if( b + c > 200)
	      d = 0;
          else if( b + c > 100){
              d = 1;
              e = 20;
          }
          else
              d = 2;
       }
       else{
           f = 300;
       }
   }
   else{
       g = 100;
       h = b;
       
   }
 
   return 0;
}
