#undef UNICODE
#pragma warning(disable : 4996)
// ���� C4996 ����Ĵ���
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
    SOCKET ConnectSocket = INVALID_SOCKET;//�ѽ���������
    int portNum[8];//�˿ں�
    long int totalLen;//�ļ� �ܳ���
    int haveRecvLen = 0;//���յ����ܳ���
public:
    const char* port ;
    char ipAddress[DEFAULT_BUFLEN] = { 0 };
    int iconnect = 0;//����״̬
public:
    ~FileTransfer();
    CStatic* pStaticText = NULL;//�ı����
    CStatic* pStaticConnectText = NULL;//����״̬���
    int initialize();
    int Listen();
    int receive(const char* filename,int port);//����
    int connectToServer(const char* hostname, const char* port);//���ӵ�ip:port
    void showIPandPort(addrinfo* ptr);
    bool GetPeerAddressAndPort(SOCKET connectSocket, char* ipAddress, int ipAddressSize, char* port, int portSize);
    int sendData(const char* filePath, int port, int index, int subLen);//����

    void AddTextToStaticControl(const CString& newText);//����ı�
    long getFileSize(FILE* file);
    //���߳��ļ�����
    void muliThreadTrans(char* filePath);
     
    
    // ���� progressControlWnd ��һ����Ч�Ľ�����ָ��
    CProgressCtrl* pProgressCtrl;
};

//�ǳ�Ա����
void writeToLogFile(const std::string& text);
 

 