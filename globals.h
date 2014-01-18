#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

/* Global Variables */
#define BUFSIZE 1024
static const int MAXPENDING = 5; // Maximum outstanding connection requests

/* Prototypes */
int   downloadFile(const int fromSocket, const char* fileName);
int   uploadFile(const int toSocket, const char* fileName);
char* toUpper(char* str);
void  dieWithUserMessage(const char* msg, const char *detail);
void  dieWithSystemMessage(const char* msg);
int   setupTCPClientSocket(const char* host, const char* service);
int   setupTCPServerSocket(const char* service);
int   acceptTCPConnection(const int servSock);
void  printSocketAddress(const struct sockaddr* address, FILE* stream);
