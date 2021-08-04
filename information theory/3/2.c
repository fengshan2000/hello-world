//4012018013_lfs
//给定信源的概率分布,task: huffman code

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
    void huffman_code(float *px,int dl,int d);
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
    huffman_code(px,dl,d);
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

void huffman_code(float *px,int dl,int d)
{
    printf("Huffman_code:\n");
    void quick_sort(float *v,int *flag,int s,int n,int dl);
    int i,j,I;    
    int px_len,code_len;
    float *pxx;
    pxx=(float*)malloc(sizeof(float)*dl);
    int *flag;
    flag=(int *)malloc(sizeof(int)*dl*dl);
    int *mat;
    mat=(int *)malloc(sizeof(int)*dl*dl);
    for(i=0;i<dl;i++) 
        for(j=0;j<dl;j++){
            flag[i*dl+j]=0;mat[i*dl+j]=-1;
        }
    for(i=0;i<dl;i++) {pxx[i]=px[i];flag[i*dl+i]=1;}    
    quick_sort(pxx,flag,0,dl-1,dl);    
        
    int l=2+(dl-2)%(d-1);   
    void first_huffman(int dl,int *flag,float *pxx,int l,int *mat,int time);
    void other_huffman(int dl,int *flag,float *pxx,int d,int *mat,int time);
    first_huffman(dl,flag,pxx,l,mat,0);        
    px_len=dl-l+1;            
    for(i=0;i<px_len;i++) pxx[i]=pxx[i+l-1];
    for(i=0;i<px_len;i++) 
        for(j=0;j<dl;j++)
        flag[i*dl+j]=flag[(i+l-1)*dl+j];
    quick_sort(pxx,flag,0,px_len-1,dl);         
    for(I=1;;I++)
    {        
        other_huffman(dl,flag,pxx,d,mat,I);
        px_len=px_len-d+1;        
        for(i=0;i<px_len;i++) pxx[i]=pxx[i+d-1];
        for(i=0;i<px_len;i++) 
            for(j=0;j<dl;j++)
                flag[i*dl+j]=flag[(i+d-1)*dl+j];
        quick_sort(pxx,flag,0,px_len-1,dl);                        
        if(pxx[0]==1.0) break;
    }
    code_len=0;
    for(i=0;i<dl;i++) 
    {
        printf("X_%d:",i);
        
        for(j=dl-1;j>=0;j--)
            if(mat[j*dl+i]!=-1) 
            {
                printf("%d ",mat[j*dl+i]);
                code_len++;
            }        
        printf("\n");
    }  
    printf("Huffman_code's Average code length:%f",code_len*1.0/dl);
}

void first_huffman(int dl,int *flag,float *pxx,int l,int *mat,int time)
{
    int i,j;
    float tem=0;
    for(i=0;i<l-1;i++)
    {
        tem=tem+pxx[i];
        pxx[i]=0;
    }    
    pxx[l-1]=pxx[l-1]+tem;
    for(i=0;i<l;i++){
        for(j=0;j<dl;j++)
        {
            if(flag[i*dl+j]==1) mat[time*dl+j]=i;            
        }
    }
    
    for(i=0;i<dl;i++)
        for(j=0;j<l-1;j++)
        {
            flag[(l-1)*dl+i]=flag[(l-1)*dl+i]+flag[j*dl+i];
            flag[j*dl+i]=0;
        }          
}

void other_huffman(int dl,int *flag,float *pxx,int d,int *mat,int time)
{    
    
    int i,j;
    float tem=0;
    for(i=0;i<d-1;i++)
    {
        tem=tem+pxx[i];
        pxx[i]=0;
    }    
    pxx[d-1]=pxx[d-1]+tem;
    for(i=0;i<d;i++){
        for(j=0;j<dl;j++)
        {
            if(flag[i*dl+j]==1) mat[time*dl+j]=i;            
        }        
    }
    
    for(i=0;i<dl;i++)
        for(j=0;j<d-1;j++)
        {
            flag[(d-1)*dl+i]=flag[(d-1)*dl+i]+flag[j*dl+i];
            flag[j*dl+i]=0;
        }          
}

//将以前编的快速排序拿过来直接使用
void quick_sort(float *v,int *flag,int s,int n,int dl)
{
    int quick(float *v,int *flag,int left,int right,int dl);  
    if(s!=n)
    {
        int f;
        f=quick(v,flag,s,n,dl);        
        if(f>s)            
            quick_sort(v,flag,s,f-1,dl);
        if(f<n)
            quick_sort(v,flag,f+1,n,dl);        
    }
}

int quick(float *v,int *flag,int left,int right,int dl)
{
    float tem;    
    int *t;
    int i;
    t=(int *)malloc(sizeof(int)*dl);
    tem=v[left];
    for(i=0;i<dl;i++) t[i]=flag[left*dl+i];    
    while(left<right)        
    {        
        while(tem<=v[right]&&left<right)                
            right--;     
        v[left]=v[right];
        for(i=0;i<dl;i++) flag[left*dl+i]=flag[right*dl+i];
        while(tem>=v[left]&&left<right)
            left++;        
        v[right]=v[left];
        for(i=0;i<dl;i++) flag[right*dl+i]=flag[left*dl+i];
    }    
    v[left]=tem;  
    for(i=0;i<dl;i++) flag[left*dl+i]=t[i];
    free(t);
    return left;
}
