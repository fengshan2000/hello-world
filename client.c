

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN
#define Nonce_Len 16
#define Date_Len 11

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include<gmp.h>
#include<time.h>

#include "sm3.h"
#include "sm4.h"

sm4_context ctxt;

struct user{
    unsigned char name[10];
    unsigned char passwordHash[32];
    char state;
    char signature[20];
    unsigned char key[16];
}User;


DWORD WINAPI send_msg(LPVOID lpParam);
DWORD WINAPI recv_msg(LPVOID lpParam);

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

SOCKET ConnectSocket = INVALID_SOCKET;

time_t  t;
struct tm *tmp;

void user1print()
{
    int i;
    printf("\n|%10s ",User.name);
    for(i=0;i<32;i++)
        printf("%0X",User.passwordHash[i]);
    printf("%5d",User.state);
    printf(" %20s|\n",User.signature);
}

void Nonce(unsigned char data[16])
{
    int seed = 0;
    seed = (int)time(NULL);
    srand(seed);
    int cur_num = 0;
    for (int i = 0; i < 16; i++) {
        cur_num = rand() % 256;
            data[i] = cur_num;
        
        }

}
int cmp(unsigned char output[32],unsigned char output1[32],int min)
{
    int i;
    
    
    for(i=0;i<min;i++)
        if(output[i]==output1[i])
            continue;
        else
        {
            printf("cmp er%d  %d %02X %02X \n",strlen(output),i,output[i],output1[i]);
            return 1;
        }
    return 0;
}

 
    
int userNameCheck(SOCKET ConnectSocket)
{
    char i,exit;
    int iResult;
    char tembuf[DEFAULT_BUFLEN];            
    char recvbuf[DEFAULT_BUFLEN];        
    int recvbuflen = DEFAULT_BUFLEN;
            
//  client 登录注册首位为0
    tembuf[0]=0x00;
inf:
    printf("Do you have a account? \nIf you have it ,please input 1.\nIf you want to sign up,please input 2.\nIf you want to quit,please input 3.\n");
    scanf("%c",&exit);
//     第二位决定是登录data 还是logondata
    if((exit-0x30)==3)
        return 1;
    tembuf[1]=exit-0x30;    
    User.state=tembuf[1];
    if(exit<0x31||exit>0x33)
    {
        printf("Error input!");
        goto inf;
    }    
    
InputName1:    
    for(i=0;i<(int)strlen(recvbuf);i++)
    {
        recvbuf[i]=0;
        User.name[i]=0;
    }
    printf("\nPlease input your user name!\nName:");
    scanf("%s",&recvbuf);    
    iResult=strlen(recvbuf);
    for(i=2;i<iResult+2;i++)
        tembuf[i]=recvbuf[i-2];
    tembuf[i]=0x00;    
    
    if(iResult<6||iResult>10)
    {
        printf("Check your username length!");
        goto InputName1;
    }
    for(i=0;i<iResult;i++)
        User.name[i]=recvbuf[i];   
    printf("%s ",User.name);
    iResult = send(ConnectSocket, tembuf, (int)strlen(recvbuf)+2, 0);            
    if(exit==0x32)
        goto InputName2;            
    
    //          长度
    
            
    do{
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0); 
            
        if(iResult>0)
        {            
//           没有此用户
            if(recvbuf[1]==0x33)
            {
                printf("Username error!\nPlease check your username!\n");
                goto InputName1;
            }
            if(recvbuf[1]==0x31)
            {
                printf("correct name");
                goto step2;
            }
        }
        
    }while(1);                
InputName2:
    printf("\n---logon---\n");    
    do{
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);             
        if(iResult>0)
        {                        
//           此用户名已存在
            if(recvbuf[1]==0x34)
            {
                printf("Username error!Please input another username1\n");
                goto InputName1;
            }
            if(recvbuf[1]==0x31)
            {
                printf("correct name");
                break;
            }
        }
        
    }while(1);
step2:                         
    return 0;
}



