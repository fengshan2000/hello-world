#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include<openssl/md4.h>
#include<openssl/des.h>

unsigned const char un[] = "qwe";
unsigned const char cp[] = "1 2 3 ";
// PPTP三次握手
//     unsigned const char cp[] = "M y P w ";
// unsigned const char ac[] = "[]|}{?/><,`!2&&(";
// unsigned const char pc[] = "!@#$%^&*()_+:3|~";

unsigned char ac[16];//服务器随机数
unsigned char pc[16];//用户随机数
unsigned char Challenge[8];  //8位挑战数
unsigned char zphash[21];    //des密钥
unsigned char nt[16];        //用户密码的hash
unsigned char hash_nt[16];   //用户密码的hash的hash

void input(FILE *f)
{
    int i;
    for(i=0;i<16;i++)
        fscanf(f,"%hhx",&ac[i]);
    for(i=0;i<16;i++)
        fscanf(f,"%hhx",&pc[i]);
}

int main(int argc,char * argv[])
{
    
     FILE *f;
     if((f=fopen(argv[1],"rb"))==NULL)
     {
       printf("error!\n");
       exit(0);
     }
    input(f);
    

    //chalenge    
    
    unsigned char Digest[20];
    SHA_CTX context;    	      
    SHA1_Init(&context);
    SHA1_Update(&context,pc,16);
    SHA1_Update(&context,ac,16);
    SHA1_Update(&context,un,strlen(un));
    SHA1_Final(Digest,&context);
    memcpy(Challenge,Digest,8);
    printf("Challlenge:\n");
    int i;
    for(i = 0; i < 8; i++)
    {
        printf("%02X ", Challenge[i]);
    }    
    printf("\n");
    
    
//ntpasswordhash
    unsigned char CP[strlen(cp)];
    printf("PasswordHash:\n");
    for(i = 0; i < strlen(cp);i++)
    {        
        if(cp[i]==32)
            CP[i]=0;
        else
            CP[i]=cp[i];

    }        
    MD4_CTX c;    
    MD4_Init(&c);
    MD4_Update(&c,CP,strlen(cp));
    MD4_Final(nt,&c);
    for(i = 0; i < 16;i++)
    {
        printf("%02X ", nt[i]);
    }    
    printf("\n");
    
    
// hsah nt passwordhash    
    printf("PasswordHashHash:\n");    
    MD4_Init(&c);
    MD4_Update(&c,nt,16);
    MD4_Final(hash_nt,&c);    
    for(i = 0; i < 16;i++)
    {
        printf("%02X ", hash_nt[i]);
    }    
    printf("\n");
    
    
// ChallengeResponse 
    
    for(i=0;i<21;i++)
        if(i<16)
            zphash[i]=nt[i];
        else 
            zphash[i]=0;
    unsigned char * ntresponse;
    ntresponse=(unsigned char*)malloc(sizeof(unsigned char)*24);
    printf("NT-Response:\n");
    void ChallengeResponse(int I,unsigned char *ntresponse);
    ChallengeResponse(0,ntresponse);
    ChallengeResponse(1,ntresponse);
    ChallengeResponse(2,ntresponse);
    printf("\n");
    
    
//  AuthenticatorResponse
    printf("AuthenticatorResponse:\n");
    void AR(unsigned char *ntresponse);
    AR(ntresponse);
    
    return 0;
}

void ChallengeResponse(int I,unsigned char *ntresponse)
{    
    int i;
    uint64_t a=0,b=0;
    uint64_t tem;
    unsigned char t=0;
    unsigned char kk[8];
    for(i=0+7*I;i<7+7*I;i++)
        kk[i-7*I+1]=zphash[i];    
	for(i=0;i<8;i++)
    {        
        a=a^kk[i];        
        a=a<<8;
    }    
    for(i=0;i<8;i++)
    {
        b=b<<8;
        t=(a&0xff00000000000000)>>56;        
        t=t&0b11111110;
        a=a<<7;        
        b=b^t;
    }    
    for(i=0;i<8;i++)
    {        
        kk[i]=(b&0xff00000000000000)>>56;        
        b=(b<<8)&0xffffffffffffff00;        
    }                
    DES_cblock key;    
    for(i=0;i<8;i++)
        key[i]=kk[i];    
    
    DES_cblock output;
    const_DES_cblock input;    
    for(i=0;i<8;i++)
        input[i]=Challenge[i];       
    DES_key_schedule schedule;    
    DES_set_key_unchecked(&key, &schedule); 
    DES_ecb_encrypt(&input, &output,&schedule, DES_ENCRYPT);
    for (i = 0; i < sizeof(input); i++)
    {
        
        ntresponse[I*8+i]=output[i];
        printf("%02X ",output[i]);    
    }
}


void AR(unsigned char *ntresponse)
{
    int i;
    
    unsigned char Magic1[39] =
        {0x4D, 0x61, 0x67, 0x69, 0x63, 0x20, 0x73, 0x65, 0x72, 0x76,
        0x65, 0x72, 0x20, 0x74, 0x6F, 0x20, 0x63, 0x6C, 0x69, 0x65,
        0x6E, 0x74, 0x20, 0x73, 0x69, 0x67, 0x6E, 0x69, 0x6E, 0x67,
        0x20, 0x63, 0x6F, 0x6E, 0x73, 0x74, 0x61, 0x6E, 0x74};
    unsigned char Magic2[41] =
        {0x50, 0x61, 0x64, 0x20, 0x74, 0x6F, 0x20, 0x6D, 0x61, 0x6B,
        0x65, 0x20, 0x69, 0x74, 0x20, 0x64, 0x6F, 0x20, 0x6D, 0x6F,
        0x72, 0x65, 0x20, 0x74, 0x68, 0x61, 0x6E, 0x20, 0x6F, 0x6E,
        0x65, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6F,
        0x6E};
        
    SHA_CTX context;
    unsigned char Digest[20];
    //chalenge	      
    SHA1_Init(&context);
    SHA1_Update(&context,hash_nt,16);
    SHA1_Update(&context,ntresponse,24);
    SHA1_Update(&context,Magic1,39);
    SHA1_Final(Digest,&context);    
            
    SHA1_Init(&context);
    SHA1_Update(&context,Digest,20);
    SHA1_Update(&context,Challenge,8);
    SHA1_Update(&context,Magic2,41);
    SHA1_Final(Digest,&context); 
    for(i = 0; i < 20; i++)
    {
        printf("%02X",Digest[i]);
    }    
    printf("\n");
}
