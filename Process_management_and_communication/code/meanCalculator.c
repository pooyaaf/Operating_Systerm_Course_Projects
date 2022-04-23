#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

int childsCount = 0; // number of processes
int classCount = 0;  // no of classes in the school

int parentToClass[2];   // pipe between parent -> class
int parentToLessons[2]; // pipe between parent <- lessons

// calculate number of classes
int countClasses(char *);
// creating children for each class + creating children for 5 lessons
void forkInitChildren(void);
// create pipes between parent and children
void createPipes(void);

/* Implicit functions*/
char *intToSrc(int value)
{
    char *buffer = calloc(0, sizeof(char));
    snprintf(buffer, 10, "%d", value);
    return buffer;
}

int main(int argc, char *argv[])
{

    classCount = countClasses(argv[1]);
    // printf("%d\n",classCount);
    createPipes();
    forkInitChildren();
    return 0;
}

int countClasses(char *path)
{
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    if ((dir_ptr = opendir(path)) == NULL)
        return 0;

    int count = 0;
    while ((direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name, ".") == 0 ||
            strcmp(direntp->d_name, "..") == 0)
            continue;
        ++count;
    }
    closedir(dir_ptr);
    return count;
}
//
void createPipes(void)
{
    if (pipe(parentToClass) < 0)
    {
        printf("Error in pipe parentToClass\n");
        exit(1);
    }
    if (pipe(parentToLessons) < 0)
    {
        printf("Error in pipe parentToLessonst\n");
        exit(1);
    }
}

//
void createClass(void)
{
    // printf("exec Mapper!\n");
    char *args[] = {intToSrc(parentToClass[0]), intToSrc(parentToClass[1]), NULL};
    execv("./class", args);
}

void createCourse(void)
{
    char *snum = intToSrc(5);
    char *args[] = {intToSrc(parentToLessons[0]), intToSrc(parentToLessons[1]), snum, NULL};
    // printf("%s %s %s %s\n", args[0], args[1], args[2], args[3]);
    execv("./course", args);
}

//
char *getName(int num)
{
    char *output = "./school/";
    char *output2 = "class";
    char *snum = intToSrc(num);
    //  printf("NYMM: %s\n", snum);
    char *name = (char *)malloc(strlen(output) + strlen(output2) + strlen(snum));
    strcpy(name, output);
    // printf("1ADDRESSS: %s\n", name);
    strcat(name, output2);
    // printf("2ADDRESSS: %s\n", name);
    strcat(name, snum);
    // printf("3ADDRESSS: %s\n", name);
    name[strlen(name)] = '\0';
    // printf("4ADDRESSS: %s\n", name);
    // printf("%s\n",name);
    return name;
}

void assignProcess(void)
{
    
    // class
    close(parentToClass[0]); // close read
    int count = classCount;
    while (count != 0)
    {
        // getName of files
        char *buf = getName(count);
        // printf("%s\n", buf);
        int i = write(parentToClass[1], buf, strlen(buf));
        // printf("status: %d\n", i);
        count -= 1;

        sleep(1);
    }
    close(parentToClass[1]);
    // course
    close(parentToLessons[0]);
    write(parentToLessons[1],"./Course/Physics",sizeof("./Course/Physics"));
    sleep(1);
    write(parentToLessons[1],"./Course/English",sizeof("./Course/English"));
    sleep(1);
    write(parentToLessons[1],"./Course/Math",sizeof("./Course/Math"));
    sleep(1);
    write(parentToLessons[1],"./Course/Literature",sizeof("./Course/Literature"));
    sleep(1);
    write(parentToLessons[1],"./Course/Chemistry",sizeof("./Course/Chemistry"));
    sleep(1);
    close(parentToLessons[1]);
}

//
void forkInitChildren(void)
{
    int parentPid = fork();
    if (parentPid < 0)
    {
        printf("Error in Fork\n");
        exit(1);
    }
    int pid = getpid();
    if (parentPid > 0)
    {
        // printf("parent process with pid : %d\n", pid);

        if (childsCount < classCount + 5) // plus reducer => childrenCount
        {
            childsCount += 1;
            forkInitChildren();
        }
        else
        {
            assignProcess();
            sleep(1); // wait until child forking done

            wait(NULL); // all children finish their job
            printf("Done!\n");
            exit(0);
        }
    }
    else
    {
        if (childsCount == classCount + 5)
        {
            printf("All children forked.\n");
        }
        else
        {
            if (childsCount < classCount)
            {
                // printf("Create Class with pid: %d\n", pid);
                createClass();
            }
            else
            {
                // printf("Create Course with pid: %d\n", pid);
                createCourse();
            }
        }
    }
}
