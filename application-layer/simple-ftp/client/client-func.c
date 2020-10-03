#include "client.h"
void commandHelp()
{
    printf("help:显示所有FTP服务器命令\n");
    printf("exit:离开FTP服务器\n");
    printf("ls : 显示FTP服务器的文件列表\n");
    printf("download <file>：从FTP服务器下载文件\n");
    printf("upload <file>:上传文件到FTP服务器\n");
    return ;
}

void commandExit()
{
    printf("Bye-Bye!\n");
}

void commandFileList(struct sockaddr_in addr, char *commd)
{
    int sockfd;
    //创建套接字
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }

    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    //将commd指向的内容写入到sockfd所指的文件中，此处即指套接字
    if(write(sockfd, commd, N) < 0)
    {
        printf("Write Error!\n");
        exit(1);
    }

    while(read(sockfd, commd, N) > 0)  //从sockfd中读取N字节内容放入commd中，
    {                                   //返回值为读取的字节数
        printf(" %s ",commd);
    }
    printf("\n");

    close(sockfd);
    return ;
}
/*
**实现文件的下载                            
*/
void commandDownload(struct sockaddr_in addr, char *commd)
{
    int fd;
    int sockfd;
    char buffer[N];
    int nbytes;
    //创建套接字，并进行错误检测
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }
    //connect函数用于实现客户端与服务端的连接,此处还进行了错误检测
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    //通过write函数向服务端发送数据
    if(write(sockfd, commd, N) < 0)
    {
        printf("Write Error!At commandDownload 1\n");
        exit(1);
    }
    //利用read函数来接受服务器发来的数据
    if(read(sockfd, buffer, N) < 0)
    {
        printf("Read Error!At commandDownload 1\n");
        exit(1);
    }
    //用于检测服务器端文件是否打开成功
    if(buffer[0] =='N')
    {
        close(sockfd);
        printf("Can't Open The File!\n");
        return ;
    }
    //open函数创建一个文件，文件地址为(commd+4)，该地址从命令行输入获取
    if((fd=open(commd+4, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
    {
        printf("Open Error!\n");
        exit(1);
    }
    //read函数从套接字中获取N字节数据放入buffer中，返回值为读取的字节数
    while((nbytes=read(sockfd, buffer, N)) > 0)
    {
        //write函数将buffer中的内容读取出来写入fd所指向的文件，返回值为实际写入的字节数
        if(write(fd, buffer, nbytes) < 0)
        {
            printf("Write Error!At commandDownload 2");
        }
    }

    close(fd);
    close(sockfd);

    return ;

}
/*
**实现文件的上传                            
*/
void commandUpload(struct sockaddr_in addr, char *commd)
{
    int fd;
    int sockfd;
    char buffer[N];
    int nbytes;
    //创建套接字
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }
    //客户端与服务端连接
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    //从commd中读取N字节数据，写入套接字中
    if(write(sockfd, commd, N)<0)
    {
        printf("Wrtie Error!At commandUpload 1\n");
        exit(1);
    }
    //open函数从(commd+4)中，读取文件路径，以只读的方式打开
    if((fd=open(commd+4, O_RDONLY)) < 0)
    {
        printf("Open Error!\n");
        exit(1);
    }
    //从fd指向的文件中读取N个字节数据
    while((nbytes=read(fd, buffer, N)) > 0)
    {
        //从buffer中读取nbytes字节数据，写入套接字中
        if(write(sockfd, buffer, nbytes) < 0)
        {
            printf("Write Error!At commandUpload 2");
        }
    }

    close(fd);
    close(sockfd);

    return ;
}