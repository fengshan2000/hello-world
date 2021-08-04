//4012018013_lfs
//给定信源的概率分布,task:1> shannon码 2> fano码  

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

//对相应文件中的信源概率分布进行输入，并校验信源的概率之
// 和是否为1，若为1则函数返回值为信源的消息个数，否则返回0

#define MAX 20
int c[MAX] = {0};

int I=0;

int main(int argc,char *argv[])
{
    int getpx(FILE *f,int datalen,float *px);
    int get_shannon_code(float *px,int dl,int d);
    void get_fano_code(float *px,int dl,int d);
    FILE *f;
    if((f=fopen(argv[1],"rb"))==NULL)
    {
      printf("your file's name error!\n");
      exit(0);
    }
    int dl,i,j;
    fseek(f,0,2);dl=ftell(f);fseek(f,0,0);         
    float *px;    
    px=(float*)malloc(sizeof(float)*dl);  
    if((dl=getpx(f,dl,px))==0) {
        printf("error input!");
        return 0;
    }    
    //向用户请求一个消息码长d
    int d;
    printf("\nYour D:");
    scanf("%d",&d);    
    get_shannon_code(px,dl,d);
    get_fano_code(px,dl,d);
    return 0;
}

int getpx(FILE *f,int datalen,float *px)
{
    printf("Your input px:\n");
    int i,count=0;
    double tem;
    char ch;
    
    for(i=0;i<datalen/2;i++){
       
        fscanf(f,"%f",&px[i]);
        printf("%f ",px[i]);        
        fscanf(f,"%ch",&ch);
        count=count+1;
        if(ftell(f)==datalen) 
            break;                
    }
    for(i=0;i<count;i++){
       tem=tem+px[i];
    }
    if(abs(1-tem)<10^(-7))
    {
        return count;
    }
    else
        return 0;
}

int get_shannon_code(float *px,int dl,int d)
{
    void quick_sort(float *v,int *flag,int s,int n);
    printf("Shannon_code:\n");
    
    int i,j;
    int item,count;
    int code_length[dl];
    float *pxx; 
    pxx=(float *)malloc(sizeof(float)*dl);
    int *flag;
    flag=(int *)malloc(sizeof(int)*dl);
    for(i=0;i<dl;i++)   { pxx[i]=px[i];flag[i]=i;}
    quick_sort(pxx,flag,0,dl-1);            
    for(i=0;i<dl;i++){        
        code_length[i]=ceil(-log2(pxx[i])/log2(d));          
    }        
    item=code_length[0];
    for(i=1;i<dl;i++){
        if(item<code_length[i])
            item=code_length[i];
    }
    //判断shannon码是否可以唯一可译
    count=0;
    for(i=0;i<dl;i++) count=count+pow(d,item-code_length[i]);    
    if(count<pow(d,item)) 
        printf("ok\n");
    else 
    {
        printf("error  \n");    
        return 0;
    }
    
    int istem[dl][item];
    for(i=0,count=0;i<pow(d,item)&&count<dl;i=i+pow(d,item-code_length[count]),count++){                        
        istem[flag[count]][0]=i;        
        for(j=0;j<item;j++){
            istem[flag[count]][j+1]=istem[flag[count]][j]/d;
            istem[flag[count]][j]=istem[flag[count]][j]%d;                         
        }                    
        }        
    for(i=0;i<dl;i++){code_length[i]=ceil(-log2(px[i])/log2(d));}            
    for(i=0;i<dl;i++){
        printf("X_%d:",i);
        for(j=0;j<code_length[i];j++)
            printf(" %d ",istem[i][item-j-1]);
        printf("\n");
    }
    count=0;
    for(i=0;i<dl;i++) count=count+code_length[i];
    printf("Shannon_code's Average code length:%f\n",(1.0)*count/dl);
    return 0;
}

void get_fano_code(float *px,int dl,int d)
{
    printf("\nFano_code:\n");
    void quick_sort(float *v,int *flag,int s,int n);
    void divide_pxx(float *pxx,int d,int start,int end,int *mat,int *flag,int time,int dl);      
    int i,j;
    float *pxx; 
    pxx=(float *)malloc(sizeof(float)*dl);
    int *flag;
    flag=(int *)malloc(sizeof(int)*dl);
    for(i=0;i<dl;i++)   { pxx[i]=px[i];flag[i]=i;}
    quick_sort(pxx,flag,0,dl-1);       
    //编码矩阵
    int *mat;
    mat=(int *)malloc(sizeof(int *)*(dl*dl));
    for(i=0;i<dl;i++) //对矩阵赋初值-1，对编码加以区分
        for(j=0;j<dl;j++)
            mat[i*dl+j]=-1;                                  
    divide_pxx(pxx,d,0,dl-1,mat,flag,0,dl);
    
    for(i=0;i<dl;i++){
        printf("X_%d:",i);
        for(j=0;j<dl;j++){
            if(mat[i*dl+j]!=-1)
                printf("%d ",mat[i*dl+j]);
            else
            {
                flag[i]=j;                
                break;
            }
        }        
        printf("\n");
    }
    i=0;
    for(j=0;j<dl;j++) i=i+flag[j];    
    pxx[0]=(1.0)*i/dl;
    printf("Fano_code's Average code length:%f",pxx[0]);
}

