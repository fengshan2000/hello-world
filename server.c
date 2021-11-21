#undef UNICODE
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#define MAX_CLNT 256    
#define max_user 10
#define Nonce_Len 16
#define Date_Len 11

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>
#include<gmp.h>
#include "sm3.h"
#include "sm4.h"

time_t  t;
struct tm *tmp;




// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

HANDLE g_hEvent;
int clnt_cnt = 0;			//统计套接字
int user_cnt = 0;           //统计注册用户个数
int clnt_socks[MAX_CLNT];	//管理套接字
unsigned char clnt_key[MAX_CLNT][16];//管理对应套接字的密钥
HANDLE hThread[MAX_CLNT];	//管理线程

DWORD WINAPI ThreadProc(LPVOID lpParam);

struct user{
    char name[10];
    unsigned char passwordHash[32];
    char state;
    char signature[20];
    unsigned char key[16] ;
} userList[10];


// 10 64 5 20 32 106 25 106 10 116 58 58 ----------
void userprint()
{
    printf("\n---------------------------------------------------------------user-list---------------------------------------------------------------\n");
    printf("user number:%d\n",user_cnt);
    int i,j;
    for(j=0;j<user_cnt;j++)
    {
        printf("|");
        for(i=0;i<10;i++)
            printf("%c",userList[j].name[i]);
        for(i=0;i<32;i++)
            printf("%02X",userList[j].passwordHash[i]);
        printf("%5d",userList[j].state);
        printf(" %20s",userList[j].signature);
        printf("");
        for(i=0;i<16;i++)
            printf("%02X",userList[j].key[i]);    
        printf("|\n");
        
    }
    printf("---------------------------------------------------------------user-list---------------------------------------------------------------\n");
}

int main()
{    
    int ServerTest() ;    
    int i;
    user_cnt++;
    char n[10]="fengshan";
    for(i=0;i<(int)strlen(n);i++)        
        userList[0].name[i]=n[i];
    sm3(n,strlen(n),userList[0].passwordHash   );
    userList[0].state=0;    
    
    userprint();    
    ServerTest();
    
    
    return 0;
}


int ServerTest() 
{
    WSADATA wsaData;
    int iResult;
    int i;
    
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    
    DWORD dwThreadId;	/*线程ID*/
    struct sockaddr_in remoteAddr;
    int nAddrLen = sizeof(remoteAddr);
    
//  关键
    while(1){
    
        ClientSocket = accept(ListenSocket, (SOCKADDR*)&remoteAddr, &nAddrLen);    
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        WaitForSingleObject(g_hEvent, INFINITE);
        hThread[clnt_cnt] = CreateThread(
			NULL,		// 默认安全属性
			0,		// 默认堆栈大小
			ThreadProc,	// 线程入口地址（执行线程的函数）
			(void*)&ClientSocket,		// 传给函数的参数
			0,		// 指定线程立即运行
			&dwThreadId);	// 返回线程的ID号
		clnt_socks[clnt_cnt++] = ClientSocket;
        
		SetEvent(g_hEvent);				/*设置受信*/
        
        printf(" Receive a connect:%s thread ID:%d\r\n", inet_ntoa(remoteAddr.sin_addr), dwThreadId);
        
    }
   WaitForMultipleObjects(clnt_cnt, hThread, TRUE, INFINITE);
    
   
    iResult = shutdown(ClientSocket, SD_SEND);
    
    
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}


#define User_login 0x01
#define User_logon 0x02


// 用户存在返回1

