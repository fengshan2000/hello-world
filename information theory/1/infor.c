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
        if(sum!=1) 
            return 0;
    }
    return 1;
}
//迭代算法 arimoto
void arimoto(float mat[count1][count2])
{
    
    
    int i,j,k,N=3;
    float px[count2];
    float pX[count2];
    float py[count1];
    float tem;
    //设置px初值
    for(i=0;i<count2;i++) 
    {
        px[i]=1.0/count2;
//         printf("px %f ",px[i]);        
    }
    for(k=0;k<N;k++){
        //由px得到py的值        
        for(i=0;i<count1;i++)
        {
            py[i]=0;
            for(j=0;j<count2;j++)
            {    
                py[i]=py[i]+px[j]*mat[j][i];
            }    
        }        
        //得到由y到x的概率转移矩阵Mat[][]
        float Mat[count2][count1];
        for(i=0;i<count2;i++)
        {
            for(j=0;j<count1;j++)
            {
            Mat[i][j]=(px[i]*mat[i][j])/py[j];            
            }            
        }        
        //计算出新的px        
        for(i=0;i<count2;i++){
            tem=0;
            for(j=0;j<count1;j++){
                if(Mat[i][j]!=0)
                    tem=tem+mat[i][j]*log2(Mat[i][j]);                            
            }
            pX[i]=pow(2,tem);            
        }
        tem=0;
        for(i=0;i<count1;i++){
            tem=tem+pX[i];        
        }
        for(i=0;i<count1;i++){
            px[i]=pX[i]/tem;            
        }
        tem=0;
        for(i=0;i<count1;i++){
            tem=tem+px[i];        
        }              
    }
    printf("px:");
    for(i=0;i<count2;i++){
        printf("%f ",px[i]);
    }
//     计算当前信道的信道容量
    for(i=0;i<count1;i++){
        py[i]=0;
        for(j=0;j<count2;j++){    
            py[i]=py[i]+px[j]*mat[j][i];
        }    
    }     
    tem=0;
    for(i=0;i<count2;i++){
        for(j=0;j<count1;j++){
            if(mat[i][j]/py[j]!=0)
                tem=tem+px[i]*mat[i][j]*log2(mat[i][j]/py[j]);
        }
    }
    printf("\nCHANNEL CAPACITY:%f",tem);
}
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
//     printf("%d \n",datalen);
    float *mmat;    
    mmat=(float*)malloc(sizeof(float)*datalen);
    float* inp(FILE *f,int datalen,float *mmat);
    inp(f,datalen,mmat);    
    
    float mat[count2][count1];
//       printf("%d %d \n",count1,count2);
    for(i=0;i<count2;i++)
    {
        for(j=0;j<count1;j++)
        {            
            mat[i][j]=mmat[i*count1+j];
            printf("%4.2f ",mat[i][j]);
        }   
        printf("\n");
    }
    free(mmat);    
    if(ifr(mat)==0)
    {
        printf("error input");
        return 0;
    }    
//     arimoto
    void arimoto(float mat[count1][count2]);
    arimoto(mat);           
    return 0;
}
