#include<stdio.h>
#include<gmp.h>
#include <stdlib.h>
#include<time.h>
#include <openssl/sha.h>


#define COMPOSITE        0
#define PROBABLE_PRIME   1

int miller_rabin_pass(mpz_t a, mpz_t n) 
{
    int i, s, result;
    mpz_t a_to_power, d, n_minus_one;
    mpz_init(n_minus_one);
    mpz_sub_ui(n_minus_one, n, 1);
    s = 0;
    mpz_init_set(d, n_minus_one);
    while (mpz_even_p(d)) 
    {
        mpz_fdiv_q_2exp(d, d, 1);
        s++;
    }
    mpz_init(a_to_power);
    mpz_powm(a_to_power, a, d, n);
    if (mpz_cmp_ui(a_to_power, 1) == 0)  {
        result=PROBABLE_PRIME; 
        goto exit;
    }
    for(i=0; i < s-1; i++) {
        if (mpz_cmp(a_to_power, n_minus_one) == 0) {
            result=PROBABLE_PRIME; 
            goto exit;
        }
        mpz_powm_ui (a_to_power, a_to_power, 2, n);
    }
    if (mpz_cmp(a_to_power, n_minus_one) == 0) {
        result=PROBABLE_PRIME; 
        goto exit;
    }
    result = COMPOSITE;
    exit:
    mpz_clear(a_to_power);
    mpz_clear(d);
    mpz_clear(n_minus_one);
    return result;
}


int miller_rabin(mpz_t n, gmp_randstate_t rand_state) 
{
    mpz_t a;
    int repeat;
    mpz_init(a);
    for(repeat=0; repeat<20; repeat++) {
        do 
        {
            mpz_urandomm(a, rand_state, n);
        } while (mpz_sgn(a) == 0);
        if (miller_rabin_pass(a, n) == COMPOSITE) 
        {
            return COMPOSITE;
        }
    }
    return PROBABLE_PRIME;
}


void get_inverse(mpz_t e, mpz_t euler,mpz_t *tem,mpz_t *a,mpz_t *b)
{
	mpz_t zero,one,i;
	mpz_init_set_str(zero, "0", 10);
	mpz_init_set_str(one, "1", 10);
	mpz_init(i);
	if (mpz_cmp(zero, euler) == 0)
	{
		mpz_set(*tem, e);
		mpz_set(*a, one);
		mpz_set(*b, zero);
		return;
	}
	mpz_mod(i, e, euler);
	get_inverse(euler,i,tem,b,a);
	mpz_div(i, e, euler);
	mpz_mul(i, i, *a);
	mpz_sub(*b, *b, i);
	mpz_clear(zero);
	mpz_clear(one);
	mpz_clear(i);
}

void sign(char *ch,mpz_t *sig,mpz_t e,mpz_t n)
{
    unsigned char Digest[20];
    SHA_CTX context;    	      
    SHA1_Init(&context);
    SHA1_Update(&context,ch,strlen(ch));
    SHA1_Final(Digest,&context);
    mpz_t sha1_message;
    mpz_init_set_str(sha1_message,Digest,10);            
    mpz_powm(*sig,sha1_message,e,n);
}


int main(int argc,char* argv[])
{
    FILE *f,*fout;
    if((f=fopen(argv[1],"rb"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    if((fout=fopen(argv[2],"w"))==NULL)
    {
      printf("error!\n");
      exit(0);
    }
    int filelen;
    fseek(f,0,2);filelen=ftell(f);fseek(f,0,0);
    char *ch;
    ch=(char *)malloc(sizeof(char)*filelen);
    fread(ch,filelen,sizeof(char),f);
    ch[filelen]='\0';
//     time
    clock_t start,stop;
    double t;
    int j;    
//     
    mp_bitcnt_t n_bit=102;
    
    mpz_t p,q,n,phi_n,e,d,i,x,y,tem;    
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(phi_n);    
    mpz_init(e);
    mpz_init(d);    
    mpz_init(i);
    mpz_init(x);
    mpz_init(y);
    mpz_init(tem);
    
    gmp_randstate_t state;
    gmp_randinit_default(state);      
    
//     gmp_randseed(state,time);
    gmp_randseed_ui(state,4012018013);
    
    
    do{
    mpz_urandomb(p,state,n_bit);        
    }while(miller_rabin(p,state)==COMPOSITE);
    gmp_printf("p         :%Zd \n",p);        
    
    do{
    mpz_urandomb(q,state,n_bit);        
    }while(miller_rabin(q,state)==0);
    gmp_printf("q         :%Zd \n",q);    
    
    mpz_mul(n,p,q);
//     gmp_printf("%Zd  \n",n); 
    
    mpz_sub_ui(p,p,1);
    mpz_sub_ui(q,q,1);
    mpz_mul(phi_n,p,q);
//     gmp_printf("%Zd  \n",phi_n);
            
    do{
    mpz_urandomm(e,state,phi_n);        
    mpz_gcd(i,e,phi_n);
    }while(mpz_cmp_ui(i,1)!=0);    
    gmp_printf("privatekey:%Zd \n",e);
//     找到一个私钥e
    
//     mpz_invert(d,e,phi_n);    
//     gmp_printf("%Zd  \n",d);        
                
    get_inverse(e, phi_n, &tem, &x, &y);
    mpz_add(d,phi_n, x);
    mpz_mod(d,d,phi_n);
    
    gmp_printf("public key:%Zd  \n",d);
            
    mpz_t message,ciphertext;    
    mpz_init(ciphertext);
                        
    mpz_init_set_str(message,ch,10);
    gmp_printf("message   :%Zd  \n",message);
    //     encode
        mpz_powm(ciphertext,message,d,n);
    //     decode
        mpz_powm(i,ciphertext,e,n);
    gmp_printf("ciphertext:%Zd  \n",ciphertext);
    gmp_printf("check d(c):%Zd  \n",i);
    mpz_out_str(fout,10,ciphertext);
    
//     重新选取参数 p,q,e 并且进行一次加解密
    start=clock();
    for(j=0;j<10000;j++)
    {        
        do{
            mpz_urandomb(p,state,n_bit);        
        }while(miller_rabin(p,state)==COMPOSITE);
//         gmp_printf("p         :%Zd \n",p);                
        do{
            mpz_urandomb(q,state,n_bit);        
        }while(miller_rabin(q,state)==0);
//         gmp_printf("q         :%Zd \n",q);    
        
        mpz_mul(n,p,q);        
        mpz_sub_ui(p,p,1);
        mpz_sub_ui(q,q,1);
        mpz_mul(phi_n,p,q);

//     gmp_printf("ciphertext%Zd  \n",ciphertext);
        do{
            mpz_urandomm(e,state,phi_n);        
            mpz_gcd(i,e,phi_n);
        }while(mpz_cmp_ui(i,1)!=0);
//         get d
        get_inverse(e, phi_n, &tem, &x, &y);
        mpz_add(d,phi_n, x);
        mpz_mod(d,d,phi_n);
        //     encode
        mpz_powm(ciphertext,message,d,n);
    //     decode
        mpz_powm(i,ciphertext,e,n);
//     gmp_printf("d(c):%Zd  \n",i);
    }    
    stop=clock();      
    t = ((double)(stop - start)) / CLOCKS_PER_SEC;        
    
    printf("ave time  :%f s",t/10000);
            
//     signature
    sign(ch,&tem,e,n);
    
    mpz_clears(p,q,n,phi_n,e,d,i,x,y,tem,message,ciphertext);
    fclose(f);fclose(fout);
    gmp_printf("d(c):%Zd  \n",tem);    
    return 0;
}

    