int Userlogin()
{       
    printf("\n--logining--\n");
    int i;
    char namelen;
    namelen=strlen(User.name);
    
    unsigned char sendbuf[DEFAULT_BUFLEN];
    sendbuf[0]=0x1;
    sendbuf[1]=0x1;    
    sendbuf[2]=namelen;
    for(i=0;i<namelen;i++)
        sendbuf[3+i]=User.name[i];
    
    unsigned char recvbuf[DEFAULT_BUFLEN];        
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    unsigned char Nc[Nonce_Len];
    unsigned char Ns[Nonce_Len];    
    unsigned char Dc[Date_Len];
    unsigned char Dc2[Date_Len];
    unsigned char Ds[Date_Len];
    
    
    
    unsigned char output[32];    
    unsigned char output1[32];    
    sm3_context ctx;
//     step 0 生成相应的Nc Dc 
    
    Nonce(Nc);
    t = time(NULL);
	tmp = localtime(&t);
	strftime(Dc,sizeof(Dc),"%Y%m%H%M%S",tmp);
        
//    step2 1 
    sm3_starts( &ctx );
    sm3_update( &ctx, Nc, Nonce_Len );
    sm3_update( &ctx, Dc, Date_Len );
    sm3_update( &ctx, User.passwordHash,32);
    sm3_finish( &ctx, output );
    memset( &ctx, 0, sizeof( sm3_context ) );
    for(i=0;i<Nonce_Len;i++)
        sendbuf[i+3+namelen]=Nc[i];           
    for(i=0;i<Date_Len;i++)
        sendbuf[i+3+namelen+Nonce_Len]=Dc[i];
    for(i=0;i<32;i++)
        sendbuf[i+3+namelen+Nonce_Len+Date_Len]=output[i];             
    

    
    iResult = send(ConnectSocket, sendbuf, namelen+3+Nonce_Len+Date_Len+32, 0);        
    for(i=0;i<iResult;i++) sendbuf[i]=0;
    
//     step 31
    while(1)
    {                                                  
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);         
        if(iResult>0)
        {
            if(recvbuf[1]==0x0)
            {
                printf("error121");
                return 1;
            }
            for(i=0;i<Nonce_Len;i++)
                Ns[i]=recvbuf[i+2];
            for(i=0;i<Date_Len;i++)
                Ds[i]=recvbuf[i+2+Nonce_Len];
            for(i=0;i<32;i++)
                output1[i]=recvbuf[i+2+Nonce_Len+Date_Len];
            
            sm3_starts( &ctx );
            sm3_update( &ctx, Ns, Nonce_Len );
            sm3_update( &ctx, Ds, Date_Len );
            sm3_update( &ctx, User.passwordHash,32);
            sm3_finish( &ctx, output );
            memset( &ctx, 0, sizeof( sm3_context ) );
            
    //         check output1  sm3(ns,ds,ph)
            if(cmp(output1,output,32)!=0)
            {
                sendbuf[0]=0x1;
                sendbuf[1]=0x0;
                printf("error131");
                send(ConnectSocket,sendbuf,2,0);
                return 1;
            }
            memset( &ctx, 0, sizeof( sm3_context ) );
            
            for(i=0;i<32;i++)
                output1[i]=recvbuf[i+2+Nonce_Len+Date_Len+32];                                    
            sm3_starts( &ctx );
            sm3_update( &ctx, Nc, Nonce_Len );
            sm3_update( &ctx, Ds, Date_Len );
            sm3_update( &ctx, User.passwordHash,32);
            sm3_finish( &ctx, output );
            memset( &ctx, 0, sizeof( sm3_context ) );
    //         check output1  sm3(nc,ds,ph)
            if(cmp(output1,output,32)!=0)
            {
                sendbuf[0]=0x1;
                sendbuf[1]=0x0;
                printf("error132");
                send(ConnectSocket,sendbuf,2,0);
                return 1;
            }
            
            
            sendbuf[0]=0x1;
            sendbuf[1]=0x3;
    //         dc2
            t = time(NULL);
            tmp = localtime(&t);
            strftime(Dc2,sizeof(Dc2),"%Y%m%H%M%S",tmp);
            
            for(i=0;i<Date_Len;i++)
                sendbuf[i+2]=Dc2[i];
//         step 32                        
    
            sm3_starts( &ctx );
            sm3_update( &ctx, Ns, Nonce_Len );
            sm3_update( &ctx, Dc2, Date_Len );
            sm3_update( &ctx, User.passwordHash,32);
            sm3_finish( &ctx, output );
            memset( &ctx, 0, sizeof( sm3_context ) );
            
//             for(i=0;i<32;i++)
//                 printf("%02X",output[i]);
//             printf("\n");
            
            for(i=0;i<32;i++)
                sendbuf[i+2+Date_Len]=output[i];   
            
//             for(i=0;i<2+32+Date_Len;i++)
//                 printf("%02X",sendbuf[i]);
//             printf("\n");
            iResult = send(ConnectSocket, sendbuf, 2+Date_Len+32, 0);                                                    
//             printf("\n||%0d||\n",iResult);
//             for(i=0;i<iResult;i++)
//                 printf("%02X",sendbuf[i]);
//             printf("\n");
            break;
        }        
    }
    
    
