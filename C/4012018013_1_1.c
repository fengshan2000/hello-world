//将字符串按照相反顺序排列存入新文件！

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
  fseek(f_1,0,2);
  datalen=ftell(f_1);
  fseek(f_1,0,2);
  printf("The datalen is %d",datalen);
  int i;char  c;
  fseek(f_1,-1L,2);
  for(i=0;i<datalen;i++)
    {
      fscanf(f_1,"%c",&c);
      fprintf(f_2,"%c",c);
      fseek(f_1,-2L,1);
    }
  fclose(f_1);
  fclose(f_2);
  return 0;
}
