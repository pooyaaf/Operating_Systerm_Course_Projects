#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

int noOfClassStudents = 0;

int classToStudent[2];

int countStudents(char *);


/* Implicit functions*/
char *intToSrc(int value)
{
    char *buffer = calloc(0, sizeof(char));
    snprintf(buffer, 10, "%d", value);
    return buffer;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Class: Not enough arguments!\n");
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
        // printf("%s\n",buf);
        noOfClassStudents = countStudents(buf);
        // printf("Student with pid : %d read from pipe: %s\n", getpid(), buf);
        printf("Student in the path %s, has %d number of students\n", buf, noOfClassStudents);
    
    }

    return 0;
}
int countStudents(char *path)
{
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    if ((dir_ptr = opendir(path)) == NULL)
        return 0;

    int count = 0;
   
    while ((direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name, ".") == 0 ||
            strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name, ".DS_Store") == 0)
            continue;
        ++count;
    }
    closedir(dir_ptr);
    return count;
}
