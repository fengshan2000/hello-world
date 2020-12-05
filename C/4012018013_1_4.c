
//对给定文件白哦是的用于位置换位的置换
#include<stdio.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
  FILE *f_1,*f_2,*f_3;
   if((f_1=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!");
      exit(0);
    }
    if((f_2=fopen(argv[2],"rb"))==NULL)
    {
      printf("error!");
      exit(0);
    }
     if((f_3=fopen(argv[3],"wb"))==NULL)
    {
      printf("error!");
      exit(0);
    }
     int datalen1,datalen2;
     fseek(f_1,0,2);datalen1=ftell(f_1);fseek(f_1,0,0);
     fseek(f_2,0,2);datalen2=ftell(f_2);fseek(f_2,0,0);
     printf("The length of data1 is %d\n",datalen1,datalen2);
     char *a,*b;
     int *f,*f2;
     a=(char *)malloc((datalen2+1)*sizeof(char));
     if(!a)
       {
	 printf("申请内存出错！\n");
	 exit(0);
       }
     int count=0;
     int i;int count1;
      f=(int *)malloc((count)*sizeof(int));
     for(i=0;i<datalen2;i++){fscanf(f_2,"%1x",&f[i]);if(f[i]<0) break;}
     count1=i;
     for(i=0;i<count1;i++) printf("%d ",f[i]);
     for(i=0;i<datalen1;i++){
       fscanf(f_1,"%1X",&count);
       fprintf(f_3,"%X",f[count]);
     }
  return 0;
}
