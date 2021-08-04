#include<stdio.h>
#include<stdlib.h>

// nc dh密钥



struct dh
{
    int base;
    int prikey;
    int mod;
    int tem;
    int pubkey;
}d1,d2;

int main()
{
    
    int PowerMod(int a, int b, int c);
    printf("base:6 mod :251\n");
//     密钥发起方    
    d1.prikey=(rand()%256|128);
    printf("a:%d ",d1.prikey);
    d1.base=6;
    d1.mod=251;
    d1.tem=PowerMod(d1.base,d1.prikey,d1.mod);
    printf("g^a:%d \n",d1.tem);    
//     密钥接受 收到了 d1的base，mod,tem
    d2.prikey=(rand()%256|128);
    printf("b:%d ",d2.prikey);
    d2.base=6;
    d2.mod=251;
    d2.tem=PowerMod(d2.base,d2.prikey,d2.mod);
    printf("g^b:%d \n",d2.tem);
//  双方生成共有的私钥
    d1.pubkey=PowerMod(d2.tem,d1.prikey,d1.mod);
    d2.pubkey=PowerMod(d1.tem,d2.prikey,d2.mod);
    printf("g^ab:%d \n",d1.pubkey);    
    unsigned char final_key[8];
    int i;
    printf("Final key:");
    for(i=0;i<8;i++)
    {        
        final_key[i]=d1.pubkey&0xff;
        printf("%02X",final_key[i]);
    }
    
    return 0;
}
int PowerMod(int a, int b, int c)
{
    int ans = 1;
    a = a % c;
    while(b>0) 
    {
        if(b % 2 == 1) 
            ans = (ans *a) % c;
        b = b/2;   
        a = (a * a) % c;
    }
    return ans;
}
