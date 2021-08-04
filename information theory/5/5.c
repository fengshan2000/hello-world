//限失真信源编码迭代算法
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define e 2.7182818284590452354
#define epsilon 0.0000001

int col=0,row=0;
float *px;  
float *py;
float *d;
float *pxy;
float R_s[2],D_s[2];

float* inp(FILE *f,int datalen,float *mmat)
{        
    char ch;
    int i,j;        
    for(i=0;i<datalen/2;i++)              //32 " " 10"\n"
    {
        
        fscanf(f,"%f",&mmat[i]);
        fscanf(f,"%ch",&ch);    
        if(ch==32)
            col++;
        else 
            if(ch==10)
                row++;
        if(ftell(f)==datalen)
            break;
    }
    col=col/row+1;   
    px=(float *)malloc(sizeof(float)*row);
    py=(float *)malloc(sizeof(float)*col);
    d=(float *)malloc(sizeof(float)*col*row);
    pxy=(float *)malloc(sizeof(float)*col*row);
    return mmat;
}    

int d_row_min(int row)
{
    int i,j=0;
    float tem=d[row*col+0];
    for(i=0;i<col;i++)
        if(tem>d[row*col+i])
        {
            tem=d[row*col+i];
            j=i;            
        }
    return j;
}


float get_d_min()
{    
    int i,j;
    int mat[row][col];
    
    float tem;
    
    for(i=0;i<row;i++)
        for(j=0;j<col;j++)        
            mat[i][j]=0;
    for(i=0;i<row;i++)
    {
        tem=tem+px[i]*d[i*col+d_row_min(i)];
        mat[i][d_row_min(i)]=1;
    }
    printf("\n\n   d_min:%f",tem);
    for(i=0;i<col;i++){
        py[i]=0;
        for(j=0;j<row;j++){    
            py[i]=py[i]+px[j]*mat[j][i];
        }    
    }     
    tem=0;
    for(i=0;i<row;i++){
        for(j=0;j<col;j++){
            if(mat[i][j]/py[j]!=0)
                tem=tem+px[i]*mat[i][j]*log2(mat[i][j]/py[j]);
        }
    }
    printf("\nR(d_min):%f",tem);
    return tem;
}

float get_d_max()
{
    int i,j,k;    
    float tem=100,tem1;
    for(i=0;i<col;i++)
    {
        tem1=0;
        for(j=0;j<row;j++)
            tem1=tem1+px[j]*d[i*col+j];        
        if(tem1<tem)
        {
            k=i;
            tem=tem1;
        }        
    }
    printf("\n   d_max:%f ",tem);
    printf("\nR(d_max):0 \n");
    return tem;
}
void get_rd_d(FILE *f3)
{
    float s;
    float tem=0;
    int i,j;
//     pij=1/m;
    for(i=0;i<row;i++)
        for(j=0;j<col;j++)
        pxy[i*col+j]=1.0/col;
//     求py
    for(i=0;i<col;i++)        
    {
        py[i]=0;
        for(j=0;j<row;j++)
            py[i]=py[i]+px[j]*pxy[j*col+i];
        printf("%f ",py[i]);    
    }
//     函数对pxy，py
    void refresh_pxy_py(float s);
    void check_pxy_py(float s);
    R_s[0]=0;D_s[0]=0;
    
    for(s=-10;s<0;s=s+0.001)
    {
        check_pxy_py(s);
        do{
            R_s[0]=R_s[1];
            D_s[0]=D_s[1];
            refresh_pxy_py(s);
            check_pxy_py(s); 
            
        }while((fabs(R_s[0]-R_s[1])>epsilon)&&(fabs(D_s[0]-D_s[1])>epsilon));
     fprintf(f3,"%2.16f %2.5f \n",D_s[1],R_s[1]);
        
    }

}

void refresh_pxy_py(float s)
{
    int i,j,k;
    float tem;
    float py2[col],pxy2[row*col];
    for(i=0;i<col;i++)
        py2[i]=py[i];
    for(i=0;i<row;i++)
        for(j=0;j<col;j++)
            pxy2[i*col+j]=pxy[i*col+j];
//  1.pxy   
    for(i=0;i<row;i++)
        for(j=0;j<col;j++)
        {
            tem=0;
            for(k=0;k<col;k++)
                tem=tem+py2[k]*pow(e,s*d[i*col+k]);
            pxy[i*col+j]=(py2[j]*pow(e,s*d[i*col+j]))/tem;
        }
//  2.py
    for(i=0;i<col;i++)        
    {
        py[i]=0;
        for(j=0;j<row;j++)
            py[i]=py[i]+px[j]*pxy[j*col+i];        
    }
}

void check_pxy_py(float s)
{
    float lambda[row];
    float tem;
    int i,j;
    
    for(i=0;i<row;i++)
    {
        tem=0;
        for(j=0;j<col;j++)
        {
            tem=tem+py[j]*pow(e,s*d[i*col+j]);
        }
        lambda[i]=1.0/tem;
    }
// D_s
    tem=0;
    for(i=0;i<row;i++)
        for(j=0;j<col;j++)
            tem=tem+lambda[i]*px[i]*py[j]*d[i*col+j]*pow(e,s*d[i*col+j]);
    
    D_s[1]=tem;
//     R_s
    tem=s*tem;
    for(i=0;i<row;i++)
        tem=tem+px[i]*log2(lambda[i]);
    R_s[1]=tem;    
}

int main(int argc,char *argv[])
{
    FILE *f1,*f2,*f3;
    if((f1=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    if((f2=fopen(argv[2],"rb"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    if((f3=fopen(argv[3],"wt"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    int datalen,i,j;
    fseek(f1,0,2);datalen=ftell(f1);fseek(f1,0,0);     
    float *mmat;    
    mmat=(float*)malloc(sizeof(float)*datalen);
    float* inp(FILE *f,int datalen,float *mmat);
    inp(f1,datalen,mmat);        
    
    printf("d(x,y):\n");
    for(i=0;i<row;i++)
    {
        for(j=0;j<col;j++)
        {            
            d[i*col+j]=mmat[i*col+j];
            printf("%4.6f ",d[i*col+j]);
        }   
        printf("\n");
    }
    free(mmat);
    printf("px:\n");
    for(i=0;i<row;i++)
    {
        fscanf(f2,"%f",&px[i]);
        printf("%f ",px[i]);
    }
    float d_min,d_max;
    d_min=get_d_min();
    d_max=get_d_max();
    get_rd_d(f3);
        
    return 0;
}
