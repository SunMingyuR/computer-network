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