//     step2 4
    while(1)
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0); 
        if(iResult>0)
        {
            if(recvbuf[1]==0x0)                
            {
                printf("error 14");
                return 1;
            }
            if(recvbuf[1]==0x4)
            {
                sendbuf[0]=0x3;                
                iResult = send(ConnectSocket, sendbuf, 1, 0);                                                    
                User.state=3;
                printf("\nlogin successful");
                break;
            }
        }
        if(iResult<0)
            return 1;
                                                            
    }
    //     生成共同的会话key=sm3(nc,ns,dc,ds,ph);
    sm3_starts( &ctx );
    sm3_update( &ctx, Nc, Nonce_Len );
    sm3_update( &ctx, Ns, Nonce_Len );
    sm3_update( &ctx, Dc, Date_Len );
    sm3_update( &ctx, Ds, Date_Len );
    sm3_update( &ctx, User.passwordHash,32);
    sm3_finish( &ctx, output );
    memset( &ctx, 0, sizeof( sm3_context ) );
    
    
    for(i=0;i<16;i++)
    {
        User.key[i]=output[2*i+1];
    }    
    
    printf("\n--longinend--\n");
    return 0;
}

unsigned char inttochar(int i)
{
    if(i<10&&i>=0)
        return (unsigned char)(i+0x30);
    else
        return (unsigned char)(i-9+0x40);
}

void Encrypt(mpz_t *result,mpz_t n, mpz_t m, mpz_t e)
{
	mpz_t i,j,k,l;                  //为了参与gmp库的大数运算，需要一些高精度变量存储一些数据

	mpz_init_set_str(i, "2", 10);     //用i代表常数2
	mpz_init_set_str(j, "1", 10);     //用j代表常数1
	mpz_init_set_str(l, "0", 10);     //用l代表常数0
	mpz_init(k);
	mpz_set(*result, j);

	mpz_mod(m, m, n);             //(a*b) mod c = ((a mod c)*(b mod c)) mod c
	while (mpz_cmp(e, l)>0) 
	{
		
		if (mpz_odd_p(e))             //判断是否是奇数 
		{
			/* result=(result*temp) mod euler */
			mpz_mul(*result, *result, m);
			mpz_mod(*result, *result, n);
		}
		mpz_div(e, e, i);
		mpz_mul(m, m, m);
		mpz_mod(m, m, n);
	}
	mpz_clear(i);
	mpz_clear(j);
	mpz_clear(k);
	mpz_clear(l);
}


