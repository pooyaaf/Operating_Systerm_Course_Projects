#include <stdio.h>

int main() {
  //printf("Hello World!");
  int d[3][2]={
  {3,5},{2,5},{2,1}
  };
  //printf("%d",d[4]);
  int *b = (int*)d;
  printf("%d\n",b[4]);
  return 0;
}