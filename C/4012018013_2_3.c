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
    }
  return i+1;
}
void print(int *a,int count1)
{
  int i;
  for(i=0;i<count1;i++) printf("%d ",a[i]);
  printf("\n");
}
int summ(int *a,int *b,int *c,int count1,int count2)
{
  int n;
  if(count1>count2) n=count2;else n=count1;
  int i;
  for(i=0;i<n;i++)
    {
      c[i]=a[i]^b[i];      
    };
  if(count1>count2)
    {
      for(;i<count1;i++)
       {c[i]=a[i];}
      return i;
    }
  else
    {
      for(;i<count2;i++)
      {c[i]=b[i];}
      return i;
    }
}
int timem(int *a,int *b,int *d,int count1,int count2)
{
  int i,j;
  for(i=0;i<count1+count2;i++) d[i]=0;
  for(i=0;i<count1;i++)
    {
      for(j=0;j<count2;j++)
	{
	  d[i+j]=(a[i]*b[j])^d[i+j];
	}	
    }
  return count1+count2-1;
}
int Mod(int *a,int *c,int *d,int count1,int count3)
{
  int i;
  int *q,*p;int length=count1;
  int *l;
  int n=count1-count3;
  q=(int *)malloc(count1*sizeof(int));
  l=(int *)malloc((count1+count3)*sizeof(int));
  p=(int *)malloc(count1*sizeof(int));
  for(i=0;i<count1;i++) q[i]=a[i]; 
  while(length>=count3)
    {
      for(i=0;i<count1;i++) p[i]=0;
      for(i=0;i<count1;i++) l[i]=0;
      p[length-count3]=1;
      timem(p,c,l,count1,count3);
      for(i=0;i<count1;i++) q[i]=q[i]^l[i];
      for(i=count1-1;i>0;i--) if(q[i]==1) {length=i+1;break;}
    }
  for(i=0;i<length;i++) d[i]=q[i];  
  return length;
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
  int *a,*b,*c;
  a=(int *)malloc(datalen*sizeof(int));
  b=(int *)malloc(datalen*sizeof(int));
  c=(int *)malloc(datalen*sizeof(int));
  int count1,count2,count3;
  count1=input(a,f);
  count2=input(b,f);
  count3=input(c,f);
  int *sum,*time,*Mosum,*Motime;int lensum,lentime,lenmosum,lenmotime;
  sum=(int *)malloc((count1+count2)*sizeof(int));
  time=(int *)malloc((count1+count2)*sizeof(int));
  Mosum=(int *)malloc(count3*sizeof(int));
  Motime=(int *)malloc(count3*sizeof(int));
  lensum=summ(a,b,sum,count1,count2);print(sum,lensum);
  lentime=timem(a,b,time,count1,count2);print(time,lentime);
  lenmosum=Mod(sum,c,Mosum,lensum,count3);print(Mosum,lenmosum);
  lenmotime=Mod(time,c,Motime,lentime,count3);print(Motime,lenmotime);
  return 0;
}
