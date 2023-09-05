#undef UNICODE
#pragma warning(disable : 4996)
// 引发 C4996 警告的代码
#define WIN32_LEAN_AND_MEAN
#ifndef MY_HEADER_FILE_H
#define MY_HEADER_FILE_H


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <functional>
#include <string>
#include <filesystem>
#include <io.h>
#include <fcntl.h>
 
#endif // MY_HEADER_FILE_H


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define CONSULT_PORT "6666"

class FileTransfer {
private:
    
    WSADATA wsaData;
    int iResult;
    struct addrinfo* result ;
    struct addrinfo hints;
    SOCKET ConnectSocket = INVALID_SOCKET;//已建立的连接
    int portNum[8];//端口号
    long int totalLen;//文件 总长度
    int haveRecvLen = 0;//接收到的总长度
public:
    const char* port ;
    char ipAddress[DEFAULT_BUFLEN] = { 0 };
    int iconnect = 0;//连接状态
public:
    ~FileTransfer();
    CStatic* pStaticText = NULL;//文本句柄
    CStatic* pStaticConnectText = NULL;//连接状态句柄
    int initialize();
    int Listen();
    int receive(const char* filename,int port);//接收
    int connectToServer(const char* hostname, const char* port);//连接的ip:port
    void showIPandPort(addrinfo* ptr);
    bool GetPeerAddressAndPort(SOCKET connectSocket, char* ipAddress, int ipAddressSize, char* port, int portSize);
    int sendData(const char* filePath, int port, int index, int subLen);//发送

    void AddTextToStaticControl(const CString& newText);//添加文本
    long getFileSize(FILE* file);
    //多线程文件传输
    void muliThreadTrans(char* filePath);
     
    
    // 假设 progressControlWnd 是一个有效的进度条指针
    CProgressCtrl* pProgressCtrl;
};

//非成员函数
void writeToLogFile(const std::string& text);
 

 