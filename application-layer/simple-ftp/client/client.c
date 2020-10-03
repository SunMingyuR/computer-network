#include "client.h"

int main(int argc, char *argv[])
{
    char command[N];
    // 将＆addr中的前sizeof(addr)字节置为0(包括'\0')
    bzero(&addr, sizeof(addr));  
    // AF_INET代表TCP／IP协议   
    addr.sin_family = AF_INET;
    // 将点间隔地址转换为网络字节顺序      
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    // 转换为网络字节顺序
    addr.sin_port = htons(8989);    
    int len = sizeof(addr);

    while(1)
    {
        printf("ftp>");
        bzero(command,N);
        //fgets函数从stdin流中读取N-1个字符放入commd中
        if(fgets(command,N,stdin) == NULL)
        {
            printf("Fgets Error!\n");
            return -1;
        }

        command[strlen(command)-1]='\0';    //fgets函数读取的最后一个字符为换行符，此处将其替换为'\0'

        printf("Input Command Is [ %s ]\n",command);

        if(strncmp(command,"help",4) == 0) //比较两个字符串前4个字节，若相等则返回0
        {
            commandHelp();
        }else if(strncmp(command, "exit",4) == 0)
        {
            commandExit();
            exit(0);   //结束进程
        }else if(strncmp(command, "ls" , 2) == 0)
        {
            commandFileList(addr, command);
        }else if(strncmp(command, "get" , 3) == 0)
        {
            commandDownload(addr, command);
        }else if(strncmp(command, "put", 3) ==0 )
        {
            commandUpload(addr, command);
        }else
        {
            printf("Command Is Error!Please Try Again!\n");
        }

    }
    return 0;
}