int Userlogon()
{   
    printf("--logoning--%s--\n",User.name);
    int i;
    int namelen=strlen(User.name);
    int iResult;
    int flag[2]={0,0}; //0 n 1 e
    int recvbuflen = DEFAULT_BUFLEN;
    unsigned char sendbuf[DEFAULT_BUFLEN];
    unsigned char recvbuf[DEFAULT_BUFLEN];
    unsigned char n[1024];
    unsigned char e[1024];
    unsigned char cipher[1024];    
    mpz_t n_m,e_m,message_m,cipher_m;
    mpz_init(message_m);
    mpz_init(cipher_m);
    
    
    sendbuf[0]=0x2;
    sendbuf[1]=0x1;    
    sendbuf[2]=namelen;
    for(i=0;i<namelen;i++)
        sendbuf[3+i]=User.name[i];    
    
//     step2 1 发送请求
    iResult=send(ConnectSocket,sendbuf,3+namelen,0);
    
    while(1)
    {
        iResult=recv(ConnectSocket,recvbuf,recvbuflen,0);
        if(iResult>0)
        {
            if(recvbuf[2]==0x1) //n
            {
                for(i=0;i<1024;i++) n[i]=0x0;
                flag[0]=1;
                for(i=0;i<iResult-3;i++)
                {
                    if(0x30<=recvbuf[i+3]&&recvbuf[i+3]<0x3A)
                        n[i]=recvbuf[i+3];
                    else
                    {
                        break;
                    }                    
                }                               
                
                mpz_init_set_str(n_m,n,10);
                
            }    
            if(recvbuf[2]==0x2) //e
            {
                for(i=0;i<1024;i++) e[i]=0x0;
                flag[1]=1;
                for(i=0;i<iResult-3;i++)
                if(0x30<=recvbuf[i+3]&&recvbuf[i+3]<0x3A)
                        e[i]=recvbuf[i+3];
                    else
                    {
                        break;
                    }                
                mpz_init_set_str(e_m,e,10);                
            }
            if(recvbuf[0]==2&&recvbuf[1]==0)
            {
                printf("error");
                return 1;
            }
        }
        if((flag[0]==1)&&(flag[1]==1))
            break;
        if(iResult<0)
        {
            return 1;
        }        
    }        
    unsigned char tem[65];    
    for(i=0;i<32;i++)
    {
        
        tem[2*i]=inttochar((int)((User.passwordHash[i]>>4)&0b00001111));
        tem[2*i+1]=inttochar((int)((User.passwordHash[i])&0b00001111));
//         printf("%c%c ",tem[2*i],tem[2*i+1]);
    }    
//     step2 3 发送c=m^e mod n m=passwordHash
    mpz_init_set_str(message_m,tem,16);
    
    
    printf("\n???|%s \n",tem);
    Encrypt(&cipher_m,n_m,message_m,e_m);
    
    
    mpz_get_str(cipher,10,cipher_m);
    
    sendbuf[0]=0x2;
    sendbuf[1]=0x3;
    for(i=0;i<strlen(cipher);i++)
        sendbuf[i+2]=cipher[i];
    iResult=send(ConnectSocket,sendbuf,2+strlen(cipher),0);
    
    while(1)
    {
        iResult=recv(ConnectSocket,recvbuf,recvbuflen,0);
        if(iResult>0)
        {
            if(recvbuf[0]==2&&recvbuf[1]==0)
            {
                printf("error");
                return 1;
            }
            else
            {
                printf("\n--logoned--\n");
            }
            break;
        }
    }
    
    
    
    
    User.state=1;
    user1print();
    return 0;
}



