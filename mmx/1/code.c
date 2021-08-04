
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

static char E[] = {
    32,  1,  2,  3,  4,  5,  
     4,  5,  6,  7,  8,  9,  
     8,  9, 10, 11, 12, 13, 
    12, 13, 14, 15, 16, 17, 
    16, 17, 18, 19, 20, 21, 
    20, 21, 22, 23, 24, 25, 
    24, 25, 26, 27, 28, 29, 
    28, 29, 30, 31, 32,  1
};


/* Post S-Box permutation */
static char P[] = {
    16,  7, 20, 21, 
    29, 12, 28, 17, 
     1, 15, 23, 26, 
     5, 18, 31, 10, 
     2,  8, 24, 14, 
    32, 27,  3,  9, 
    19, 13, 30,  6, 
    22, 11,  4, 25
};

/* Permuted Choice 1 Table */
static char PC1[] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};

/* Permuted Choice 2 Table */
static char PC2[] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};
static char key_shift[] = {
 /* 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16 */
    1,  1,  2,  2,  2,  2,  2,  2,  1,  2,  2,  2,  2,  2,  2,  1
};

//初始密钥中未知位，unknow>>4
static char unknow[]={9,18,22,25,35,38,43,54};
/* The S-Box tables */
static char S[8][64] = {{
    /* S1 */
    14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,  
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,  
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0, 
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
},{
    /* S2 */
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,  
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,  
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15, 
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
},{
    /* S3 */
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,  
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,  
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
},{
    /* S4 */
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,  
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,  
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
},{
    /* S5 */
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9, 
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6, 
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14, 
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
},{
    /* S6 */
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
},{
    /* S7 */
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
},{
    /* S8 */
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
}};



