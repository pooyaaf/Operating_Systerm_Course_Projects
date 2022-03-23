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
#include "constant.h"

#define LocalPort 8082
// server file:
int fd;
///
char board[3][3];
const char PLAYER = 'X';
const char COMPUTER = 'O';
int stepIndex =0 ;

void resetBoard()
{
   for(int i = 0; i < 3; i++)
   {
      for(int j = 0; j < 3; j++)
      {
         board[i][j] = ' ';
      }
   }
}
void printBoard()
{
  
   printf(" %c | %c | %c ", board[0][0], board[0][1], board[0][2]);
   printf("\n---|---|---\n");
   printf(" %c | %c | %c ", board[1][0], board[1][1], board[1][2]);
   printf("\n---|---|---\n");
   printf(" %c | %c | %c ", board[2][0], board[2][1], board[2][2]);
   printf("\n");
}
int checkFreeSpaces()
{
   int freeSpaces = 9;

   for(int i = 0; i < 3; i++)
   {
      for(int j = 0; j < 3; j++)
      {
         if(board[i][j] != ' ')
         {
            freeSpaces--;
         }
      }
   }
   return freeSpaces;
}
int checkWinner(int step)
{
   //check rows
   for(int i = 0; i < 3; i++)
   {
      if(board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ')
      {
       if(step%2==0)
          return 2;
         else{
             return 1;
         }
      }
   }
    //check columns
   for(int i = 0; i < 3; i++)
   {
      if(board[0][i] == board[1][i] && board[0][i] == board[2][i] && board[2][i] != ' ')
      {
      if(step%2==0)
          return 2;
         else{
             return 1;
         }
      }
   }
   //check diagonals
   if(board[0][0] == board[1][1] && board[0][0] == board[2][2] && board[2][2] != ' ' )
   {
       if(step%2==0)
          return 2;
         else{
             return 1;
         }
   }
   if(board[0][2] == board[1][1] && board[0][2] == board[2][0] && board[0][2] !=' ')
   {
     if(step%2==0)
          return 2;
         else{
             return 1;
         }
   }


   return 0;
}

///
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


//UDP :
// int turns[9][2]={
//     {1,0},
//     {0,1},
//     {1,0},
//     {0,1},
//     {1,0},
//     {0,1},
//     {1,0},
//     {0,1},
//     {1,0}
// };

void alarm_handler(int sig)
{
    printf("Times out!\n");
}

void Groom(int t1, int t2,int id)
{
    //UDP set-up
    int sock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bc_address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(roomPort);
    bc_address.sin_addr.s_addr = inet_addr("192.168.1.255");

    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    // 
   // int turns[9]={1,0,1,0,1,0,1,0,1}; // X - O
     int turns[9][2] = {
        {t1, t2},
        {t2, t1},
        {t1, t2},
        {t2, t1},
        {t1, t2},
        {t2, t1},
        {t1, t2},
        {t2, t1},
        {t1, t2}
        };
    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    int indexAnswer = 0;
     int winner = 0;
     char response = ' ';
    // steps[CSTEPS][100];
    
    resetBoard();
    for (int j = 0; j < 9; j++)
    {
        if(winner == 0 && checkFreeSpaces() != 0)
        {
                        //printBoard();
                        if (turns[j][0] == id)
                        {
                        memset(buffer, 0, 1024);
                        printf("Your Turn\n");
                        int read_ret = 0;
                        // -- player Move
                        if (j % 2 == 0)
                        {
                            printf("You have 60 seconds to play,player 1:\n Enter row(1-3)+space+col(1-3):\n");
                            alarm(60);
                            read_ret = read(0, buffer, 1024);
                            // ASCII TB 1 -> 49
                          
                            alarm(0);
                            printf("\n");
                        }
                        // -- computer Move
                        else
                        {
                            printf("You have 60 seconds to play,player 2:\n Enter row(1-3)+space+col(1-3):\n");
                            alarm(60);
                            read_ret = read(0, buffer, 1024);
                            // ASCII TB 1 -> 49
                         
                            alarm(0);
                            printf("\n");
                        }
                        if (read_ret < 0)
                        {
                            sprintf(buffer, "Time is over for %d\n", id);
                        }
                        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));
                        recv(sock, buffer, 1024, 0);

                   
                        if(sock%2==0)
                            board[buffer[0] - '0'-1][buffer[2] - '0'-1] = PLAYER;
                        else
                            board[buffer[0] - '0'-1][buffer[2] - '0'-1] = COMPUTER;
                        
                 
                    }
                    else
                    {
                        memset(buffer, 0, 1024);
                        printf("Turn for others!\nWait...\n\n");
                        recv(sock, buffer, 1024, 0);
                        if(sock%2==0)
                            board[buffer[0] - '0'-1][buffer[2] - '0'-1] = COMPUTER;
                        else
                            board[buffer[0] - '0'-1][buffer[2] - '0'-1] = PLAYER;
                       
                        printf("%s\n", buffer);
                    }   
                    stepIndex++;
                    //
                    winner = checkWinner(stepIndex);
                        //send(sock, &winner, sizeof(int), 0);
                        //recv(sock, &winner,sizeof(int),0);
                     printBoard(); 
                     if(winner>0){
                        printf("winner is player%d\n",winner); 
                   
                     }
                                
        }
        
    }
    
}
//
int main(int argc, char const *argv[])
{
  
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
    int id = atoi(&buff[0]);
    // printf("buffer --> %s",&buff[0]);
   // printf("buffer --> %d\n",id);
    printf("Server said, you're client %s ", buff);
    memset(buff, 0, 1024);
    int turn1, turn2;
    int type;
    int port;
    while (1)
    {
        read(0, buff, 1024); // get client type in shell
        send(fd, buff, strlen(buff), 0);//send type client to server
        //
        type=buff[0];
        type = type - CTOINT;
        if(type==2){
            printf("List available ports:\n");
            read(fd,&port,sizeof(port));
            while(port > 0 ){
                printf("port: %d \n",port);
                read(fd,&port,sizeof(port));
            }
            return 0;
        }
        //
        printf("Wait for Server response!\n");
        printf("roomport is: %d\n",roomPort);
        recv(fd, buff, 1024, 0);
        roomPort = atoi(&buff[0]);
        if (roomPort > LocalPort)
        {
            turn1 = atoi(&buff[4]);
            turn2 = atoi(&buff[6]);
            printf("Server said: on Port %d:\n Welcome to your game room! \n ", roomPort);
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
        printf("Players are: %d %d! \n ", turn1, turn2);
        printf("__________________________ \n ");
        Groom(turn1,turn2,id);
    }
    return 0;
}