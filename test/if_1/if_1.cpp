int main(){

  int __attribute__((annotate("tainted"))) flag;
  int ch1,ch2,ch3;
  ch1 = flag + 2;
  if (flag){
    ch2 = 10;  
    ch3 = flag;        
  }
  else {
    ch2 = 11;		
  }                           

  return 0;
}
