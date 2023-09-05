#pragma warning(disable : 4996)
// 引发 C4996 警告的代码


#include "pch.h"
#include "FileTransfer.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <mutex>

std::mutex mtx;  // 定义互斥锁
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
    //    // 文件不存在，创建新文件
    //    file.open("log.txt");
    //    if (!file) {
    //        std::cout << "无法创建文件！" << std::endl;
    //        return;
    //    }
    //}

    //// 获取当前时间
    //std::time_t currentTime = std::time(nullptr);
    //std::tm localTime{};
    //localtime_s(&localTime, &currentTime);
    //char timeString[100];
    //std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &localTime);

    //// 写入时间和文本
    //file << timeString << std::endl;
    //file << text << std::endl;

    //// 关闭文件
    //file.close();

    //std::cout << "写入成功！" << std::endl;
}

 
void FileTransfer::AddTextToStaticControl(const CString& newText)
{
    if (pStaticText != nullptr)
    {
        // 获取当前的文本内容
        CString strText;
        pStaticText->GetWindowText(strText);

        // 添加新的一行文本
        strText += _T("\r\n") + newText;

        // 设置新的文本内容
        pStaticText->SetWindowText(strText);
    }
}

void FileTransfer::showIPandPort(addrinfo* ptr)
{
    // 将 sockaddr 转换为 sockaddr_in 结构体
    sockaddr_in* addr_in = (sockaddr_in*)ptr->ai_addr;

    // 将 IP 地址转换为字符串并打印
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("IP address: %s\n", ip_str);

    // 将端口号打印
    printf("Port number: %d\n", ntohs(addr_in->sin_port));
}
int FileTransfer::initialize()
{
    int tmp[8]={9990,9991,9992,9993,9994,9995,9996,9997};
    for (int i = 0; i < 8; i++) {
        this->portNum[i] = tmp[i];// 初始化多线程端口号
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
    // 解析服务器地址和端口
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
    writeToLogFile("server:创建ListenSocket套接字");
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    writeToLogFile("server:绑定ListenSocket套接字");
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    writeToLogFile("server:开始监听");
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    writeToLogFile("server:等待连接");
    ClientSocket = accept(ListenSocket, NULL, NULL);

    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    writeToLogFile("server:连接已建立。");
    this->iconnect = 1;
    pStaticConnectText->SetWindowText(_T("已连接"));
    // No longer need server socket
    closesocket(ListenSocket);


    //连接建立之后应该开始协商=================
    //char recvbuf[DEFAULT_BUFLEN] = { 0 };
    char filenameBuffer[DEFAULT_BUFLEN] = { 0 };
    char fileLenBuffer[DEFAULT_BUFLEN] = { 0 };
    char threadNumBuffer[DEFAULT_BUFLEN] = { 0 };
    char* recvStr = "hjy";
    int recvbuflen = DEFAULT_BUFLEN;

    //接收文件名
    iResult = recv(ClientSocket, filenameBuffer, recvbuflen, 0);
    removeNewline(filenameBuffer);
    if (iResult > 0) {
		writeToLogFile("server:接收到文件名"+std::string(filenameBuffer));
        send(ClientSocket, recvStr, (int)strlen(recvStr), 0);//同步
        //接收文件长度
        iResult = recv(ClientSocket, fileLenBuffer, recvbuflen, 0);
        removeNewline(fileLenBuffer);
        if (iResult > 0) {
            writeToLogFile("server:接收到文件长度" + std::string(fileLenBuffer));
            send(ClientSocket, recvStr, (int)strlen(recvStr), 0);//同步

            //接收线程数
            iResult = recv(ClientSocket, threadNumBuffer, recvbuflen, 0);
            removeNewline(threadNumBuffer);
            if (iResult > 0) {
				writeToLogFile("server:接收到线程数" + std::string(threadNumBuffer));
                int threadNum = std::stoi(threadNumBuffer); // 将字符串转换为整数
                //开始接收
                writeToLogFile("server:等待接收...TODO");
                FILE* fp = NULL; // 打开输入文件
                if (fopen_s(&fp, filenameBuffer, "wb") != 0) {
                    // 打开文件失败
                    writeToLogFile("无法打开文件\n");
                    return 1;
                }
                else {
                    writeToLogFile("成功打开文件" + std::string(filenameBuffer));
                    // 将文件指针移动到指定位置
                    long fileLen = atol(fileLenBuffer); // 转换为长整数
                    if (fseek(fp, fileLen - 1, SEEK_SET) != 0) {
                        printf("无法移动文件指针\n");
                        fclose(fp);
                        return 1;
                    }
                    // 写入一个字节，将文件长度扩展到指定长度
                    if (fwrite("", 1, 1, fp) != 1) {
                        printf("无法写入文件\n");
                        fclose(fp);
                        return 1;
                    }
                    fclose(fp);
                }
                //fopen_s(&fp, filenameBuffer, "wb");
                std::vector<std::thread> threadQueue;

                // 启动线程
                for (int i = 0; i < threadNum; i++) {
                    threadQueue.emplace_back(&FileTransfer::receive, this, filenameBuffer, portNum[i]);
                }
                 
                send(ClientSocket, recvStr, (int)strlen(recvStr), 0); // 同步
                writeToLogFile("server:已同步");
                
              
               
                
                // 等待所有线程执行完毕
                for (auto& threadObj : threadQueue) {
                    if (threadObj.joinable()) {
                        threadObj.join();
                    }
                }

                fclose(fp);

            }
            else {
				writeToLogFile("server:接收线程数失败");
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
            }
        }
        else {
            writeToLogFile("server:接收文件长度失败");
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

	}
    else {
		writeToLogFile("server:接收文件名失败");
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

 
    return 0;
}
int FileTransfer::receive(const char* filename,int port)//接收
{
    writeToLogFile("server:执行receive"+std::to_string(port));
    int iResult = 0;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    // 解析服务器地址和端口
    struct addrinfo* result;
    iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
    if (iResult != 0) {
        writeToLogFile("getaddrinfo failed with error: %d " + std::to_string(port));
        WSACleanup();
        return 1;
    }
  
    writeToLogFile("server:多线程创建ListenSocket套接字"+std::to_string(port));
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        writeToLogFile("socket failed with error: %ld\n");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    writeToLogFile("server:多线程绑定ListenSocket套接字" + std::to_string(port));
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        writeToLogFile("bind failed with error: %d\n");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    writeToLogFile("server:多线程开始监听" + std::to_string(port));
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        writeToLogFile("listen failed with error: %d\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

   
    writeToLogFile("server:多线程等待连接" + std::to_string(port));
    ClientSocket = accept(ListenSocket, NULL, NULL);

    if (ClientSocket == INVALID_SOCKET) {
        writeToLogFile("accept failed with error: %d\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    writeToLogFile("server:多线程连接已建立。" + std::to_string(port));
    pStaticConnectText->SetWindowText(_T("已连接"));
    // No longer need server socket
    closesocket(ListenSocket);

    //// 设置套接字为非阻塞模式
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

        
    
    writeToLogFile("server:等待接收文件字节下标..." + std::to_string(port));
    recv(ClientSocket, indexbuf, recvbuflen, 0);//接收文件下标
    removeNewline(indexbuf);
    writeToLogFile("server:接收到了文件字节下标  " + std::string(indexbuf) +"  " + std::to_string(port));
    send(ClientSocket, "hjy", (int)strlen("hjy"), 0);//同步
    
    
    long int fileIndex = atoi(indexbuf);
    writeToLogFile("！！！！文件名字：" + std::string(filename));
    //打开文件，循环接收并写入
    int fd = _open(filename, _O_WRONLY | _O_BINARY);
    __int64 newPosition = _lseeki64(fd, fileIndex, SEEK_SET);  // 设置文件指针位置为偏移地址
    if (fd == -1) {
        // 处理文件打开失败的情况
    }
    else {//文件可以打开
        int i = 0;
         while ((iResult = recv(ClientSocket, recvbuf, recvbuflen, 0)) > 0)
        {
        
        
            haveRecvLen += iResult;//记录接收到的文件长度
            writeToLogFile("server:TCP接收到数据! 长度="+std::to_string(iResult));

         
            writeToLogFile("写入... "+std::string(recvbuf));
            int bytesWritten = _write(fd, recvbuf, iResult);
            if (bytesWritten > 0) {
                // 写入成功
            writeToLogFile("写入完成！");            
            }

            i++;

            //if (i % 1000 == 0)
            //{
            //    int progress = float(haveRecvLen) / float(fileLen) * 100;
            //    // 设置进度控件的进度值
            //    progressControlWnd->SendMessage(PBM_SETPOS, progress, 0);
            //    //writeToLogFile("文件接收进度"+std::to_string(progress));
            //}

         
        } 
        // 关闭文件描述符
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

// 函数用于获取已建立连接的对端的 IP 地址和端口号，并存储到 const char* 变量中
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
    char portConnect[6];  // 最多 5 个字符 + 结尾的 null 字符

    bool isSuccess = GetPeerAddressAndPort(ConnectSocket, ipAddressConnect, sizeof(ipAddressConnect), portConnect, sizeof(portConnect));

    if (!(strcmp(ipAddressConnect, hostname) == 0 && strcmp(portConnect, port))) //没有和目标主机建立连接
    {

        int iResult;

        struct	addrinfo* ptr = NULL;

        //const char* hostname = "172.19.43.69"; //假设的服务器地址——测试使用
        //const char* hostname = "127.0.0.1"; //假设的服务器地址——测试使用

        iResult = getaddrinfo(hostname, port, &hints, &result);//获取的结果放到result中
        //printf("iResult = %d\n", iResult);

        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return 1;
        }
 
        ptr = result;//指针ptr与result相同

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        //检查错误以确保套接字是有效的套接字
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("Error at socket():%ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup(); //WSACleanup 用于终止WS2_32 DLL 的使用。
            return 1;
        }

        //----------------------------
        showIPandPort(ptr);
        //-----------------------------
        //连接到服务器
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);//指明大小，用来保证信息读取正确

        //异常处理
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);//关闭连接
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
            writeToLogFile("无法连接服务器\n");
            //WSACleanup();//终止WS2_32 DLL的使用
            return 1;
        }
        else {
            writeToLogFile("连接到了服务器%d\n");
        }
    }

    return 0;

}

long FileTransfer::getFileSize(FILE* file) {
    long size;

    // 移动文件指针到文件末尾
    fseek(file, 0L, SEEK_END);

    // 获取文件指针的位置，即文件长度
    size = ftell(file);

    // 将文件指针移动回文件开头
    fseek(file, 0L, SEEK_SET);

    return size;
}


int FileTransfer::sendData(const char* filePath,int port,int index,int subLen)
{
    //建立连接------------------------------
    writeToLogFile("client:开始建立连接");
    char ipAddressConnect[INET6_ADDRSTRLEN];
    char portConnect[6];  // 最多 5 个字符 + 结尾的 null 字符
    SOCKET SendtSocket = SOCKET_ERROR;//已建立的连接

    bool isSuccess = GetPeerAddressAndPort(SendtSocket, ipAddressConnect, sizeof(ipAddressConnect), portConnect, sizeof(portConnect));

    if (!(strcmp(ipAddressConnect, this->ipAddress) == 0 && strcmp(portConnect, std::to_string(port).c_str()))) //没有和目标主机建立连接
    {
        writeToLogFile("client: 进入if");
        int iResult;
        struct	addrinfo* ptr = NULL;
        // 解析服务器地址和端口
 
        writeToLogFile("client:this->ipAddress=" + std::string(this->ipAddress));
        iResult = getaddrinfo(this->ipAddress, std::to_string(port).c_str(), &hints, &result);//获取的结果放到result中
        //printf("iResult = %d\n", iResult);

        if (iResult != 0) {
            writeToLogFile("getaddrinfo failed: %d\n");
            WSACleanup();
            return 1;
        }

        ptr = result;//指针ptr与result相同

        // Create a SOCKET for connecting to server
        SendtSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        //检查错误以确保套接字是有效的套接字
        if (SendtSocket == INVALID_SOCKET)
        {
            writeToLogFile("Error at socket():%ld\n");
            freeaddrinfo(result);
            WSACleanup(); //WSACleanup 用于终止WS2_32 DLL 的使用。
            return 1;
        }

        //----------------------------
        showIPandPort(ptr);
        //-----------------------------
        //连接到服务器
        iResult = connect(SendtSocket, ptr->ai_addr, (int)ptr->ai_addrlen);//指明大小，用来保证信息读取正确

        //异常处理
        while (iResult == SOCKET_ERROR)
        {
            closesocket(SendtSocket);//关闭连接
            writeToLogFile("nnnnn\n");
            SendtSocket = INVALID_SOCKET;
            iResult = connect(SendtSocket, ptr->ai_addr, (int)ptr->ai_addrlen);//指明大小，用来保证信息读取正确

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
            writeToLogFile("无法连接服务器\n");
            //WSACleanup();//终止WS2_32 DLL的使用
            return 1;
        }
        else {
            writeToLogFile("连接到了服务器%d\n");
        }
    }
    //建立了连接----------------------------
    writeToLogFile("client:退出if");

     
    //FILE* sbufp = fp;
    //fseek(sbufp, index, SEEK_SET);//从index开始读取文件
    //writeToLogFile("client:文件指针起始位置" + std::to_string(int(sbufp)) + "端口号：" + std::to_string(port));
    int fd = _open(filePath, _O_RDONLY | _O_BINARY);
    if (fd == -1) {

    }
    else {//打开成功
        __int64 newPosition = _lseeki64(fd, index, SEEK_SET);  // 设置文件指针位置为偏移地址
        char fileIndex[DEFAULT_BUFLEN] = { 0 };
        char buffertmp[DEFAULT_BUFLEN] = { 0 };
        sprintf_s(fileIndex, sizeof(fileIndex), "%ld", index);
        addNewline(fileIndex);//末尾加上'\n'
        send(SendtSocket, fileIndex, sizeof(fileIndex), MSG_OOB); //发送文件字节索引
        recv(SendtSocket, buffertmp, DEFAULT_BUFLEN, 0);//接收文件字节索引--同步


        unsigned char buffer[DEFAULT_BUFLEN];
        size_t bytesRead;
   
        writeToLogFile("client:开始读取文件并发送,期望发送字节"+std::to_string(subLen) + "端口号：" + std::to_string(port));
        int sum = 0;
        int needReadLen = 0;
        while (sum < subLen)//未结束
        {
            writeToLogFile("client:进入循环,期望发送字节" + std::to_string(subLen)+ "端口号：" + std::to_string(port));
            needReadLen = (DEFAULT_BUFLEN < subLen) ? DEFAULT_BUFLEN : subLen;

            

            bytesRead = _read(fd, buffer, sizeof(buffer));

            std::string str;
            std::copy(buffer, buffer + DEFAULT_BUFLEN, std::back_inserter(str));
            writeToLogFile("client:进入循环,读到的字节内容" + str + "端口号：" + std::to_string(port));

            //sum += bytesRead;
            
            //writeToLogFile("client:进循环，目前已读字节数" + std::to_string(sum) + "端口号："+std::to_string(port));
            //记录目前总长度
            //更新进度条
           

            if (bytesRead > 0) {
                iResult = send(SendtSocket, (const char*)buffer, bytesRead, 0);
                writeToLogFile("client:发送了" + std::to_string(iResult) + "字节" + "端口号：" + std::to_string(port));
            }
            else {
                break;
            }
        }
        // 关闭文件描述符
        _close(fd);
        //fclose(fp);//关闭文件
        writeToLogFile("client:发送结束");
        //结束发送
        //writeToLogFile("client:文件指针终止位置" + std::
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
        // 如果找不到反斜杠，则返回原始路径
        sendbuf = filePath;
    }
    
     
    char recvbuf[DEFAULT_BUFLEN] = { 0 };
    int recvbuflen = DEFAULT_BUFLEN;

    //发送文件名------------------------------------------------------
    addNewline(sendbuf);//末尾加上'\n'
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);//发送文件名
    writeToLogFile("client:发送文件名：" + std::string(sendbuf));
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);//接收同步信号
    writeToLogFile("client:收到文件名同步消息");
    //发送文件名------------------------------------------------------
    
    //发送文件长度-----------------------------------------------------
    //打开文件，循环读取并发送
    FILE* fp;//文件指针变量
    removeNewline(filePath);//去掉末尾的'\n'
    // 打开文件并获取文件描述符
    //int fd = open(filePath, O_RDONLY);


    if (fopen_s(&fp, filePath, "rb") != 0) {
        writeToLogFile("client:无法打开文件。");
        pStaticText->SetWindowText(CString("无法打开文件。"));//可视化显示
        printf("无法打开文件。\n");
         
    }





    FILE* fplen = fp;//文件指针
    long int fileSize = getFileSize(fplen);
    this->totalLen = fileSize;//文件总长度
    fclose(fp);
    char fileLength[512]; // 假设目标字符串的长度不超过20个字符
    sprintf_s(fileLength, sizeof(fileLength), "%ld", fileSize);
    addNewline(fileLength);//末尾加上'\n'
    send(ConnectSocket, fileLength, sizeof(fileLength), MSG_OOB); //发送文件长度
    writeToLogFile("client:发送文件长度：" + std::string(fileLength));
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);//接收同步信号
    //发送文件长度-----------------------------------------------------

    //pProgressCtrl->SetPos(50);

    //发送需要的线程数-----------------------------------------------------
    int threadNum = 2;
    //TODO:根据文件大小选择threadNum大小
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

    char threadNumStr[20]; // 假设足够存储整数转换后的字符串

    sprintf_s(threadNumStr, sizeof(threadNumStr), "%d", threadNum);
    addNewline(threadNumStr);//末尾加上'\n'
    send(ConnectSocket, threadNumStr, sizeof(threadNumStr), MSG_OOB); //发送线程数
    //发送需要的线程数-----------------------------------------------------


    //计算文件指针的相对位置
    //long int fileSize 文件字节数
    writeToLogFile("client:文件字节数：" + std::to_string(fileSize));
    int threadSize = fileSize / threadNum;//每个线程需要发送的字节数 8
    int threadSizeLast = threadSize + fileSize % threadNum;//最后一个线程需要发送的字节数 8 + 2 = 10
    int threadSizeLastIndex = threadNum - 1;//最后一个线程的索引
    int threadSizeLastIndexStart = threadSizeLastIndex * threadSize;//最后一个线程的起始位置
    std::vector<std::thread> threadQueue;

    for (int i = 0; i < threadNum; i++) {
        int len = (i == threadSizeLastIndex) ? threadSizeLast : threadSize;
        int startIdx = i * threadSize;
        writeToLogFile("文件内字节索引：" + std::to_string(startIdx)+",client:文件分段长度：" + std::to_string(len) );
        writeToLogFile("创建新线程" + std::to_string(portNum[i]));
        //sendData(fp, portNum[i], startIdx, 25);
        threadQueue.emplace_back(&FileTransfer::sendData, this, filePath, portNum[i], startIdx, len); // 发送，创建一个新线程并添加到线程队列
    }
    // 等待所有线程执行完毕
    for (auto& threadObj : threadQueue) {
        if (threadObj.joinable()) {
            threadObj.join();
        }
    }
 

}