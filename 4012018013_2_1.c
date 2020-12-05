//4012018013
//转化为以字符“0”和“1”表示的序列文本文件
//二元域上模下运算，系数数组
#include<stdio.h>
#include<stdlib.h>
int input(int *a,FILE *f)
{
  char ch[3];
  int i;
  for(i=0;;)//获取多项式的系数
    {
      fscanf(f,"%1d",&a[i]);
      fgets(ch,2,f);
      fseek(f,-1L,1);
      if(ch[0]==13)
	{
	  break;
	}
      i++;
    }printf("\n");
  return i+1;
}
int summ(int *a,int *b,int *c,int count1,int count2)
{
  int n;
  if(count1>count2) n=count2;else n=count1;
  int i;
  printf("sum=");
  for(i=0;i<n;i++)
    {
      c[i]=a[i]^b[i];      
    };
  if(count1>count2)
    {
      for(;i<count1;i++)
       {c[i]=a[i];}
      for(i=0;i<count1;i++) printf("%d",c[i]);
      return i;
    }
  else
    {
      for(;i<count2;i++)
      {c[i]=b[i];}
      for(i=0;i<count2;i++) printf("%d",c[i]);
      return i;
    }
}
int timem(int *a,int *b,int *d,int count1,int count2)
{
  printf("\ntimes=");
  int i,j;
  for(i=0;i<count1+count2;i++) d[i]=0;
  for(i=0;i<count1;i++)
    {
      for(j=0;j<count2;j++)
	{
	  d[i+j]=(a[i]*b[j])^d[i+j];
	}	
    }
  for(i=0;i<count1+count2-1;i++) printf("%d",d[i]);
  return 0;
}
int main(int argc,char *argv[])
{
  FILE *f;
  if((f=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!");
      exit(0);
    }
  int i;
  int datalen;
  fseek(f,0,2);datalen=ftell(f);fseek(f,0,0);
  printf("%d\n",datalen);
  int *a,*b;
  a=(int *)malloc(datalen*sizeof(int));
  b=(int *)malloc(datalen*sizeof(int));
  int count1,count2,count3;
  count1=input(a,f);
  count2=input(b,f);
  int *sum,*time;int min;
  sum=(int *)malloc((count1+count2)*sizeof(int));
  time=(int *)malloc((count1+count2)*sizeof(int));
  summ(a,b,sum,count1,count2);
  timem(a,b,time,count1,count2);
  return 0;
}