int main(int argc,char *argv[])
{
//     读取data.txt 文件中的数据
    
    FILE *f;
    if((f=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    
    
    uint8_t j,k,num_data=3;
    uint8_t n1=6;//给定的明密文对数量
    uint8_t E_R_1i[8],E_R_2i[8];    
    uint8_t test;
    uint8_t Si_output[8];
    uint8_t flag[8][64]={0};
    
    uint16_t i;
    
    
//     L3,L3*;
    uint32_t L3_1,L3_2;
    uint32_t tem32;
    uint32_t key_c,key_d;
    uint32_t S_output,PS_output=0;
    
    uint64_t k3=0;
    uint64_t tem64;   
    uint64_t key_cd,key_test;
    uint64_t key_target;    
//  E_L3 32-48    
    uint64_t E_R_1=0;
    uint64_t E_R_2=0;
    uint64_t key=0;
    uint64_t data[n1][2];            
    
    
    //E:S盒输入前的扩展 32-48     
    uint64_t E_32_48(uint32_t a);
//     Si盒的函数
    uint8_t Si_6_4(int i,uint8_t input);
    uint32_t key_c_left(uint8_t i,uint32_t key_c);
    uint64_t fake_des3(uint64_t key_cd0,uint64_t plaintext);    
    
//从文件指针中读取数据     
    for(i=0;i<n1;i++)
        fscanf(f,"%llX %llX",&data[i][0],&data[i][1]);    
    fclose(f);
    
//  对给定的数据进行差分，
    for(k=0;k<num_data;k++)
    {
//      得到S盒 的R输入部分
        L3_1=data[k*2][1]>>32;
        L3_2=data[k*2+1][1]>>32;                                
        E_R_1=E_32_48(L3_1);
        E_R_2=E_32_48(L3_2);    
        
    //    每个L3的对应S盒子的输入 6位        
        for(i=0;i<8;i++)
        {
            E_R_1i[7-i]=(E_R_1>>(i*6))&0b00111111;            
        }
        for(i=0;i<8;i++)
        {
            E_R_2i[7-i]=(E_R_2>>(i*6))&0b00111111;                
        }            
        
    //     输出差分        
        PS_output=((data[k*2][1]^data[k*2+1][1])&0xffffffff)^(((data[k*2][0]^data[k*2+1][0])>>32)&0xffffffff);
        
        S_output=0;
        for(i=0;i<32;i++)
        {
            tem32=(PS_output>>(31-i))&0b01;
            tem32=tem32<<(32-P[i]);    
            S_output=S_output|tem32;
        }            
        for(i=0;i<8;i++)
        {
            Si_output[7-i]=(S_output>>(i*4))&0x0f;    
        }
        
//         对每个E_R_1i[]的元素 过S盒子
        for(i=0;i<8;i++)
        {
            for(j=0;j<63;j++)
            {            
                if((Si_6_4(i,j^E_R_1i[i])^Si_6_4(i,j^E_R_2i[i]))==Si_output[i])
                    flag[i][j]++;                
            }                 
        }                
    }
    
//     得到48bit的k3        
    k3=0;
    for(i=0;i<8;i++)        
        {
            for(j=0;j<63;j++)        
                if(flag[i][j]==3)
                {
                    k3=k3<<6;                
                    k3=k3|j;
                }            
        }    
    
// 直接pc-2反求出当时的c3 d3
    uint64_t subkey3=0;
    
    for(i=0;i<48;i++)
    {
        tem64=(k3>>(47-i))&1;        
        tem64=tem64<<(56-PC2[i]);         
        subkey3=subkey3|tem64;
    }    
            
    //  key_cd0中未知位的位置
    for(i=0;i<8;i++)
    {
        if(unknow[i]<29)
            unknow[i]=(unknow[i]+4)%28;
        else
            unknow[i]=28+(unknow[i]-28+4)%28;            
    }    
// 求得C0D0
    key_d=subkey3&0x0fffffff;
    key_c=(subkey3>>28)&0x0fffffff;                    
//     左移24相当于 右移4位
    key_c=key_c_left(24,key_c);
    key_d=key_c_left(24,key_d);                        
    key_cd=key_c;
    key_cd=key_cd<<28;
    key_cd=key_cd|key_d;    
//  填补C0D0中的未知的位置
    for(i=0;i<256;i++)
    {       
        key_test=key_cd;
//         填补相应的未知位置
        for(j=0;j<8;j++)
        {
            tem64=(i>>(7-j))&1;
            tem64=tem64<<(56-unknow[j]);
            key_test=key_test|tem64;            
        }
//         测试通过加密过程能否得到相应密文
         if(fake_des3(key_test,data[0][0])==data[0][1])
         {
             key_target=key_test;
             break;
         }
    }
//     得到相应的C0D0后 利用PC1求逆得初始密钥key
    key=0;
    for(i=0;i<56;i++)
    {
        tem64=(key_target>>(55-i))&1;        
        tem64=tem64<<(64-PC1[i]);         
        key=key|tem64;
    }
//  输出最终求得的密钥
    printf("%016llX ",key);
    return 0;
}

// 题目中的des3轮加密流程 给定key_cd0而非初始key 
uint64_t fake_des3(uint64_t key_cd0,uint64_t plaintext)
{   
    uint8_t i,j;
    
    uint32_t tem32;
    uint32_t L=(plaintext>>32)&0x00000000ffffffff;
    uint32_t R=plaintext&0x00000000ffffffff;
        
    uint64_t tem64;
    uint64_t roundkey=key_cd0;
        
    uint64_t E_32_48(uint32_t a);
    uint32_t S_1_8(uint64_t roundkey,uint64_t ER);
    uint64_t key_cd_left(uint8_t i,uint64_t key_cd);
    uint64_t PC2_56_48(uint64_t roundkey);
    uint32_t P_Soutput(uint32_t output);
        
    for(i=0;i<3;i++)
    {
        roundkey=key_cd_left(key_shift[i],roundkey);
        tem32=R;                
        R=L^P_Soutput(S_1_8(PC2_56_48(roundkey),E_32_48(R)));        
        L=tem32;                        
    }     
    tem64=L;
    tem64=tem64<<32;
    tem64=(tem64&0xffffffff00000000)|R;    
    return tem64;
}


// 实现每轮密钥中一边 32bit c的左移i位
uint32_t key_c_left(uint8_t i,uint32_t key_c)
{
    uint8_t j;
    uint32_t tem32;
    for(j=0;j<i;j++)
    {
        tem32=(key_c>>27)&0b01;
        key_c=(key_c<<1)&0x0fffffff;
        key_c=key_c|tem32;
    }
    return key_c;
}

// 每轮密钥CD两边同时左移i位
uint64_t key_cd_left(uint8_t i,uint64_t key_cd)
{
    uint32_t c=(key_cd>>28)&0x0fffffff;
    uint32_t d=key_cd&0x0fffffff;            
    c=key_c_left(i,c);
    d=key_c_left(i,d);    
    key_cd=c;
    key_cd=key_cd<<28;    
    key_cd=key_cd|d;
    return key_cd;
}

// 对第i个s盒输出input 返回 输出
uint8_t Si_6_4(int i,uint8_t input)
{
    uint8_t row=0,col=0,re;
    col=(input>>1)&0b00001111;
    row=(input>>5)&0b00000001;
    row=(row<<1)|(input&0b00000001);
    re=S[i][row*16+col];
    return re;
}

// 将对应的R32扩展为48位
uint64_t E_32_48(uint32_t a)
{
    uint64_t re=0;
    int i;
    for(i=0;i<48;i++){
        re=re<<1;
        re=re|(uint64_t)(a>>(32-E[i]))&0x00000001;          
    }
    return re;  
}

//已有 key48，E(R)48 输出S盒后的S_output 32
uint32_t S_1_8(uint64_t roundkey,uint64_t ER)
{
    uint8_t i;
    uint8_t si_input;
    uint8_t si_out;
    uint32_t out=0;
    uint64_t tem64=roundkey^ER;
    
    for(i=0;i<8;i++)
    {
        out=out<<4;
        si_input=tem64>>(42-i*6)&0x3f;
        si_out=Si_6_4(i,si_input);
        out=out|si_out;
    }
    
    return out;
}
// 求得每轮密钥(56)去掉8位后实际进行运算的密钥(48)
uint64_t PC2_56_48(uint64_t roundkey)
{
    uint8_t i;
    uint64_t tem64=0;
    for(i=0;i<48;i++)
    {
        tem64=tem64<<1;
        tem64=tem64|((roundkey>>(56-PC2[i]))&0x1);
    }
    return tem64;
}
// 对S盒整体输出的32比特通过P得到P_Soutput
uint32_t P_Soutput(uint32_t output)
{
    uint32_t tem32=0;
    uint8_t i;
    for(i=0;i<32;i++)
    {
        tem32=tem32<<1;
        tem32=tem32|(output>>(32-P[i]))&1;
    }
    return tem32;
}
