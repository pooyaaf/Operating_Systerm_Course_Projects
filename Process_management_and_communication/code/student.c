#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Student: Not enough arguments!\n");
        exit(1);
    }
    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    close(writePipe);
    // Getting name of the class
    char *buf = (char *)calloc(1000, sizeof(char));

    int temp;

    while ((temp = read(readPipe, buf, 1000)) > 0)
    {

        // printf("Student with pid : %d read from pip: %s\n", getpid(), buf);
    }
    return 0;
}