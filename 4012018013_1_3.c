
//转化为以字符“0”和“1”表示的序列文本文件

#include<stdio.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
  FILE *f_1,*f_2;
  if((f_1=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!");
      exit(0);
    }
  if((f_2=fopen(argv[2],"wb"))==NULL)
    {
      printf("error!");
      exit(0);
    }
  int datalen;
  fseek(f_1,0,2);datalen=ftell(f_1);fseek(f_1,0,2);
  printf("The length of input is %d \n",datalen);
  int i,j,bit[4];
  int a;
  fseek(f_1,-1L,2);
  for(i=0;i<datalen;i++){
    fscanf(f_1,"%1x",&a);
    fseek(f_1,-2l,1);
    for(j=0;j<4;j++)
      {
	bit[j]=(a>>(3-j))&1;
      }
    fprintf(f_2,"%d%d%d%d",bit[3],bit[2],bit[1],bit[0]);
  }
  fclose(f_1);
  fclose(f_2);
  return 0;
}
