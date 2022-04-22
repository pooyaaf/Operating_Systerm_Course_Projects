#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

int noOfClassStudents = 0;
int childsCount = 0;
int classToStudent[2];
char *buf;

void readArgs(char **, char **);

int countStudents(char *, char **);
// create pipes between parent and students
void createPipes(void);
// creating children for each class
void forkInitChildren(char **);
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
    char *studentNames[100]; // Student names
    readArgs(argv, studentNames);
    createPipes();

    forkInitChildren(studentNames);
    exit(0);
    return 0;
}
int countStudents(char *path, char **studentNames)
{
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    if ((dir_ptr = opendir(path)) == NULL)
        return 0;

    int count = 0;
    //
    char *name;
    char *format;

    while ((direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name, ".") == 0 ||
            strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name, ".DS_Store") == 0)
            continue;
        // printf("name:%s\n",direntp->d_name);
        name = strtok(direntp->d_name, ".");
        format = strtok(NULL, ".");

        if (strcmp(format, "csv") == 0)
        {
            studentNames[count] = malloc(strlen(name));
            strcpy(studentNames[count], name);
            count++;
        }
    }
    closedir(dir_ptr);
    return count;
}
//
void readArgs(char *argv[], char **studentNames)
{
    int readPipe = atoi(argv[0]);
    int writePipe = atoi(argv[1]);
    close(writePipe);
    // Getting name of the class
    buf = (char *)malloc(1000 * sizeof(char));
    int temp;

    if (temp = read(readPipe, buf, 1000) > 0)
    {
        // printf("%s\n",buf);
        noOfClassStudents = countStudents(buf, studentNames);
        // printf("Class with pid : %d read from pipe: %s, has %d number of students\n", getpid(), buf, noOfClassStudents);
        // printf("Class in the path %s, has %d number of students\n", buf, noOfClassStudents);
        // for (int i = 0; i < noOfClassStudents; i++)
        // {
        //     printf("name %d: %s\n", i, studentNames[i]);
        // }
        // printf("\n---\n");
        // printf("%s\n", studentNames[0]);
    }
}
//
void createPipes(void)
{
    if (pipe(classToStudent) < 0)
    {
        printf("Error in pipe classToStudent\n");
        exit(1);
    }
}
//
//
char *getName(char *studentName)
{

    //  printf("NYMM: %s\n", snum);
    //printf("-->%s\n", buf);
    char *name = (char *)malloc(strlen(buf) + 1 +strlen(studentName));
    strcpy(name, buf);
    // printf("1ADDRESSS: %s\n", name);
    strcat(name,"/");
    strcat(name, studentName);

    // printf("3ADDRESSS: %s\n", name);
    name[strlen(name)] = '\0';
    // printf("4ADDRESSS: %s\n", name);
    // printf("%s\n",name);
    return name;
}

void assignProcess(char **studentNames)
{
    // class
    close(classToStudent[0]); // close read
    int count = noOfClassStudents - 1;
    while (count >= 0)
    {
        // getName of files
        char *buffer = getName(studentNames[count]);
        // printf("buffer :%s\n",buf);
        //  printf("%s\n", buf);
        int i = write(classToStudent[1], buffer, strlen(buffer));
        // printf("status: %d\n", i);
        count -= 1;

        sleep(1);
    }
    close(classToStudent[1]);
}

void createStudents(void)
{
    // printf("exec Mapper!\n");
    char *args[] = {intToSrc(classToStudent[0]), intToSrc(classToStudent[1]), NULL};
    execv("./student", args);
}
//
void forkInitChildren(char **studentNames)
{
    int parentPid = fork();
    if (parentPid < 0)
    {
        printf("Error in Fork in class file\n");
        exit(1);
    }
    int pid = getpid();
    if (parentPid > 0)
    {
        // printf("parent process with pid : %d\n", pid);

        if (childsCount < noOfClassStudents) // plus reducer => childrenCount
        {
            childsCount += 1;
            forkInitChildren(studentNames);
        }
        else
        {
            assignProcess(studentNames);
            sleep(1);   // wait until child forking done
            wait(NULL); // all children finish their job
            // printf("Done!Creating students for class ... \n");
            exit(0);
        }
    }
    else
    {
        if (childsCount == noOfClassStudents)
        {
            // printf("All students in the class forked.\n");
        }
        else
        {

            createStudents();
        }
    }
}
