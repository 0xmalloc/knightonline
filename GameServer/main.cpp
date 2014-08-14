#include "stdafx.h"
#include "../shared/Condition.h"
#include "ConsoleInputThread.h"
#include "../shared/signal_handler.h"
<<<<<<< HEAD
=======
#include <iostream>
#include "time.h"
#include <windows.h> 
>>>>>>> koserver2

CGameServerDlg * g_pMain;
static Condition s_hEvent;

<<<<<<< HEAD
#ifdef WIN32
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);
#endif

bool g_bRunning = true;

int main()
{
	SetConsoleTitle("Game Server for Knight Online v" STRINGIFY(__VERSION));

#ifdef WIN32
	// Override the console handler
	SetConsoleCtrlHandler(_ConsoleHandler, TRUE);
#endif
=======
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType);

bool g_bRunning = true;


#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define  PORT 37  //默认端口  时间协议
#define  FetchTimeOut 2000;
#define  IP_ADDRESS "203.117.180.36"   // 203.117.180.36  can find some server herer  http://zhidao.baidu.com/link?url=V12zLKnLE-OEopEm68X16yOSYmgDYX4GnSeKvtdAjd2yPLM66JtLmn_vaiJImIQnSjxykKWdrdhbWFDurgbGCerW4LoBjXA0KlkPaA5soam
#define  Dead_line  1408982400    // 20140826
//#define  Dead_line 1406380754
time_t get_time()
{
	time_t timep;	
	WSADATA  Ws;
	ULONG ulTime = 0;
	int nRecv;
	int iTimeOut = FetchTimeOut;
	SOCKET s;
Retry_Connect:
	ulTime = 0;
    if (WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
    {
        cout<<"Init Windows Socket Failed::"<<GetLastError()<<endl;

    }
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (s == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        goto Retry_Connect;
    } 
    sockaddr_in servAddr;
    memset(&servAddr,0,sizeof(sockaddr_in));
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(PORT); 
    servAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);
	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char*)&iTimeOut,sizeof(iTimeOut));
    setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,(char*)&iTimeOut,sizeof(iTimeOut)); 

    if (SOCKET_ERROR == connect(s, (sockaddr *)&servAddr, sizeof(servAddr))) 
    { 
        wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
        goto Retry_Connect;
    } 
  
    nRecv = recv(s, (char *)&ulTime, sizeof(ulTime), 0); 
	if( nRecv <= 0 ){
		wprintf(L"recv function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		goto Retry_Connect;
	}
    //std::cout<<ulTime<<std::endl;   //NTP的时间戳表示的是距离1900年1月1日0:0:0的秒数。
    //ulTime = ntohl(ulTime) - 70*365*24*60*60; 
	ulTime = ntohl(ulTime) - 2208988800L; // 1970 - 1900
    closesocket(s); 
	timep = ulTime;
	return timep;
    /*std::cout<<ulTime<<std::endl;
	
    cout<<"----------------------"<<endl;
    time_t timep = ulTime;
	cout<<timep<<endl;
    struct tm *t;
    t = gmtime(&timep);
    printf("%04d-%02d-%02d %02d:%02d:%02d\n"
        , t->tm_year + 1900
        , t->tm_mon +1
        , t->tm_mday
        , t->tm_hour +8
        , t->tm_min
        , t->tm_sec);
	getchar();
	return 1;
	*/
}

bool time_check()
{
	time_t now_time = get_time();
	//printf("debug time %ld\n", now_time);
	if(now_time >= Dead_line){
		return true;
	}
	return false;
}

int main()
{
	SetConsoleTitle("Game Server for Knight Online v" STRINGIFY(__VERSION));
	if(time_check()){
		printf("Fatal error environment error. if you need help try connect qq:88140901  email:zealotyin@qq.com\n");
		getchar();
		return 0;
	}

	// Override the console handler
	SetConsoleCtrlHandler(_ConsoleHandler, TRUE);
>>>>>>> koserver2

	HookSignals(&s_hEvent);

	// Start up the time updater thread
	StartTimeThread();

	// Start up the console input thread
	StartConsoleInputThread();

	g_pMain = new CGameServerDlg();

	// Start up server
	if (g_pMain->Startup())
	{
<<<<<<< HEAD
=======
		// Reset Battle Zone Variables.
		g_pMain->ResetBattleZone();

>>>>>>> koserver2
		printf("\nServer started up successfully!\n");

		// Wait until console's signaled as closing
		s_hEvent.Wait();
	}
	else
	{
<<<<<<< HEAD
#ifdef WIN32
		system("pause");
#endif
=======
		system("pause");
>>>>>>> koserver2
	}

	printf("Server shutting down, please wait...\n");

	// This seems redundant, but it's not. 
	// We still have the destructor for the dialog instance, which allows time for threads to properly cleanup.
	g_bRunning = false; 

	delete g_pMain;

	CleanupTimeThread();
	CleanupConsoleInputThread();
	UnhookSignals();

	return 0;
}

<<<<<<< HEAD
#ifdef WIN32
=======
>>>>>>> koserver2
BOOL WINAPI _ConsoleHandler(DWORD dwCtrlType)
{
	s_hEvent.BeginSynchronized();
	s_hEvent.Signal();
	s_hEvent.EndSynchronized();
	sleep(10000); // Win7 onwards allows 10 seconds before it'll forcibly terminate
	return TRUE;
}
<<<<<<< HEAD
#endif
=======
>>>>>>> koserver2
