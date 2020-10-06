#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>

// 文件传输的最大长度为256字节
#define N 256

void commandFileList(int);
void commandDownload(int, char *);
void commandUpload(int, char *);