#include "server.h"

int main(int arg, char *argv[])
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int serverAddrLen, clientAddrLen;
    char command [N];
    // 将commd所指向的字符串的前N个字节置为0
    bzero(command, N);

    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0) ) < 0)
    {
        printf("Sokcet Error!\n");
        return -1;
    }

    bzero(&serverAddr, sizeof(serverAddr));
    // AF_INET代表TCP／IP协议 
    serverAddr.sin_family = AF_INET;
    // 本地ip地址
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 转换成网络字节
    serverAddr.sin_port = htons(8989);
    serverAddrLen = sizeof(serverAddr);

    // 将ip地址与套接字绑定
    if((bind(serverSocket, (struct sockaddr *)&serverAddr, serverAddrLen)) < 0)
    {
        printf("Bind Error!\n");
        return -1;
    }
    // 服务器端监听
    if(listen(serverSocket, 5) < 0)
    {
        printf("Linsten Error!\n");
        return -1;
    }

    bzero(&clientAddr, sizeof(clientAddr));
    clientAddrLen = sizeof(clientAddr);

    while(1)
    {
        printf("server>");
        // 服务器端接受来自客户端的连接, 返回一个新建的套接字, 并将客户端的地址等信息存入cilentAddr中
        // 原来的套接字仍处于监听中
        if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0)
        {
            printf("Accept Error!\n");
            exit(1);
        }
        // 由套接字接收数据时，套接字把接收的数据放在套接字缓冲区，再由用户程序把它们复制到用户缓冲区，然后由read函数读取
        // write函数同理
        //read函数从clientSocket中读取N个字节数据放入command中
        if(read(clientSocket, command, N) < 0)  
        {
            printf("Read Error!\n");
            exit(1);
        }

        printf("received [ %s ]\n", command);

        if(strncmp(command, "ls", 2) == 0)
        {
            commandFileList(clientSocket);
        }else if(strncmp(command, "download", 8) == 0 )
        {
            commandDownload(clientSocket, command + 9);
        }else if(strncmp(command, "upload", 6) == 0)
        {
            commandUpload(clientSocket, command + 7);
        }else
        {
            printf("Command Error!\n");
        }
    }

    return 0;
}

// 文件列表展示功能的实现
void commandFileList(int socket)
{
    DIR * fileDir = NULL;
    struct dirent *dirFiles = NULL;
    char buffer[N] ;
    bzero(buffer, N);

    // opendir为用来打开参数name指定的目录, 并返回DIR*形态的目录流 
    if((fileDir = opendir(".")) == NULL)
    {
        printf("OpenDir Error!\n");
        exit(1);
    }

    // 用来读取目录,返回dirent结构体指针
    while((dirFiles = readdir(fileDir)) != NULL)
    {
        // 把文件名写入command指向的缓冲区
        if(sprintf(buffer, dirFiles->d_name, N) < 0)
        {
            printf("Sprintf Error!\n");
            exit(1);
        }

        // 将command缓冲区的内容发送给client
        if(write(socket, buffer, N) < 0 )
        {
            printf("Write Error!\n");
            exit(1);
        }
    }

    closedir(fileDir);
    close(socket);
    return ;
}

// 文件下载功能的实现
void commandDownload(int socket, char *filename)
{
    int file, nbytes;
    char buffer[N];
    bzero(buffer, N);

    printf("download filename : [ %s ]\n", filename);
    // 以只读的方式打开文件
    if((file = open(filename, O_RDONLY)) < 0)
    {
        printf("Open file Error!\n");
        buffer[0]='N';

        if(write(socket, buffer, N) <0)
        {
            printf("Write Error! At commandDownload 1\n");
            exit(1);
        }
        return ;
    }

    // 'Y'表示文件读取成功
    buffer[0] = 'Y';  
    if(write(socket, buffer, N) <0)
    {
        printf("Write Error! At commandDownload 2!\n");
        close(file);
        exit(1);
    }

    // 将file内容读到buffer中
    while((nbytes = read(file, buffer, N)) > 0)
    {   
        // 将buffer发送回client
        if(write(socket, buffer, nbytes) < 0)
        {
            printf("Write Error! At commandDownload 3!\n");
            close(file);
            exit(1);
        }
    }

    close(file);
    close(socket);
    return ;
}

// 文件上传功能的实现
void commandUpload(int socket, char *filename)
{
    int file, nbytes;
    char buffer[N];
    bzero(buffer, N);

    printf("download filename : [ %s ]\n", filename);
    // 以只写的方式打开文件，若文件存在则清空，若文件不存在则新建文件
    if((file = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
    {
        printf("Open file Error!\n");
        return ;
    }

    // 将client发送的文件写入buffer
    while((nbytes = read(socket, buffer, N)) > 0)
    {
        // 将buffer中的内容写到file中
        if(write(file, buffer, nbytes) < 0)
        {
            printf("Write Error! At commandUpload 1!\n");
            close(file);
            exit(1);
        }
    }

    close(file);
    close(socket);
    return ;
}