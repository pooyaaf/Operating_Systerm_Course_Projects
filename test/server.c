#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "constant.h"

#define LocalPort 8082
int cnt = 0;
int setupServer(int port)
{
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0); //TCP

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Error in bind()\n");
        return -1;
    }
    if (listen(server_fd, MAX_REQUEST) < 0)
    {
        printf("Error in listen()\n");
        return -1;
    }

    return server_fd;
}

int acceptClient(int server_fd)
{
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
    return client_fd;
}
///
int numClient[2][50]={0}; // 50 room each room 2 player max
int numOnline[50]={0}; // no of  online players in 50 rooms
int availablePort = LocalPort+1;
int trace = 0 ; // first available room for player 
int avroom[50]={0};

int checkClientType(char buffer[], int clientFD, int type)
{
    if (type > 0 && type < 3)
    {
        printf("client %d type is: %d \n", clientFD, type);
        if(type == 1){
            // client fd is a player
         
            numClient[numOnline[trace]][trace]=clientFD; // put client in first empty room
            //printf("debug : %d\n",numClient[numOnline[trace]][trace]);
            numOnline[trace] += 1 ; // add online num in the room               
           if(numOnline[trace] == 2){
                printf("!!!Hooray %d & %d  Match found!New port generated\n", numClient[0][trace], numClient[1][trace]);
                //  printf("debug %s\n", buffer);
                //fill the buffer with <- port , client[0] , client[1]
                sprintf(buffer, "%d %d %d", availablePort,  numClient[0][trace], numClient[1][trace]);
                avroom[trace]=availablePort;
                trace += 1; // move to the next room
                availablePort += 1;
                return 1;
           }
          
        }
        else{
            //client fd is a viewer
                //show list available rooms:
                
        }
    }
    else
    {
        //sprintf(buffer, "You should choose between :(1 & 2) id \n");
        char text[] ="You should choose between :(1 & 2) id \n";
        send(clientFD, text, strlen(text), 0);
    }
    return 0;
}
///

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, max_sd;
    char buffer[1024] = {0};
    fd_set master_set, working_set;

    if (argv[1] > 0)
    {
        server_fd = setupServer(atoi(argv[1]));
        printf("Server is running on port %s... \n ", argv[1]);
    }
    else{
        server_fd = setupServer(LocalPort);
        printf("Server is running ... \n");   
    }
        

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);

   

    int basePort = LocalPort + 1;

    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                
                if (i == server_fd) {  // new clinet
                    new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                    printf("New client connected. fd = %d\n", new_socket);
                    sprintf(buffer, "%d \n Select your type with number : \n (1- Player && 2- Spectator) : \n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);
                    memset(buffer, 0, 1024);
                }
                
                else { // client sending msg
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    printf("Buffer -:%s\n",buffer);
                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                   
                    int type = atoi(&buffer[0]);    
                    if(checkClientType(buffer,i,type)==1){
                        for(int i=0;i<2;i++){
                            printf("debug %s\n", buffer);
                            send(numClient[i][trace-1], buffer, strlen(buffer), 0);
                        }
                        printf("Success!\n");
                        
    
                    }
                  
                    else if(type == 2){
                        
                        for(int j=0;j<50;j++)
                        {
                            if(avroom[j]>0){
                                printf("room %d is available!\n" ,avroom[j]);
                                send(i, &avroom[j], sizeof(int), 0);
                            }
                            
                        }
                        int a=0;
                        send(i,&a,sizeof(int),0);
                        char watchRoom;
                        recv(i , &watchRoom, sizeof(watchRoom), 0);
                        printf("Client wants to connect to room port : %d\n",watchRoom-(CTOINT)+(LocalPort));
                    }
                    else if(type==4){
                        //log :
                        printf("Creating Log ... Buffer : %s\n",buffer);
                        recv(i , buffer, 9, 0);
                        if(cnt%2 ==0){
                        int new_fd;
                        new_fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR, 0664);
                        write(new_fd, &buffer[1], 1);
                        write(new_fd, " ", 1);
                        write(new_fd, &buffer[2], 1);
                        write(new_fd, " ", 1);
                        write(new_fd, &buffer[3], 1);
                        write(new_fd, "\n", 1);
                        write(new_fd, &buffer[4], 1);
                        write(new_fd, " ", 1);
                        write(new_fd, &buffer[5], 1);
                        write(new_fd, " ", 1);
                        write(new_fd, &buffer[6], 1);
                        write(new_fd, "\n", 1);
                        write(new_fd, &buffer[7], 1);
                        write(new_fd, " ", 1);
                        write(new_fd, &buffer[8], 1);
                        write(new_fd, " ", 1);
                        write(new_fd, &buffer[9], 1);
                        printf("%d\n",i);
                        write(new_fd, "\n---\n", 5);
                        close(new_fd);
                        }cnt++;
                        
                    }
                    memset(buffer, 0, 1024);
                }
                
            }
        }
    }
    
    return 0;
}