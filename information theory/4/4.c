//迭代算法
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int count1=0,count2=0;
//对相应文件中的矩阵进行读取，并得到矩阵的行列数
float* inp(FILE *f,int datalen,float *mmat)
{        
    char ch;
    int i,j;        
    for(i=0;i<datalen/2;i++)              //32 " " 10"\n"
    {
        
        fscanf(f,"%f",&mmat[i]);
        fscanf(f,"%ch",&ch);    
        if(ch==32)
            count1++;
        else 
            if(ch==10)
                count2++;
        if(ftell(f)==datalen)
            break;
    }
    count1=count1/count2+1;               
    return mmat;
}    
//判断状态转移矩阵是否正确 正确返回1 不正确返回0
int ifr(float mat[count2][count1])
{
    int i,j;
    float sum;
    for(i=0;i<count2;i++)
    {
        for(sum=0,j=0;j<count1;j++)
        {
            if(mat[i][j]<0||mat[i][j]>1) 
                return 0;
            sum=sum+mat[i][j];        
        }        
        if(fabs(sum-1)>=0.000001) 
            return 0;
    }
    return 1;
}
//迭代算法 arimoto



int main(int argc,char *argv[])
{
    FILE *f;
    if((f=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    int datalen,i,j;
    fseek(f,0,2);datalen=ftell(f);fseek(f,0,0);     
    float *mmat;    
    mmat=(float*)malloc(sizeof(float)*datalen);
    float* inp(FILE *f,int datalen,float *mmat);
    inp(f,datalen,mmat);        
    float mat[count2][count1];
    
    for(i=0;i<count2;i++)
    {
        for(j=0;j<count1;j++)
        {            
            mat[i][j]=mmat[i*count1+j];
            printf("%4.6f ",mat[i][j]);
        }   
        printf("\n");
    }
    free(mmat);    
    if(ifr(mat)==0)
    {
        printf("error input");
        return 0;
    }    
    
    float px[count2];
    printf("\nplease input your px\n");
    for(i=0;i<count2;i++)
    {
        scanf("%f",&px[i]);        
    }
    void mle(int count1,int count2,float mat[count2][count1],float px[count2]);
    mle(count1,count2,mat,px);
    void map(int count1,int count2,float mat[count2][count1],float px[count2]);
    map(count1,count2,mat,px);
    return 0;
}

void mle(int count1,int count2,float mat[count2][count1],float px[count2])
{
    printf("\n MLE:\n");
    int g[count1];
    float ftem;
    int i,j;
    int tem;
    for(i=0;i<count1;i++)
    {
        tem=0;ftem=mat[i][0];
        for(j=0;j<count2;j++)
        {            
//             printf("%f %f \n",mat[j][i],ftem);
            if(mat[j][i]>ftem)
            {                
                tem=j;
                ftem=mat[j][i];
            }
        }
        g[i]=tem;
        printf("g(%d)->%d \n",i,g[i]);
    }
    ftem=0;
    for(i=0;i<count2;i++)
        for(j=0;j<count1;j++)
            if(g[j]!=i) ftem=ftem+mat[i][j]*px[i];
    printf("Pe\n %f",ftem);
}
void map(int count1,int count2,float mat[count2][count1],float px[count2])
{
    printf("\nMAP: \n");
    int g[count1];
    float ftem;
    float ma[count2][count1];
    int i,j;
    int tem;
    for(i=0;i<count2;i++)    
        for(j=0;j<count1;j++)        
            ma[j][i]=mat[j][i]*px[i];        
    
    for(i=0;i<count1;i++)
    {
        tem=0;ftem=ma[i][0];
        for(j=0;j<count2;j++)
        {                         
            if(ma[j][i]>ftem)
            {                
                tem=j;
                ftem=ma[j][i];
            }
        }
        g[i]=tem;
        printf("g(%d)->%d \n",i,g[i]);
    }
    
    ftem=0;
    for(i=0;i<count2;i++)
        for(j=0;j<count1;j++)
            if(g[j]!=i) ftem=ftem+mat[i][j]*px[i];
    printf("Pe:\n %f",ftem);
}
