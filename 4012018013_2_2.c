
//4012018013
#include<stdio.h>
#include<stdlib.h>
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
int times(int *a,int *b,int *d,int count1,int count2)
{

  int *c,*e;int i,j;int k;
  c=(int *)malloc(count1*sizeof(int));
  e=(int *)malloc((count1*count2)*sizeof(int));
  for(j=0;j<count1;j++) {d[j]=b[0]+a[j];e[j]=d[j];}
  
  k=count1;printf("\n%d ",k);
  for(i=1;i<count2;i++)
    {
      for(j=0;j<count1;j++) {c[j]=b[i]+a[j];}
      k=sum(c,e,d,count1,k);printf("%d ",k);
      for(j=0;j<k;j++) {e[j]=d[j];}     
    }
  return k;
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
  int *a,*b;
  a=(int *)malloc(datalen*sizeof(int));
  b=(int *)malloc(datalen*sizeof(int));
  char  ch[3];
  int i=0,count1=0,count2=0;
  for(i=0;;)//获取第一个多项式的系数
    {
      fscanf(f,"%d",&a[i]);
      fgets(ch,2,f);
      if(ch[0]==13)
	break;
      i++;
    }count1=i+1;
  for(i=0;;)//获取第二个多项式的系数
    {
      fscanf(f,"%d",&b[i]);
      fgets(ch,2,f);
      if(ch[0]==13)
	break;
      i++;
    }count2=i+1;
   int *c,*d;
  c=(int *)malloc((count1+count2)*sizeof(int));
  d=(int *)malloc((count1*count2)*sizeof(int));
  int k;
  k=sum(a,b,c,count1,count2);printf("sum=");
  for(i=0;i<k;i++) printf("%d ",c[i]);
  k=times(a,b,d,count1,count2);printf("\ntime=");
  for(i=0;i<k;i++) printf("%d ",d[i]);
  return 0;
  
}
