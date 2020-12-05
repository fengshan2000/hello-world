// 4012018013
//有理数域多项式加法乘法，非零系数次数数组表示多项式
#include<stdio.h>
#include<stdlib.h>

int main(int argc,char * argv[])
{
    FILE *f;
    if((f=fopen(argv[1],"r"))==NULL)
    {
        printf("error");exit(0);
    }   
    int datalen;
    fseek(f,0,2);datalen=ftell(f);fseek(f,0,0); 
    int input(int *a,FILE *f);
    void print(int *a,int count1);
    int sum(int * a,int *b,int *c,int count1,int count2);
    int times(int *a,int *b,int *d,int count1,int count2);
    int *a,*b;int count1,count2;
    a=(int *)malloc(datalen*sizeof(int));
    b=(int *)malloc(datalen*sizeof(int));
    count1=input(a,f);
    count2=input(b,f);
    int *su,lensum,*time,lentime;
    su=(int *)malloc((count1+count2)*sizeof(int));
    lensum=sum(a,b,su,count1,count2);print(su,lensum);
    time=(int *)malloc((count1*count2)*sizeof(int));
    lentime=times(a,b,time,count1,count2);print(time,lentime);
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

int sum(int * a,int *b,int *c,int count1,int count2)     //（系数1，系数2，加法结果，系数1长度，系数2长度）return 加法长度
{
  
  int i=0,j=0,k=0;
  for(i=0,j=0;i<count1&&j<count2;)
    {
      
      if(a[i]==b[j])
	{
        c[k]=a[i];
        c[k+1]=a[i+1]+b[i+1];
        i+=2;j+=2;k+=2;
	  continue;
	}
      else if(a[i]<b[j])
	{
	  c[k]=a[i];
      c[k+1]=a[i+1];
	  k+=2;i+=2;
	}
      else
	{
	  c[k]=b[j];
      c[k+1]=b[j+1];
	  k+=2;j+=2;
	}
    }
  if(i<count1) for(;i<count1;i++) {c[k]=a[i];k++;}
  if(j<count2) for(;j<count2;j++) {c[k]=b[j];k++;}
  return k;
}
int times(int *a,int *b,int *d,int count1,int count2)
{

  int *c,*e;int i,j;int k;
  c=(int *)malloc(count1*sizeof(int));
  e=(int *)malloc((count1*count2)*sizeof(int));
  for(j=0;j<count1;j+=2) 
    {
      d[j]=b[0]+a[j];
      d[j+1]=b[1]*a[j+1];
      e[j]=d[j];e[j+1]=d[j+1];
    }       
  
  k=count1;
  for(i=2;i<count2;i+=2)
    {
      for(j=0;j<count1;j+=2) 
      {
          c[j]=b[i]+a[j];
          c[j+1]=b[i+1]*a[j+1];
      }
      k=sum(c,e,d,count1,k);
      for(j=0;j<k;j++) 
        e[j]=d[j];
    }
  return k;
}
