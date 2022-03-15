#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>

#define LocalPort 8082
int roomPort = 0;
int connectServer(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        printf("Error in connecting to server\nTry another port\n");
        return -1;
    }

    return fd;
}
int main(int argc, char const *argv[])
{
    int fd;
    char buff[1024] = {0};

    if (argv[1] > 0)
    {
        fd = connectServer(atoi(argv[1]));
        printf("Port = %s\n for %d", argv[1], fd);
    }
    else
        fd = connectServer(LocalPort);

    if (fd < 0)
        return 0;

    recv(fd, buff, 1024, 0);
    printf("Server said: %s\n", buff);
    memset(buff, 0, 1024);
    int turn1, turn2;
    while (1)
    {
        read(0, buff, 1024);
        send(fd, buff, strlen(buff), 0);
        printf("Wait for Server response!\n");
        printf("roomport is: %d",roomPort);
        recv(fd, buff, 1024, 0);
        roomPort = atoi(&buff[0]);
        printf("roomport is: %d",roomPort);
        if (roomPort > LocalPort)
        {
            turn1 = atoi(&buff[4]);
            turn2 = atoi(&buff[6]);
            printf("Server said: on Port %d: Welcome to your question room! \n ", roomPort);
            break;
        }
        else
        {
            printf("Server said: %s\n", buff);
        }
        memset(buff, 0, 1024);
    }
    if (roomPort > 0)
    {
        printf("Turns: %d  %d! \n ", turn1, turn2);
    }

    return 0;
}