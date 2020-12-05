
//对给定文件白用于位置换位的置换
#include<stdio.h>
#include<stdlib.h>
int input(int *a,FILE *f,int datalen)
{
  char ch[3];
  int i;
  for(i=0;;)//获取多项式的系数
    {
      fscanf(f,"%d",&a[i]);
      fgets(ch,2,f);
      fseek(f,-1L,1);
      if(ch[0]==13||ftell(f)==datalen)
	{
	  break;
	}
      i++;
    }printf("\n");
  return i+1; 
}
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
     int *a,*b;
     a=(int *)malloc((datalen2+1)*sizeof(int));
     if(!a)
       {
	 printf("申请内存出错！\n");
	 exit(0);
       }         
     int count=0;
     count=input(a,f_2,datalen2);
     
     b=(int *)malloc((count)*sizeof(int));
      if(!b)
       {
	 printf("申请内存出错！\n");
	 exit(0);
       } 
      int num=datalen1/count;
      int j,i;
     for(i=0;i<num;i++)
       {
	 for(j=0;j<count;j++)
	   fscanf(f_1,"%1X",&b[j]);
	 for(j=0;j<count;j++)
	   {
	     fprintf(f_3,"%X",b[a[j]-1]);
	   }	   
       }
     num=datalen1%num;
     for(i=0;i<num;i++)
       {
	 fscanf(f_1,"%1X",&b[i]);
       }     
     for(j=0;j<num;j++)
	   {
	     fprintf(f_3,"%X",b[j]);
	   }
     fclose(f_1);fclose(f_3);
  return 0;
}
