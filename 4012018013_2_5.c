//4012018013
//有理数域上的加法和乘法
#include<stdio.h> 
#include<stdlib.h>

int main(int argc,char *argv[])
{
    FILE *f;
    if((f=fopen(argv[1],"r"))==NULL)
    {
        printf("error!");exit(0);
    }
    int datalen;
    fseek(f,0,2);datalen=ftell(f);fseek(f,0,0);   
    int input(int *a,FILE *f);
    void print(int *a,int count1);
    int summ(int *a,int *b,int *c,int count1,int count2);
    int timem(int *a,int *b,int *d,int count1,int count2);
    int *a;int *b;int count1,count2;
    a=(int *)malloc(datalen*sizeof(int));
    b=(int *)malloc(datalen*sizeof(int));    
    count1=input(a,f);
    count2=input(b,f);
    int *sum;int lensum;
    sum=(int *)malloc((count1+count2)*sizeof(int));
    lensum=summ(a,b,sum,count1,count2);print(sum,lensum);
    int *time,lentime;
    time=(int *)malloc((count1+count2)*sizeof(int));
    lentime=timem(a,b,time,count1,count2);print(time,lentime);
    return 0;
}

int input(int *a,FILE *f)
{
  char ch[3];
  int i;
  for(i=0;;)//获取多项式的系数
    {
      fscanf(f,"%d",&a[i]);
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
      c[i]=a[i]+b[i];      
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
	  d[i+j]=(a[i]*b[j])+d[i+j];
	}	
    } 
  return count1+count2-1;
}
