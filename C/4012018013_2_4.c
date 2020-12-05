//4012018013
//2_4 二元域上模下运算，用非零系数次数数组表示多项式
#include<stdio.h>
#include<stdlib.h>
int input(int *a,FILE *f,int datalen) //获取多项式的系数到a指向的动态数组中,return 该多项式中有几个非零项
{
  char  ch[3];
  int i;
  for(i=0;;)
    {
      fscanf(f,"%d",&a[i]);
      fgets(ch,2,f);
      if(ch[0]==13||ftell(f)==datalen)
	break;
      i++;
    }
  return i+1;
}
void print(int *a,int count1)  //输出指针动态数组函数，count1为有效长度
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
	  i++;j++;
	  continue;
	}
      else if(a[i]<b[j])
	{
	  c[k]=a[i];
	  k++;i++;
	}
      else
	{
	  c[k]=b[j];
	  k++;j++;
	}
    }
  if(i<count1) for(;i<count1;i++) {c[k]=a[i];k++;}
  if(j<count2) for(;j<count2;j++) {c[k]=b[j];k++;}
  return k;
}
int time(int *a,int *b,int *d,int count1,int count2) //非零系数次数数组多项式乘法
{
  int *c,*e;int i,j;int k;
  c=(int *)malloc(count1*sizeof(int));
  e=(int *)malloc((count1*count2)*sizeof(int));
  for(j=0;j<count1;j++)
    {
      d[j]=b[0]+a[j];
      e[j]=d[j];
    }  
  k=count1;
  for(i=1;i<count2;i++)
    {
      for(j=0;j<count1;j++) {c[j]=b[i]+a[j];}
      k=sum(c,e,d,count1,k);
      for(j=0;j<k;j++) {e[j]=d[j];}     
    }
  free(c);free(e);
  return k;
}
int Mod(int *a,int *c,int *mo,int count1,int count3)
{
  int i;
  int *q,*p,*m,l;int length;
  q=(int *)malloc((count1+count3)*sizeof(int));
  for(i=0;i<count1+count3;i++) q[i]=0;
  p=(int *)malloc((count3)*sizeof(int));
  m=(int *)malloc((count1+count3)*sizeof(int));
  for(i=0;i<count1;i++) m[i]=a[i];
  length=a[count1-1];
  while(length>c[count3-1])
    {
      l=length-c[count3-1];
      for(i=0;i<count3;i++) p[i]=c[i]+l;       
      count1=sum(p,m,q,count3,count1);
      for(i=0;i<count1;i++) m[i]=q[i];
      length=q[count1-1];
    }
  for(i=0;i<count1;i++) mo[i]=q[i];
  free(q);free(p);free(m);
  return count1;
}
int main(int argc,char *argv[])
{
  FILE *f;
  if((f=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
  int datalen;
  fseek(f,0,2);datalen=ftell(f);fseek(f,0,0); 
  int *a,*b,*c;int count1,count2,count3;
  a=(int *)malloc(datalen*sizeof(int));
  b=(int *)malloc(datalen*sizeof(int));
  c=(int *)malloc(datalen*sizeof(int));
  count1=input(a,f,datalen);
  count2=input(b,f,datalen);
  count3=input(c,f,datalen);
  fclose(f);
  int *su,*tim,*mosum,*motime;
  int lensum,lentime,lenmosum,lenmotime;
  su=(int *)malloc((count1+count2)*sizeof(int));
  tim=(int *)malloc((count1*count2)*sizeof(int));
  lensum=sum(a,b,su,count1,count2);print(su,lensum);
  lentime=time(a,b,tim,count1,count2);print(tim,lentime);
  lenmosum=Mod(su,c,mosum,lensum,count3);print(mosum,lenmosum);
  lenmotime=Mod(tim,c,motime,lentime,count3);print(motime,lenmotime);
  free(a);free(b);free(c);free(su);free(tim);free(mosum);free(motime);
  return 0;  
}

