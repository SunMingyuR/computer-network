#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define N 256

void commandHelp();
void commandExit();
void commandFileList(struct sockaddr_in, char *);
void commandDownload(struct sockaddr_in , char *);
void commandUpload(struct sockaddr_in , char *);

int main(int argc, char *argv[])
{
    char command[N];
    struct sockaddr_in addr;
    int len;
    // 将＆addr中的前sizeof(addr)字节置为0
    bzero(&addr, sizeof(addr)); 
    // AF_INET代表TCP／IP协议    
    addr.sin_family = AF_INET;   
    // 将点间隔地址转换为网络字节顺序   
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    // 转换为网络字节顺序
    addr.sin_port = htons(8989);    
    len = sizeof(addr);

    while(1)
    {
        printf("ftp>");
        bzero(command, N);
        // fgets函数从stdin流中读取N-1个字符放入command中
        if(fgets(command, N, stdin) == NULL)
        {
            printf("Fgets Error!\n");
            return -1;
        }

        // fgets函数读取的最后一个字符为换行符, 此处将其替换为'\0'
        command[strlen(command)-1] = '\0';    

        printf("Input Command Is [ %s ]\n", command);

        // 比较两个字符串前4个字节，若相等则返回0
        if(strncmp(command, "help", 4) == 0) 
        {
            commandHelp();
        }else if(strncmp(command, "exit", 4) == 0)
        {
            commandExit();
            // 结束进程
            exit(0);   
        }else if(strncmp(command, "ls" , 2) == 0)
        {
            commandFileList(addr, command);
        }else if(strncmp(command, "download", 8) == 0)
        {
            commandDownload(addr, command);
        }else if(strncmp(command, "upload", 6) == 0 )
        {
            commandUpload(addr, command);
        }else
        {
            printf("Command Is Error! Please Try Again!\n");
        }

    }
    return 0;
}

// FTP命令行
void commandHelp()
{
    printf("help:显示所有FTP服务器命令\n");
    printf("exit:离开FTP服务器\n");
    printf("ls : 显示FTP服务器的文件列表\n");
    printf("download <file>：从FTP服务器下载文件\n");
    printf("upload <file>:上传文件到FTP服务器\n");
    return ;
}

// 退出FTP客户端
void commandExit()
{
    printf("Bye-Bye!\n");
}

// 文件列表展示功能的实现
void commandFileList(struct sockaddr_in addr, char *command)
{
    int newSocket;
    //创建套接字
    if((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }

    if(connect(newSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }

    // 将command指向的内容写入到socket所指的文件中
    if(write(newSocket, command, N) < 0)
    {
        printf("Write Error!\n");
        exit(1);
    }

    // 从socket中读取N字节内容放入command中, 返回值为读取的字节数
    while(read(newSocket, command, N) > 0)  
    {                                   
        printf(" %s ",command);
    }
    printf("\n");

    close(newSocket);
    return ;
}

// 文件下载功能的实现
void commandDownload(struct sockaddr_in addr, char *command)
{
    int file;
    int newSocket;
    char buffer[N];
    int nbytes;

    // 创建套接字, 并进行错误检测
    if((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }

    // connect函数用于实现客户端与服务端的连接, 此处还进行了错误检测
    if(connect(newSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }

    // 通过write函数向服务器发送数据
    if(write(newSocket, command, N) < 0)
    {
        printf("Write Error!At commandDownload 1\n");
        exit(1);
    }

    // 通过read函数来接受服务器发来的数据
    if(read(newSocket, buffer, N) < 0)
    {
        printf("Read Error!At commandDownload 1\n");
        exit(1);
    }
    // 用于检测服务器端文件是否打开成功
    if(buffer[0] == 'N')
    {
        close(newSocket);
        printf("Can't Open The File!\n");
        return ;
    }
    // open函数创建一个文件, 文件地址为(command+4), 该地址从命令行输入获取
    if((file=open(command + 9, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
    {
        printf("Open Error!\n");
        exit(1);
    }
    // read函数从套接字中获取N字节数据放入buffer中, 返回值为读取的字节数
    while((nbytes = read(newSocket, buffer, N)) > 0)
    {
        // write函数将buffer中的内容读取出来写入file所指向的文件, 返回值为实际写入的字节数
        if(write(file, buffer, nbytes) < 0)
        {
            printf("Write Error!At commandDownload 2");
        }
    }

    close(file);
    close(newSocket);

    return ;

}

// 文件上传功能的实现
void commandUpload(struct sockaddr_in addr, char *command)
{
    int file;
    int newSocket;
    char buffer[N];
    int nbytes;

    // 创建套接字
    if((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }

    // 客户端与服务端连接
    if(connect(newSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }

    // 从command中读取N字节数据, 写入套接字中
    if(write(newSocket, command, N)<0)
    {
        printf("Wrtie Error!At commandUpload 1\n");
        exit(1);
    }

    // open函数从(command+4)中读取文件路径, 并以只读的方式打开
    if((file = open(command + 7, O_RDONLY)) < 0)
    {
        printf("Open Error!\n");
        exit(1);
    }

    // 从file指向的文件中读取N个字节数据
    while((nbytes = read(file, buffer, N)) > 0)
    {
        // 从buffer中读取nbytes字节数据，写入套接字中
        if(write(newSocket, buffer, nbytes) < 0)
        {
            printf("Write Error!At commandUpload 2");
        }
    }

    close(file);
    close(newSocket);

    return ;
}