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

    if ((temp = read(readPipe, buf, 1000)) > 0)
    {

        //printf("Student with pid : %d read from pip: %s\n", getpid(), buf);
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
            if (row == 1)
                continue;

            // Splitting the data
            char *value = strtok(buffer, ", \n");

            while (value)
            {
                // Column 1
                if (column == 0)
                {
                    printf("Course Title :");
                }

                // Column 2
                if (column == 1)
                {
                    printf(" Grade :");
                }

               
                printf("%s", value);
                value = strtok(NULL, ", \n");
                column++;
            }

            printf("\n");
        }

        fclose(fp);
    }
    exit(0);
    return 0;
}