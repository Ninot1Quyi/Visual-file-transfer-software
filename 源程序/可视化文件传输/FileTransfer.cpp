#pragma warning(disable : 4996)
// ���� C4996 ����Ĵ���


#include "pch.h"
#include "FileTransfer.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <mutex>

std::mutex mtx;  // ���廥����
void removeNewline(char* str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
void addNewline(char* str) {
    int len = strlen(str);
    str[len] = '\n';
    str[len + 1] = '\0';
}
void writeToLogFile(const std::string& text) {
    //std::ofstream file("log.txt", std::ios::app);
    //if (!file) {
    //    // �ļ������ڣ��������ļ�
    //    file.open("log.txt");
    //    if (!file) {
    //        std::cout << "�޷������ļ���" << std::endl;
    //        return;
    //    }
    //}

    //// ��ȡ��ǰʱ��
    //std::time_t currentTime = std::time(nullptr);
    //std::tm localTime{};
    //localtime_s(&localTime, &currentTime);
    //char timeString[100];
    //std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &localTime);

    //// д��ʱ����ı�
    //file << timeString << std::endl;
    //file << text << std::endl;

    //// �ر��ļ�
    //file.close();

    //std::cout << "д��ɹ���" << std::endl;
}

 
void FileTransfer::AddTextToStaticControl(const CString& newText)
{
    if (pStaticText != nullptr)
    {
        // ��ȡ��ǰ���ı�����
        CString strText;
        pStaticText->GetWindowText(strText);

        // ����µ�һ���ı�
        strText += _T("\r\n") + newText;

        // �����µ��ı�����
        pStaticText->SetWindowText(strText);
    }
}

void FileTransfer::showIPandPort(addrinfo* ptr)
{
    // �� sockaddr ת��Ϊ sockaddr_in �ṹ��
    sockaddr_in* addr_in = (sockaddr_in*)ptr->ai_addr;

    // �� IP ��ַת��Ϊ�ַ�������ӡ
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("IP address: %s\n", ip_str);

    // ���˿ںŴ�ӡ
    printf("Port number: %d\n", ntohs(addr_in->sin_port));
}
int FileTransfer::initialize()
{
    int tmp[8]={9990,9991,9992,9993,9994,9995,9996,9997};
    for (int i = 0; i < 8; i++) {
        this->portNum[i] = tmp[i];// ��ʼ�����̶߳˿ں�
    }
    int iResult;


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    // ������������ַ�Ͷ˿�
    iResult = getaddrinfo(NULL, CONSULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    return 0;
}
 
int FileTransfer::Listen()
{
    int iResult = 0;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    
 
    // Create a SOCKET for the server to listen for client connections.
    writeToLogFile("server:����ListenSocket�׽���");
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    writeToLogFile("server:��ListenSocket�׽���");
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    writeToLogFile("server:��ʼ����");
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    writeToLogFile("server:�ȴ�����");
    ClientSocket = accept(ListenSocket, NULL, NULL);

    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    writeToLogFile("server:�����ѽ�����");
    this->iconnect = 1;
    pStaticConnectText->SetWindowText(_T("������"));
    // No longer need server socket
    closesocket(ListenSocket);


    //���ӽ���֮��Ӧ�ÿ�ʼЭ��=================
    //char recvbuf[DEFAULT_BUFLEN] = { 0 };
    char filenameBuffer[DEFAULT_BUFLEN] = { 0 };
    char fileLenBuffer[DEFAULT_BUFLEN] = { 0 };
    char threadNumBuffer[DEFAULT_BUFLEN] = { 0 };
    char* recvStr = "hjy";
    int recvbuflen = DEFAULT_BUFLEN;

    //�����ļ���
    iResult = recv(ClientSocket, filenameBuffer, recvbuflen, 0);
    removeNewline(filenameBuffer);
    if (iResult > 0) {
		writeToLogFile("server:���յ��ļ���"+std::string(filenameBuffer));
        send(ClientSocket, recvStr, (int)strlen(recvStr), 0);//ͬ��
        //�����ļ�����
        iResult = recv(ClientSocket, fileLenBuffer, recvbuflen, 0);
        removeNewline(fileLenBuffer);
        if (iResult > 0) {
            writeToLogFile("server:���յ��ļ�����" + std::string(fileLenBuffer));
            send(ClientSocket, recvStr, (int)strlen(recvStr), 0);//ͬ��

            //�����߳���
            iResult = recv(ClientSocket, threadNumBuffer, recvbuflen, 0);
            removeNewline(threadNumBuffer);
            if (iResult > 0) {
				writeToLogFile("server:���յ��߳���" + std::string(threadNumBuffer));
                int threadNum = std::stoi(threadNumBuffer); // ���ַ���ת��Ϊ����
                //��ʼ����
                writeToLogFile("server:�ȴ�����...TODO");
                FILE* fp = NULL; // �������ļ�
                if (fopen_s(&fp, filenameBuffer, "wb") != 0) {
                    // ���ļ�ʧ��
                    writeToLogFile("�޷����ļ�\n");
                    return 1;
                }
                else {
                    writeToLogFile("�ɹ����ļ�" + std::string(filenameBuffer));
                    // ���ļ�ָ���ƶ���ָ��λ��
                    long fileLen = atol(fileLenBuffer); // ת��Ϊ������
                    if (fseek(fp, fileLen - 1, SEEK_SET) != 0) {
                        printf("�޷��ƶ��ļ�ָ��\n");
                        fclose(fp);
                        return 1;
                    }
                    // д��һ���ֽڣ����ļ�������չ��ָ������
                    if (fwrite("", 1, 1, fp) != 1) {
                        printf("�޷�д���ļ�\n");
                        fclose(fp);
                        return 1;
                    }
                    fclose(fp);
                }
                //fopen_s(&fp, filenameBuffer, "wb");
                std::vector<std::thread> threadQueue;

                // �����߳�
                for (int i = 0; i < threadNum; i++) {
                    threadQueue.emplace_back(&FileTransfer::receive, this, filenameBuffer, portNum[i]);
                }
                 
                send(ClientSocket, recvStr, (int)strlen(recvStr), 0); // ͬ��
                writeToLogFile("server:��ͬ��");
                
              
               
                
                // �ȴ������߳�ִ�����
                for (auto& threadObj : threadQueue) {
                    if (threadObj.joinable()) {
                        threadObj.join();
                    }
                }

                fclose(fp);

            }
            else {
				writeToLogFile("server:�����߳���ʧ��");
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
            }
        }
        else {
            writeToLogFile("server:�����ļ�����ʧ��");
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

	}
    else {
		writeToLogFile("server:�����ļ���ʧ��");
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

 
    return 0;
}
int FileTransfer::receive(const char* filename,int port)//����
{
    writeToLogFile("server:ִ��receive"+std::to_string(port));
    int iResult = 0;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    // ������������ַ�Ͷ˿�
    struct addrinfo* result;
    iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
    if (iResult != 0) {
        writeToLogFile("getaddrinfo failed with error: %d " + std::to_string(port));
        WSACleanup();
        return 1;
    }
  
    writeToLogFile("server:���̴߳���ListenSocket�׽���"+std::to_string(port));
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        writeToLogFile("socket failed with error: %ld\n");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    writeToLogFile("server:���̰߳�ListenSocket�׽���" + std::to_string(port));
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        writeToLogFile("bind failed with error: %d\n");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    writeToLogFile("server:���߳̿�ʼ����" + std::to_string(port));
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        writeToLogFile("listen failed with error: %d\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

   
    writeToLogFile("server:���̵߳ȴ�����" + std::to_string(port));
    ClientSocket = accept(ListenSocket, NULL, NULL);

    if (ClientSocket == INVALID_SOCKET) {
        writeToLogFile("accept failed with error: %d\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    writeToLogFile("server:���߳������ѽ�����" + std::to_string(port));
    pStaticConnectText->SetWindowText(_T("������"));
    // No longer need server socket
    closesocket(ListenSocket);

    //// �����׽���Ϊ������ģʽ
    //u_long mode = 1;
    //if (ioctlsocket(ClientSocket, FIONBIO, &mode) != 0) {
    //    std::cerr << "Failed to set socket to non-blocking mode." << std::endl;
    //    closesocket(sock);
    //    WSACleanup();
    //    return 1;
    //}

    char recvbuf[DEFAULT_BUFLEN] = { 0 };
    char indexbuf[DEFAULT_BUFLEN] = { 0 };
    int recvbuflen = DEFAULT_BUFLEN;
    int fileLen = 0;
    

    int haveRecvLen = 0;

        
    
    writeToLogFile("server:�ȴ������ļ��ֽ��±�..." + std::to_string(port));
    recv(ClientSocket, indexbuf, recvbuflen, 0);//�����ļ��±�
    removeNewline(indexbuf);
    writeToLogFile("server:���յ����ļ��ֽ��±�  " + std::string(indexbuf) +"  " + std::to_string(port));
    send(ClientSocket, "hjy", (int)strlen("hjy"), 0);//ͬ��
    
    
    long int fileIndex = atoi(indexbuf);
    writeToLogFile("���������ļ����֣�" + std::string(filename));
    //���ļ���ѭ�����ղ�д��
    int fd = _open(filename, _O_WRONLY | _O_BINARY);
    __int64 newPosition = _lseeki64(fd, fileIndex, SEEK_SET);  // �����ļ�ָ��λ��Ϊƫ�Ƶ�ַ
    if (fd == -1) {
        // �����ļ���ʧ�ܵ����
    }
    else {//�ļ����Դ�
        int i = 0;
         while ((iResult = recv(ClientSocket, recvbuf, recvbuflen, 0)) > 0)
        {
        
        
            haveRecvLen += iResult;//��¼���յ����ļ�����
            writeToLogFile("server:TCP���յ�����! ����="+std::to_string(iResult));

         
            writeToLogFile("д��... "+std::string(recvbuf));
            int bytesWritten = _write(fd, recvbuf, iResult);
            if (bytesWritten > 0) {
                // д��ɹ�
            writeToLogFile("д����ɣ�");            
            }

            i++;

            //if (i % 1000 == 0)
            //{
            //    int progress = float(haveRecvLen) / float(fileLen) * 100;
            //    // ���ý��ȿؼ��Ľ���ֵ
            //    progressControlWnd->SendMessage(PBM_SETPOS, progress, 0);
            //    //writeToLogFile("�ļ����ս���"+std::to_string(progress));
            //}

         
        } 
        // �ر��ļ�������
        _close(fd);
        if (iResult == 0)
                writeToLogFile("Connection closing..." + std::to_string(port));
        else {
            writeToLogFile("recv failed with error: %d"+std::to_string(WSAGetLastError()) + std::to_string(port));
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }   
        
    }
   
    //progressControlWnd->SendMessage(PBM_SETPOS, 100, 0);
    
    return 0;

}

// �������ڻ�ȡ�ѽ������ӵĶԶ˵� IP ��ַ�Ͷ˿ںţ����洢�� const char* ������
bool FileTransfer::GetPeerAddressAndPort(SOCKET connectSocket, char* ipAddress, int ipAddressSize, char* port, int portSize)
{
    struct sockaddr_storage addr;
    int addrLen = sizeof(addr);
    int result = getpeername(connectSocket, (struct sockaddr*)&addr, &addrLen);
    if (result == 0) {
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in* sockaddrIPv4 = (struct sockaddr_in*)&addr;
            inet_ntop(AF_INET, &(sockaddrIPv4->sin_addr), ipAddress, ipAddressSize);
            snprintf(port, portSize, "%d", ntohs(sockaddrIPv4->sin_port));
            return true;
        }
        else if (addr.ss_family == AF_INET6) {
            struct sockaddr_in6* sockaddrIPv6 = (struct sockaddr_in6*)&addr;
            inet_ntop(AF_INET6, &(sockaddrIPv6->sin6_addr), ipAddress, ipAddressSize);
            snprintf(port, portSize, "%d", ntohs(sockaddrIPv6->sin6_port));
            return true;
        }
    }

    return false;
}
int FileTransfer::connectToServer(const char* hostname, const char* port)
{
    if (iconnect == 1) {
        return 0;
    }
    char ipAddressConnect[INET6_ADDRSTRLEN];
    char portConnect[6];  // ��� 5 ���ַ� + ��β�� null �ַ�

    bool isSuccess = GetPeerAddressAndPort(ConnectSocket, ipAddressConnect, sizeof(ipAddressConnect), portConnect, sizeof(portConnect));

    if (!(strcmp(ipAddressConnect, hostname) == 0 && strcmp(portConnect, port))) //û�к�Ŀ��������������
    {

        int iResult;

        struct	addrinfo* ptr = NULL;

        //const char* hostname = "172.19.43.69"; //����ķ�������ַ��������ʹ��
        //const char* hostname = "127.0.0.1"; //����ķ�������ַ��������ʹ��

        iResult = getaddrinfo(hostname, port, &hints, &result);//��ȡ�Ľ���ŵ�result��
        //printf("iResult = %d\n", iResult);

        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return 1;
        }
 
        ptr = result;//ָ��ptr��result��ͬ

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        //��������ȷ���׽�������Ч���׽���
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("Error at socket():%ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup(); //WSACleanup ������ֹWS2_32 DLL ��ʹ�á�
            return 1;
        }

        //----------------------------
        showIPandPort(ptr);
        //-----------------------------
        //���ӵ�������
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);//ָ����С��������֤��Ϣ��ȡ��ȷ

        //�쳣����
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);//�ر�����
            printf("nnnnn\n");
            ConnectSocket = INVALID_SOCKET;
        }
        printf("aaaaaaaa\n");
        if (result == NULL)
        {
            printf("null\n");
        }

        //freeaddrinfo(result);
        printf("bbbbbbbbb\n");
        if (ConnectSocket == INVALID_SOCKET)
        {
            writeToLogFile("�޷����ӷ�����\n");
            //WSACleanup();//��ֹWS2_32 DLL��ʹ��
            return 1;
        }
        else {
            writeToLogFile("���ӵ��˷�����%d\n");
        }
    }

    return 0;

}

long FileTransfer::getFileSize(FILE* file) {
    long size;

    // �ƶ��ļ�ָ�뵽�ļ�ĩβ
    fseek(file, 0L, SEEK_END);

    // ��ȡ�ļ�ָ���λ�ã����ļ�����
    size = ftell(file);

    // ���ļ�ָ���ƶ����ļ���ͷ
    fseek(file, 0L, SEEK_SET);

    return size;
}


int FileTransfer::sendData(const char* filePath,int port,int index,int subLen)
{
    //��������------------------------------
    writeToLogFile("client:��ʼ��������");
    char ipAddressConnect[INET6_ADDRSTRLEN];
    char portConnect[6];  // ��� 5 ���ַ� + ��β�� null �ַ�
    SOCKET SendtSocket = SOCKET_ERROR;//�ѽ���������

    bool isSuccess = GetPeerAddressAndPort(SendtSocket, ipAddressConnect, sizeof(ipAddressConnect), portConnect, sizeof(portConnect));

    if (!(strcmp(ipAddressConnect, this->ipAddress) == 0 && strcmp(portConnect, std::to_string(port).c_str()))) //û�к�Ŀ��������������
    {
        writeToLogFile("client: ����if");
        int iResult;
        struct	addrinfo* ptr = NULL;
        // ������������ַ�Ͷ˿�
 
        writeToLogFile("client:this->ipAddress=" + std::string(this->ipAddress));
        iResult = getaddrinfo(this->ipAddress, std::to_string(port).c_str(), &hints, &result);//��ȡ�Ľ���ŵ�result��
        //printf("iResult = %d\n", iResult);

        if (iResult != 0) {
            writeToLogFile("getaddrinfo failed: %d\n");
            WSACleanup();
            return 1;
        }

        ptr = result;//ָ��ptr��result��ͬ

        // Create a SOCKET for connecting to server
        SendtSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        //��������ȷ���׽�������Ч���׽���
        if (SendtSocket == INVALID_SOCKET)
        {
            writeToLogFile("Error at socket():%ld\n");
            freeaddrinfo(result);
            WSACleanup(); //WSACleanup ������ֹWS2_32 DLL ��ʹ�á�
            return 1;
        }

        //----------------------------
        showIPandPort(ptr);
        //-----------------------------
        //���ӵ�������
        iResult = connect(SendtSocket, ptr->ai_addr, (int)ptr->ai_addrlen);//ָ����С��������֤��Ϣ��ȡ��ȷ

        //�쳣����
        while (iResult == SOCKET_ERROR)
        {
            closesocket(SendtSocket);//�ر�����
            writeToLogFile("nnnnn\n");
            SendtSocket = INVALID_SOCKET;
            iResult = connect(SendtSocket, ptr->ai_addr, (int)ptr->ai_addrlen);//ָ����С��������֤��Ϣ��ȡ��ȷ

        }
        writeToLogFile("aaaaaaaa\n");
        if (result == NULL)
        {
            writeToLogFile("null\n");
        }

        //freeaddrinfo(result);
        writeToLogFile("bbbbbbbbb\n");
        if (SendtSocket == INVALID_SOCKET)
        {
            writeToLogFile("�޷����ӷ�����\n");
            //WSACleanup();//��ֹWS2_32 DLL��ʹ��
            return 1;
        }
        else {
            writeToLogFile("���ӵ��˷�����%d\n");
        }
    }
    //����������----------------------------
    writeToLogFile("client:�˳�if");

     
    //FILE* sbufp = fp;
    //fseek(sbufp, index, SEEK_SET);//��index��ʼ��ȡ�ļ�
    //writeToLogFile("client:�ļ�ָ����ʼλ��" + std::to_string(int(sbufp)) + "�˿ںţ�" + std::to_string(port));
    int fd = _open(filePath, _O_RDONLY | _O_BINARY);
    if (fd == -1) {

    }
    else {//�򿪳ɹ�
        __int64 newPosition = _lseeki64(fd, index, SEEK_SET);  // �����ļ�ָ��λ��Ϊƫ�Ƶ�ַ
        char fileIndex[DEFAULT_BUFLEN] = { 0 };
        char buffertmp[DEFAULT_BUFLEN] = { 0 };
        sprintf_s(fileIndex, sizeof(fileIndex), "%ld", index);
        addNewline(fileIndex);//ĩβ����'\n'
        send(SendtSocket, fileIndex, sizeof(fileIndex), MSG_OOB); //�����ļ��ֽ�����
        recv(SendtSocket, buffertmp, DEFAULT_BUFLEN, 0);//�����ļ��ֽ�����--ͬ��


        unsigned char buffer[DEFAULT_BUFLEN];
        size_t bytesRead;
   
        writeToLogFile("client:��ʼ��ȡ�ļ�������,���������ֽ�"+std::to_string(subLen) + "�˿ںţ�" + std::to_string(port));
        int sum = 0;
        int needReadLen = 0;
        while (sum < subLen)//δ����
        {
            writeToLogFile("client:����ѭ��,���������ֽ�" + std::to_string(subLen)+ "�˿ںţ�" + std::to_string(port));
            needReadLen = (DEFAULT_BUFLEN < subLen) ? DEFAULT_BUFLEN : subLen;

            

            bytesRead = _read(fd, buffer, sizeof(buffer));

            std::string str;
            std::copy(buffer, buffer + DEFAULT_BUFLEN, std::back_inserter(str));
            writeToLogFile("client:����ѭ��,�������ֽ�����" + str + "�˿ںţ�" + std::to_string(port));

            //sum += bytesRead;
            
            //writeToLogFile("client:��ѭ����Ŀǰ�Ѷ��ֽ���" + std::to_string(sum) + "�˿ںţ�"+std::to_string(port));
            //��¼Ŀǰ�ܳ���
            //���½�����
           

            if (bytesRead > 0) {
                iResult = send(SendtSocket, (const char*)buffer, bytesRead, 0);
                writeToLogFile("client:������" + std::to_string(iResult) + "�ֽ�" + "�˿ںţ�" + std::to_string(port));
            }
            else {
                break;
            }
        }
        // �ر��ļ�������
        _close(fd);
        //fclose(fp);//�ر��ļ�
        writeToLogFile("client:���ͽ���");
        //��������
        //writeToLogFile("client:�ļ�ָ����ֹλ��" + std::
    }
 

    closesocket(SendtSocket);
 

    return 0;
}

FileTransfer::~FileTransfer()
{
    WSACleanup();
}

void FileTransfer::muliThreadTrans(char* filePath) {
    int iResult = 0;
    char* sendbuf = NULL;
    char* lastBackslash = strrchr(filePath, '\\');
    if (lastBackslash != nullptr)
    {
        sendbuf = lastBackslash + 1;
    }
    else
    {
        // ����Ҳ�����б�ܣ��򷵻�ԭʼ·��
        sendbuf = filePath;
    }
    
     
    char recvbuf[DEFAULT_BUFLEN] = { 0 };
    int recvbuflen = DEFAULT_BUFLEN;

    //�����ļ���------------------------------------------------------
    addNewline(sendbuf);//ĩβ����'\n'
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);//�����ļ���
    writeToLogFile("client:�����ļ�����" + std::string(sendbuf));
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);//����ͬ���ź�
    writeToLogFile("client:�յ��ļ���ͬ����Ϣ");
    //�����ļ���------------------------------------------------------
    
    //�����ļ�����-----------------------------------------------------
    //���ļ���ѭ����ȡ������
    FILE* fp;//�ļ�ָ�����
    removeNewline(filePath);//ȥ��ĩβ��'\n'
    // ���ļ�����ȡ�ļ�������
    //int fd = open(filePath, O_RDONLY);


    if (fopen_s(&fp, filePath, "rb") != 0) {
        writeToLogFile("client:�޷����ļ���");
        pStaticText->SetWindowText(CString("�޷����ļ���"));//���ӻ���ʾ
        printf("�޷����ļ���\n");
         
    }





    FILE* fplen = fp;//�ļ�ָ��
    long int fileSize = getFileSize(fplen);
    this->totalLen = fileSize;//�ļ��ܳ���
    fclose(fp);
    char fileLength[512]; // ����Ŀ���ַ����ĳ��Ȳ�����20���ַ�
    sprintf_s(fileLength, sizeof(fileLength), "%ld", fileSize);
    addNewline(fileLength);//ĩβ����'\n'
    send(ConnectSocket, fileLength, sizeof(fileLength), MSG_OOB); //�����ļ�����
    writeToLogFile("client:�����ļ����ȣ�" + std::string(fileLength));
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);//����ͬ���ź�
    //�����ļ�����-----------------------------------------------------

    //pProgressCtrl->SetPos(50);

    //������Ҫ���߳���-----------------------------------------------------
    int threadNum = 2;
    //TODO:�����ļ���Сѡ��threadNum��С
    long int  tenMb = std::stol("10485760");
    long int threentyMb = std::stol("31457280");
    if (fileSize < tenMb) {
        threadNum = 2;
    }
    else if (fileSize > threentyMb) {
        threadNum = 8;
    }
    else {
        threadNum = 4;
    }

    char threadNumStr[20]; // �����㹻�洢����ת������ַ���

    sprintf_s(threadNumStr, sizeof(threadNumStr), "%d", threadNum);
    addNewline(threadNumStr);//ĩβ����'\n'
    send(ConnectSocket, threadNumStr, sizeof(threadNumStr), MSG_OOB); //�����߳���
    //������Ҫ���߳���-----------------------------------------------------


    //�����ļ�ָ������λ��
    //long int fileSize �ļ��ֽ���
    writeToLogFile("client:�ļ��ֽ�����" + std::to_string(fileSize));
    int threadSize = fileSize / threadNum;//ÿ���߳���Ҫ���͵��ֽ��� 8
    int threadSizeLast = threadSize + fileSize % threadNum;//���һ���߳���Ҫ���͵��ֽ��� 8 + 2 = 10
    int threadSizeLastIndex = threadNum - 1;//���һ���̵߳�����
    int threadSizeLastIndexStart = threadSizeLastIndex * threadSize;//���һ���̵߳���ʼλ��
    std::vector<std::thread> threadQueue;

    for (int i = 0; i < threadNum; i++) {
        int len = (i == threadSizeLastIndex) ? threadSizeLast : threadSize;
        int startIdx = i * threadSize;
        writeToLogFile("�ļ����ֽ�������" + std::to_string(startIdx)+",client:�ļ��ֶγ��ȣ�" + std::to_string(len) );
        writeToLogFile("�������߳�" + std::to_string(portNum[i]));
        //sendData(fp, portNum[i], startIdx, 25);
        threadQueue.emplace_back(&FileTransfer::sendData, this, filePath, portNum[i], startIdx, len); // ���ͣ�����һ�����̲߳���ӵ��̶߳���
    }
    // �ȴ������߳�ִ�����
    for (auto& threadObj : threadQueue) {
        if (threadObj.joinable()) {
            threadObj.join();
        }
    }
 

}