int SearchName(char *recvbuf,int iResult)
{   
    int i;
    char tem[10];
    for(i=0;i<iResult;i++)
    {        
        tem[i]=recvbuf[i];        
    }
    tem[i]=0;
    for(i=0;i<user_cnt;i++)
    {                 
        if(lstrcmp(tem,userList[i].name)==0)
            return 1;
    }
    
    
    return 0;
}


    
int CheckUser(SOCKET ClientSocket,unsigned char recvbuf[DEFAULT_BUFLEN],int iResult) 
{

    int i;
    char UserExist;
    char temName[10];
    for(i=2;i<iResult;i++)
        temName[i-2]=recvbuf[i];
    temName[i-2]=0x00;    
            
    int SearchName(char *recvbuf,int iResult);                    
    
            UserExist=recvbuf[1];
            if(UserExist==User_login)
            {
                printf("User:%s want to login ",temName);
            }
            else if(UserExist==User_logon)
            {
                printf("User:%s want to logon ",temName);
            }
    //         用户名长度不符合规则，返回send0 ，重新输入，continue            
    //             用户名再不在对应的变量中
            if(!SearchName(temName,iResult-2)&&(UserExist==User_login))
            {                
                iResult=send(ClientSocket,"03",3,0); 
                printf(",but userser:%s doesn't exist!\n",temName);
                return 1;
            }            
            //             用户名在对应的变量中
            
            if(SearchName(temName,iResult-2)&&(UserExist==User_logon))
            {                
                iResult=send(ClientSocket,"04",3,0);                
                printf(",and userser:%s has exist!\n",temName);
                return 1;
            }                        
            iResult=send(ClientSocket,"01",3,0);              
            printf(" correct \n",temName);
        
    return 0;
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

int cmp(unsigned char output[],unsigned char output1[],int min)
{
    int i;
    
    
    
    for(i=0;i<min;i++)
        if((output[i]==output1[i]))
        {
            continue;
            
        }
        else
        {
            
            return 1;
        }
    return 0;
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

int checkUserPassword(SOCKET ClientSocket,unsigned char recvbuf[DEFAULT_BUFLEN],int iResult,struct user *User) 
{    
    int i;   
    int j;
    int I;
    int recvbuflen;
    char namelen;
    
    unsigned char sendbuf[DEFAULT_BUFLEN];
    unsigned char Nc[Nonce_Len];
    unsigned char Ns[Nonce_Len];    
    unsigned char Dc[Date_Len];
    unsigned char Dc2[Date_Len];
    unsigned char Ds[Date_Len];
    unsigned char username[10];
    
    unsigned char output[32];    
    unsigned char output1[32];
    
    sm3_context ctx;
//     step 2 2
    namelen=recvbuf[2];
    
    for(i=0;i<namelen;i++)
        username[i]=recvbuf[i+3];
    username[i]=0x0;
    for(i=0;i<namelen;i++)
        (*User).name[i]=recvbuf[i+3];
    (*User).name[i]=0x0;
    
    
    printf("%d|%s logining--\n",namelen, username);
    
    
    
    
    for(i=0;i<Nonce_Len;i++)
    {   
        Nc[i]=recvbuf[i+3+namelen];
    }
   
    
    for(i=0;i<Date_Len;i++)
    {   
        Dc[i]=recvbuf[i+3+namelen+Nonce_Len];
    }
    for(i=0;i<32;i++)
    {
        output1[i]=recvbuf[i+3+namelen+Nonce_Len+Date_Len];  
    }      
    
    
    sm3_starts( &ctx );
    sm3_update( &ctx, Nc, Nonce_Len );
    sm3_update( &ctx, Dc, Date_Len );
    for(i=0;i<user_cnt;i++)
    {                
        if(strlen(userList[i].name)!=namelen)
            continue;
        if(cmp(username,userList[i].name,namelen)==0)
        {
            
            I=i;                      
            sm3_update(&ctx, userList[i].passwordHash, 32 );    
            break;
        }
    }
    sm3_finish( &ctx, output );    
    memset( &ctx, 0, sizeof( sm3_context ) );
    printf("\n");
    
    
    if(cmp(output1,output,32)!=0)
    {
        printf("error121\n");
        sendbuf[0]=0x1;
        sendbuf[1]=0x0;
        send(ClientSocket,sendbuf,2,0);
        return 1;
    }
    
//     step2 2 checkpass 生成 Ns Ds 
    Nonce(Ns);
    Nonce(Ns);
    t = time(NULL);
	tmp = localtime(&t);
	strftime(Ds,sizeof(Ds),"%Y%m%H%M%S",tmp);

    
    sendbuf[0]=0x01;
    sendbuf[1]=0x02;
    
    sm3_starts( &ctx );
    sm3_update( &ctx, Ns, Nonce_Len );
    sm3_update( &ctx, Ds, Date_Len );
    sm3_update(&ctx, userList[I].passwordHash, 32 );    
    sm3_finish( &ctx, output);
    memset( &ctx, 0, sizeof( sm3_context ) );
        
    for(i=0;i<Nonce_Len;i++)
        sendbuf[i+2]=Ns[i];
    for(i=0;i<Date_Len;i++)
        sendbuf[i+2+Nonce_Len]=Ds[i];
    for(i=0;i<32;i++)
        sendbuf[i+2+Nonce_Len+Date_Len]=output[i];
    
    sm3_starts( &ctx );
    sm3_update( &ctx, Nc, Nonce_Len );
    sm3_update( &ctx, Ds, Date_Len );
    sm3_update(&ctx, userList[I].passwordHash, 32 );   
    sm3_finish( &ctx, output);
    memset( &ctx, 0, sizeof( sm3_context ) );
          
    for(i=0;i<32;i++)
        sendbuf[i+2+Nonce_Len+Date_Len+32]=output[i];
    
    
    iResult = send(ClientSocket, sendbuf,2+Nonce_Len+Date_Len+32+32 , 0);       
    for(i=0;i<iResult;i++) sendbuf[i]=0;
    
    while(1)
    {    
        iResult=recv(ClientSocket, recvbuf, 45, 0);
        if(iResult>0)
        {            
            if(recvbuf[1]==0x0)
            {
                printf("error13");
                return 1;            
            }
            
            for(i=0;i<Date_Len;i++)
                Dc2[i]=recvbuf[i+2];
            for(i=0;i<32;i++)
                output1[i]=recvbuf[i+2+Date_Len];                        
            
//             printf("\nrecv");
//             printf("\n||%d||\n",iResult);
//             for(i=0;i<iResult;i++)
//                 printf("%02X",recvbuf[i]);
//             printf("\n");
//             for(i=0;i<32;i++)
//                 printf("%02X",output1[i]);
//             printf("\n");
//             
            sm3_starts( &ctx );
            sm3_update( &ctx, Ns, Nonce_Len );
            sm3_update( &ctx, Dc2, Date_Len );
            sm3_update( &ctx, userList[I].passwordHash,32);
            sm3_finish( &ctx, output );
            memset( &ctx, 0, sizeof( sm3_context ) );
// //             
            
            
            /*
            for(i=0;i<32;i++)
                printf("%02X",output[i]);
            printf("\n");*/
    
    //         check output1  sm3(ns,dc2,ph)
            if(cmp(output1,output,32)!=0)
            {
                printf("error14");
                sendbuf[0]=0x1;
                sendbuf[1]=0x0;
                send(ClientSocket,sendbuf,2,0);                
                return 1;
            }
            printf("login successful\n");
            sendbuf[0]=0x1;
            sendbuf[1]=0x4;
            send(ClientSocket,sendbuf,2,0);
            userList[I].state=0x1;            
            break;
        }
        
        if(iResult<0)
        {
            return 1;
        }
    }
//     生成共同的会话key=sm3(nc,ns,dc,ds,ph);
    
    sm3_starts( &ctx );
    sm3_update( &ctx, Nc, Nonce_Len );
    sm3_update( &ctx, Ns, Nonce_Len );
    sm3_update( &ctx, Dc, Date_Len );
    sm3_update( &ctx, Ds, Date_Len );
    sm3_update( &ctx, userList[I].passwordHash,32);
    sm3_finish( &ctx, output );
    memset( &ctx, 0, sizeof( sm3_context ) );
    
    userprint();
    for(j=0;j<clnt_cnt;j++)
    {
        if(clnt_socks[j]==ClientSocket)
        {
            for(i=0;i<16;i++)
            {
                userList[I].key[i]=output[2*i+1];
                clnt_key[j][i]=output[2*i+1];
            }
        }
    }
    return 0;
}
int Userlogon(SOCKET ClientSocket,unsigned char recvbuf[DEFAULT_BUFLEN],int iResult) 
{
    unsigned char username[10];
    unsigned char passwH[32];
    unsigned char cipher[1024];
    unsigned char message[1024];
    
    int i;
    int namelen;
    int recvbuflen = DEFAULT_BUFLEN;
    namelen=recvbuf[2];
    for(i=0;i<10;i++) username[i]=0;
    for(i=0;i<namelen;i++)
        username[i]=recvbuf[i+3];
    printf("\n %s logoning--",username,namelen);
    unsigned char sendbuf[DEFAULT_BUFLEN];
            
    const unsigned char n[]="740871580039435875416481692623095113551379237179182409736093022131302372640271466807517581405135410635208762336965594083371251919534075081517705320726861129138311795549086043497723420750352122968101791948696008732315609861839334969981559595996817383345885558324816030398605700779439437932125609930105188525740764834385480069244073639844500300955054271360099884610521497722664615680800165793826406334697422849228860651527744111443506482398788603688419149385339092285793197555302733689460153107080218327714584261433556398817550309786621694475109133528646539602685737510810170827284740016113731216898983801371584104339";
    const unsigned char e[]="41670006189533811163938388341593655751451438352185359167526922251981983038319205294758008353381863069971066369595699300887686245872550171388666803065497317765004896743027521175924529757544576105836965372348249084001006844253033018638001542430913093553091361856299664354527382003652197024252807039772032662893479075565180308558785439186429014915166284131509852931990853333827653069989390299376072756758304503485705465136313620194965250643945149441541092428786780132070340448001160604406161080185126260281024258222883009079660014808761889563019210106492617060641842034908363965739282477409781257111187158751715026281";
    const unsigned char d[]="672664113662288040056929054619982634981627396488723301700770487896525120967232104509716917659234242467022413517214493219826393858679273744515987428549556192542152969081873085895945613554783952550428549821767201482104015286789243359544551229664609761608396227415953223405971492766507565700288270244203769988280825060777837212286702944567065141722371398898445204245608230301135297156329946040207842291568352529421791662721502345254399415734986479622625563014975229822863114618305255156134945597503633129962345248113193832504212760092496019011346727898839877083968690488775857310960939815544506925369735110092635059153";
    
    mpz_t n_m,e_m,d_m,message_m,cipher_m;
    mpz_init_set_str(n_m,n,10);
    mpz_init_set_str(e_m,e,10);
    mpz_init_set_str(d_m,d,10);
    mpz_init(cipher_m);    
    
    
    
    sendbuf[0]=0x2;
    sendbuf[1]=0x2;
    sendbuf[2]=0x1; //n
    for(i=0;i<strlen(n);i++)
        sendbuf[i+3]=n[i];
    iResult=send(ClientSocket,sendbuf,3+strlen(n),0);
    
    
    sendbuf[0]=0x2;
    sendbuf[1]=0x2;
    sendbuf[2]=0x2; //e
    for(i=0;i<strlen(n);i++)
        sendbuf[i+3]=e[i];
    iResult=send(ClientSocket,sendbuf,3+strlen(e),0);
    Sleep(233);
    while(1)    
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if(iResult>100)
        {            
            for(i=0;i<iResult-2;i++)
            {
                if(0x30<=recvbuf[i+2]&&recvbuf[i+2]<0x3A)
                    cipher[i]=recvbuf[i+2];                
                else
                {
                    cipher[i]=0x0;
                    break;
                }
            }            
            mpz_init_set_str(cipher_m,cipher,10);            
            break;
        }
        if(iResult<0)
            break;
            
    }
//         step4 decodes
    Encrypt(&message_m,n_m,cipher_m,d_m);    
    mpz_get_str(message,16,message_m);        
    if(strlen(message)==1)
    {
        sendbuf[0]=0x2;
        sendbuf[1]=0x0;
        iResult=send(ClientSocket,sendbuf,2,0);
        return 1;
    }
    else
    {
        sendbuf[0]=0x2;
        sendbuf[1]=0x1;
        iResult=send(ClientSocket,sendbuf,2,0);
    }
    
    unsigned char chartoint(char i);

    for(i=0;i<32;i++)
    {
        passwH[i]=(unsigned char)((chartoint(message[2*i])<<4)&0b11110000)|(chartoint(message[2*i+1])&0b00001111);        
    }                    
    
    
    for(i=0;i<32;i++)
        userList[user_cnt].passwordHash[i]=passwH[i];
    for(i=0;i<namelen;i++)
        userList[user_cnt].name[i]=username[i];
    user_cnt++;
    
    
    printf("logon end\n");
    return 0;
}
unsigned char chartoint(char i)
{
    if(i<=0x39&&i>=0x30)
        return (unsigned char)(i-0x30);
    else
        return (unsigned char)(i+9-0x40);
}

void send_msg(char* msg, int len,int ClientSocket);

DWORD WINAPI ThreadProc(LPVOID lpParam)
{    
    int ClientSocket = *((int*)lpParam);
    unsigned char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    int i,j;
    
    
    struct user userInThread;    
    do{
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if(iResult>0)
        {            
            printf("--------------");
            if(recvbuf[0]==0x00)        
                CheckUser(ClientSocket,recvbuf,iResult);
            if(recvbuf[0]==0x01&&recvbuf[1]==0x01)
            {
                i=checkUserPassword(ClientSocket,recvbuf,iResult,&userInThread);
                if(i==0)
                {
                    printf("\nUser:%s login\n",userInThread.name);
                    break;                    
                }
            }
            if(recvbuf[0]==0x02&&recvbuf[1]==0x01)
            {
                Userlogon(ClientSocket,recvbuf,iResult);                
            } 
            
        }   
        
        
        /*等待内核事件对象状态受信*/
        if(iResult<0)            
        {
            printf("End thread of%d\n", GetCurrentThreadId());
            break;
        }
    }while(1);
    unsigned char C_message[DEFAULT_BUFLEN];
    unsigned char P_message[DEFAULT_BUFLEN];
    unsigned char key[16];
    sm4_context ctxt;
    int P_m_len;
    while ((iResult = recv(ClientSocket, recvbuf, sizeof(recvbuf), 0)) != -1)
        {
            if(iResult>0)
            {
                if(recvbuf[0]==7)
                {
                    printf("%s|End thread of%d\n",userInThread.name, GetCurrentThreadId());
                    for(i=0;i<user_cnt;i++)
                    {                 
                        if(lstrcmp(userInThread.name,userList[i].name)==0)
                        {                               
                            userList[i].state=0;
                            break;
                        }
                    }
                    userprint();
                }            
                        
                for(i=0;i<iResult;i++)
                    C_message[i]=recvbuf[i];
            
            for(i=0;i<clnt_cnt;i++)
            {
                if(clnt_socks[i]==ClientSocket)
                {                    
                    
                    sm4_setkey_dec(&ctxt, clnt_key[i]);
                    sm4_crypt_ecb(&ctxt, 0, iResult, C_message, P_message);
                }
            }
            P_m_len=strlen(P_message);            
//             printf("%s|%d",P_message,strlen(P_message));
//             printf("\n");    
            send_msg(P_message,iResult,ClientSocket);            
            }
        }        
    
    WaitForSingleObject(g_hEvent, INFINITE);
    for (i = 0; i < clnt_cnt; i++)
	{
		if (ClientSocket == clnt_socks[i])
		{
			while (i++ < clnt_cnt - 1)
            {
                clnt_socks[i] = clnt_socks[i + 1];
                for(j=0;j<16;j++)
                    clnt_key[i][j]=clnt_key[i+1][j];
            }
			break;
		}
	}
	
	clnt_cnt--;
	SetEvent(g_hEvent);		/*设置受信*/
	// 关闭同客户端的连接
	closesocket(ClientSocket);
	return 0;
    
    
}


void send_msg(char* msg, int len,int ClientSocket)
{
    unsigned char C_message[DEFAULT_BUFLEN];
	int i,j;  
    int C_m_len;
    sm4_context ctxt;
	/*等待内核事件对象状态受信*/
	WaitForSingleObject(g_hEvent, INFINITE);
// 	printf("\nclnt_cnt %d|",clnt_cnt);
    for (i = 0; i < clnt_cnt; i++)
    {                
//             printf("%d ",i);
            sm4_setkey_enc(&ctxt, clnt_key[i]);
            sm4_crypt_ecb(&ctxt, 0,strlen(msg), msg, C_message);
            C_m_len=strlen(C_message);
            send(clnt_socks[i], C_message, strlen(C_message), 0);                    
            
            for(j=0;i<C_m_len;j++,C_message[j]=0);
    }
	SetEvent(g_hEvent);		/*设置受信*/
}
