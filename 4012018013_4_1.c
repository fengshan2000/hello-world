//4012018013
//shanks:关键快速排序，辗转相除法求逆元，二分查找

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int I=-1;
int main(int argc,char *argv[])
{
    FILE *f;
    if((f=fopen(argv[1],"r"))==NULL)
    {
        printf("error\n");exit(0);
    }        
    
    int shanks(int p,int a,int b);
    int p,a,b;
    fscanf(f,"%d",&p);
    fscanf(f,"%d",&a);
    fscanf(f,"%d",&b);
    fclose(f);
    shanks(p,a,b);        
    return 0;    
}
int shanks(int p,int a,int b)
{
    void quick_sort(long long *v,int s,int n);      //对动态数组进行排序
    long long inverse(int a,int p);                 //利用辗转相除法求逆
    int search(long long a,long long *v,int m,int n,int i);//二分查找
    int i,j;
    int n=ceil(sqrt(p));
    long long *v;                               
    v=(long long *)malloc(n*sizeof(long long));
    v[0]=a;
      for(i=1;i<n;i++)      //对动态数组赋初值
    {
        v[i]=(v[i-1]*a)%p;        
    }   
    quick_sort(v,0,n-1);
    long long a_n,tem1,tem2;
    a_n=inverse(a,p);
    tem1=a_n;    
    for(i=0;i<n-1;i++)
    {
        a_n=(a_n*tem1)%p;                
    }
    tem1=b;
    for(i=0;i<n;i++)        //找到b^(a_n*I)的I
    {
        search(tem1,v,0,n-1,i);  
        if(I==i)
        {            
            break;
        }
        tem1=(tem1*a_n)%p;        
    }    
    tem2=a;
       for(i=1;i<n;i++)         //找到与b^(a_n*I)的相等的a^i的i
    {
        if(tem2==tem1)
        {               
            break;
        }
        tem2=(tem2*a)%p;
    }    
    printf("k=%d \n",i+I*n);
    return 0;
}
void quick_sort(long long *v,int s,int n)
{
    int quick(long long *v,int left,int right);    
    if(s!=n)
    {
        int f;
        f=quick(v,s,n);        
        if(f>s)            
            quick_sort(v,s,f-1);
        if(f<n)
            quick_sort(v,f+1,n);        
    }
}
int quick(long long *v,int left,int right)
{
    long long tem;
    tem=v[left];
    while(left<right)        
    {        
        while(tem<=v[right]&&left<right)                
            right--;     
        v[left]=v[right];
        while(tem>=v[left]&&left<right)
            left++;        
        v[right]=v[left];    
    }    
    v[left]=tem;    
    return left;
}
long long inverse(int a,int p)
{
    int *q,*r;
    q=(int *)malloc((a+1)*sizeof(int));
    r=(int *)malloc(a*sizeof(int));
    q[0]=p;q[1]=a;
    int i=2;
    do{
        q[i]=q[i-2]%q[i-1];
        r[i-2]=q[i-2]/q[i-1];
        i++;
    }while(q[i-1]!=0);
    int m=1,n=-r[i-4];    
    int j=i-3;int tem;
    for(;j>1;j--)
    {
        tem=n;
        n=-n*r[j-2]+m;
        m=tem;        
    }    
    return (n+p)%p;
}
int search(long long a,long long *v,int m,int n,int i)
{
    if(abs(n-m)==1)
    {        
        if(a==v[m]||a==v[n])                
        {
            I=i;                
        }        
        return 0;
    }
    int mid=(m+n)/2;       
    if(a>v[mid])
    {        
        search(a,v,mid+1,n,i);
    }
    if(a<v[mid])
    {        
        search(a,v,m,mid-1,i);
    }    
    return 0;
}

