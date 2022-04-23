#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>



int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf(" Not enough arguments!\n");
        exit(1);
    }
    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    int tasks = atoi(argv[2]);

    char *buf = (char *)malloc(1000 * sizeof(char));

    int temp;
    if (temp = read(readPipe, buf, 1000) > 0)
    {
        // printf("course says :%s\n", buf);
        // printf("%s\n", buf);
        if (mkfifo(buf, 0666) == -1)
        {
            if (errno != EEXIST)
            {
                printf("Could not create physics FIFO");
                return 1;
            }
        }
        FILE *fd = fopen(buf, "r+");
        if (fd == NULL)
            printf("NULLLLL ...  IN course file\n");
        // printf("%s\n", buf);

        char x[100];
        int count = 0; 
        double sum = 0;
        char *eptr;
        while (fscanf(fd,"%s",x)>0)
        {

            //printf("khoond : ");
            sum += strtod(x,&eptr);
            // printf("%s\n", x);
            count++;
        }
        printf("%s result :%.3f\n",buf,sum / count);
        fclose(fd);
    }

    return 0;
}