void divide_pxx(float *pxx,int d,int start,int end,int *mat,int *flag,int time,int dl)
{   
    void comp(int m,int M,int N,int* sit);
    int comb(int M,int N);
    float sum(float* pxx,int start,int end);
    void tran_sit_F(int i,int* sit,int* F,int d,int start,int end,float *pxx);
    
    int i,j;
    float tem,ave,ftem;  
    float d_sum[d];
    int F[d][2];    
    int dnum[d],dnum2[d];                    
    if((end-start+1)<=d)
        for(i=start;i<=end;i++)
        {
            mat[flag[i]*dl+time]=i-start;
        }    
    else
    {
        int *F;
        F=(int*)malloc(sizeof(int)*2*d);
        F[0]=start;F[2*d-1]=end;        
        int *sit; //存储所有分组的情况 ，每M个一组
        sit=(int*)malloc(sizeof(int)*(comb(end-start+1-1,d-1)*(d-1)));        
        I=0; //初始
        comp(1,d-1,end-start+1-1,sit); //得到dl-1个信源中分成d-1组的间隔选择位置                
        //对于comb(end-start,d-1)种分布情况，便利每一种，求得最小的平均差
        ftem=1;I=0;
        for(I=0,i=0;i<comb(end-start,d-1);i++)
        {
            //第一步 分割信源 第i种情况
            ave=sum(pxx,start,end)/d;
            tran_sit_F(i,sit,F,d,start,end,pxx);
            for(j=0;j<d;j++) d_sum[j]=sum(pxx,F[j*2+0],F[j*2+1]);
            // 做差加fabs 再求和
            for(tem=0,j=0;j<d;j++) tem=tem+fabs(d_sum[j]-ave);                        
            if(ftem>tem) {ftem=tem;I=i;}                                     
        }
        tran_sit_F(I,sit,F,d,start,end,pxx);             
        //对mat第time列进行赋值
        for(j=0;j<d;j++)
        {
            for(i=F[j*2+0];i<=F[j*2+1];i++)
                mat[flag[i]*dl+time]=j;
            if(F[j*2+1]-F[j*2+0]+1>1)
                divide_pxx(pxx,d,F[j*2+0],F[j*2+1],mat,flag,time+1,dl);
        }
        
    }
}

void tran_sit_F(int i,int* sit,int* F,int d,int start,int end,float *pxx)
{
    int j;
    int dnum[d];
    int dnum2[d];
    dnum[0]=sit[i*(d-1)];
    for(j=1;j<d-1;j++)
        dnum[j]=sit[i*(d-1)+j]-sit[i*(d-1)+j-1];
    dnum[d-1]=end-start+1-sit[i*(d-1)+d-2];
    dnum2[0]=start+dnum[0]-1;
    for(j=1;j<d;j++)
        dnum2[j]=dnum2[j-1]+dnum[j];
    for(j=0;j<d-1;j++){F[j*2+1]=dnum2[j];F[(j+1)*2+0]=dnum2[j]+1;}
}

float sum(float* pxx,int start,int end)
{
    int i;
    float sum=0;
    for(i=start;i<=end;i++)
    {
        sum=sum+pxx[i];
    }
    return sum;
}

void print(int M,int *sit)
{
    int i;
    for (i = 0; i < M; i++)
    {
        sit[I]=c[i+1];
        I++;
    }    
}

void comp(int m,int M,int N,int* sit)
{
    if (m == M + 1)
    {
        print(M,sit);    
    }
    else
    {
        for (c[m] = c[m - 1] + 1; c[m] <= N - M + m; c[m]++) 
        {
            comp(m + 1,M,N,sit);
        }
    }
}

int comb(int M,int N)
{
    int tem1=1,tem2=1,tem3=1;    
    int i;
    for(i=2;i<=M;i++) tem1=tem1*i; 
    for(i=2;i<=N;i++) tem2=tem2*i;
    for(i=2;i<=(M-N);i++) tem2=tem2*i;
    return tem1/(tem2*tem3);
}

//将以前编的快速排序拿过来直接使用
void quick_sort(float *v,int *flag,int s,int n)
{
    int quick(float *v,int *flag,int left,int right);  
    if(s!=n)
    {
        int f;
        f=quick(v,flag,s,n);        
        if(f>s)            
            quick_sort(v,flag,s,f-1);
        if(f<n)
            quick_sort(v,flag,f+1,n);        
    }
}
int quick(float *v,int *flag,int left,int right)
{
    float tem;int t;
    tem=v[left];t=flag[left];
    while(left<right)        
    {        
        while(tem>=v[right]&&left<right)                
            right--;     
        v[left]=v[right];
        flag[left]=flag[right];
        while(tem<=v[left]&&left<right)
            left++;        
        v[right]=v[left];
        flag[right]=flag[left];
    }    
    v[left]=tem;  
    flag[left]=t;
    return left;
}
