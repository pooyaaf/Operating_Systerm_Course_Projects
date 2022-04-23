#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
/*
create 5 FIFO (Named pipe):
:Physics
:English
:Math
:Literature
:Chemistry
Send data to the course.c
*/

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

    //
    // if(mkfifo("Physics",0777) == -1)
    // {
    //     if(errno != EEXIST)
    //     {
    //         printf("Could not create physics FIFO");
    //         return 1;
    //     }
    // }
    // int fd_physics = open("Physics", O_CREAT | O_APPEND | O_RDWR);
    // int x= 9;
    // write(fd_physics,&x,sizeof(x));
    // close(fd_physics);
    //
    int temp;

    if ((temp = read(readPipe, buf, 1000)) > 0)
    {

        // printf("Student with pid : %d read from pip: %s\n", getpid(), buf);
        strcat(buf, ".csv"); // path create
        FILE *fp = fopen(buf, "r");

        char buffer[1024];

        int row = 0;
        int column = 0;

        while (fgets(buffer,
                     1024, fp))
        {
            column = 0;
            row++;

            // To avoid printing of column
            // names in file can be changed
            // according to need
            // if (row == 1)
            //     continue;

            // Splitting the data
            char *value = strtok(buffer, ", \n");
            // --
            FILE *filed_dec;
            while (value)
            {
                int fd;

                // Column 1
                if (column == 0)
                {
                    // if (mkfifo(value, 0777) == -1)
                    // {
                    //     if (errno != EEXIST)
                    //     {
                    //         printf("Could not create %s FIFO", value);
                    //         return 1;
                    //     }
                    // }

                    char shit1[100];
                    strcpy(shit1, "./Course/");
                    strcat(shit1, value);
                    filed_dec = fopen(shit1, "a");
                    // printf("file Desc writes shit1:%s \n", shit1);

                    // printf("Course Title :%s,",value);
                }

                // Column 2
                if (column == 1)
                {
                    // printf(" Grade :%s\n",value);
                    // char *x = "oops!";
                    // printf("%d\n",fd);
                    // write(fd, x, strlen(x));
                    // close(fd);

                    // printf("%s\n", value);
                    fprintf(filed_dec, "%s\n", value);

                    // printf("done .. \n");
                    fclose(filed_dec);
                }

                // printf("%s", value);
                value = strtok(NULL, ", \n");
                column++;
            }
        }
        // printf("\n---\n");
        fclose(fp);
        // int fd = open("test", O_WRONLY);
        // write(fd, "test1", strlen("test1"));
        // close(fd);
    }
    exit(0);
    return 0;
}