int main(int argc, char **argv) 
{
    int i;
    WSADATA wsaData;    
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;    
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    
    int g;
//     step1 完成用户名的输入，及登录或者注册的选择
    int userNameCheck(SOCKET ConnectSocket);
    g=userNameCheck(ConnectSocket);
    
    if(g==1)
        return 0;
//  输入密码，保存到相应的User.passwordHash
    char password[16];
password:
    printf("\nPlease input your password:\n");
    scanf("%s",&password);
    int passwordlen=strlen(password);
    sm3(password, passwordlen, User.passwordHash);        
    
    
//     step2     
    if(User.state==1)
    {
        while((i=Userlogin())!=0)
        {            
            goto password;
        }   
    }
    else if(User.state==2)
    {
        while((i=Userlogon())!=0)
        {                        
            printf("error logon %d",i);
            goto password;
        }
        Sleep(500);
        while((i=Userlogin())!=0)
        {
            goto password;
        }
    }
    
    user1print();
    
    HANDLE hThread[2];
	DWORD dwThreadId;
    
    if(User.state==3)
    {
        printf("\n----------Wlecome to FengShan's Chatting Room!!----------\n");
        printf("\n|                  Exit with q/Q.                       |\n");
        printf("\n----------Wlecome to FengShan's Chatting Room!!----------\n");
    }
    do{
    if(User.state==3)
    {
        
     hThread[0] = CreateThread(
		NULL,		// 默认安全属性
		0,		// 默认堆栈大小
		send_msg,	// 线程入口地址（执行线程的函数）
		&ConnectSocket,		// 传给函数的参数
		0,		// 指定线程立即运行
		&dwThreadId);	// 返回线程的ID号
	hThread[1] = CreateThread(
		NULL,		// 默认安全属性
		0,		// 默认堆栈大小
		recv_msg,	// 线程入口地址（执行线程的函数）
		&ConnectSocket,		// 传给函数的参数
		0,		// 指定线程立即运行
		&dwThreadId);	// 返回线程的ID号       
    }
    }while(1);
    
    // 等待线程运行结束
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	printf(" Thread Over,Enter anything to over.\n");                                                                            
    return 0;
}
    
    
#define BUF_SIZE 256
#define NAME_SIZE 30
char C=0x4;
char msg[BUF_SIZE];
unsigned char C_message[DEFAULT_BUFLEN];
unsigned char P_message[DEFAULT_BUFLEN];
int P_m_len;
int C_m_len;

   DWORD WINAPI send_msg(LPVOID lpParam)
{
    int i;
	int sock = *((int*)lpParam);
	char name_msg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
            name_msg[0]=7;            
            send(sock, name_msg,1, 0);
			closesocket(sock);
			exit(0);
		}
		
		sprintf(P_message, "%c[%s]:%s",4,User.name, msg);        
        P_m_len=strlen(P_message);
        for(i=0;i<16-P_m_len%16;i++)
        {
            P_message[i+P_m_len]=' ';
        }
        P_m_len=strlen(P_message);
//         printf("P %d\n",strlen(P_message));
        sm4_setkey_enc(&ctxt, User.key);
        sm4_crypt_ecb(&ctxt, 1, P_m_len, P_message, C_message);                
//         printf("C %d\n",strlen(C_message)); 
        
        
        if(strlen(C_message)%16!=0) 
        {
            printf("error enc\n");
            printf("input your message again\n");
                continue;
        }
        for(i=0;i<P_m_len;i++,P_message[i]=0);
        
		int nRecv = send(sock, C_message, strlen(C_message), 0);
//         printf("S %d\n",nRecv);
        for(i=0;i<C_m_len;i++,C_message[i]=0);
	}
	return 0;
}

DWORD WINAPI recv_msg(LPVOID lpParam)
{    
    int i;
	int sock = *((int*)lpParam);
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len;
	while (1)
	{
		str_len = recv(sock, name_msg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (str_len == -1)
			return -1;
		name_msg[str_len] = 0;
        
        sm4_setkey_dec(&ctxt,User.key);
        sm4_crypt_ecb(&ctxt, 0, str_len, name_msg, name_msg);
        if(name_msg[0]==0x4)
        {
            name_msg[0]='|';            
            for(i=0;i<str_len;i++)
            {
                printf("%c",name_msg[i]);
                if(name_msg[i]=='\n')
                    break;
            }
        }
	}
	return 0;
}



 
 
 
 
 
 
 
 
 
 
 
 

 
