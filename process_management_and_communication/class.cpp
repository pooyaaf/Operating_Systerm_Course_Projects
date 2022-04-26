#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

#define SIZE 1024
#define COURSE_NUM 5
#define DEL ','

#define STUDENT "./std.out"
#define FIFO_ADDRESS "/tmp/myfifo"
#define IS_VALID_DATA "isSTD"

using namespace std;
// Global
int studentFD[2];
int FDpipe_;
//
vector<double> scores(COURSE_NUM, 0);
vector<double> avg(COURSE_NUM, 0);
// Function
void createPipe();
void forkInitChildren(int, char *, int, int, vector<string>);
vector<string> split(string, char);
vector<double> getScores(char *);
vector<string> getStudentCSV(char *);

int main(int argc, char *argv[])
{

    int unnamedPipe = stoi(argv[1]);
    int namedPipeFd = stoi(argv[2]);
    char address[SIZE];
    strcpy(address, argv[3]);
    // printf("%s\n",argv[3]);
    // vector of student of each class.
    vector<string> files = getStudentCSV(address);

    int studentCount = files.size(); // 4 3 4

    //
    createPipe();

    forkInitChildren(studentCount, address, unnamedPipe, namedPipeFd, files);
    return 0;
}

vector<string> getStudentCSV(char *address)
{
    vector<string> files;
    // int counter = 0;
    DIR *dirp;
    struct dirent *entry;
    dirp = opendir(address);
    while ((entry = readdir(dirp)) != NULL)
        if (entry->d_type == DT_REG)
        {
            string name = entry->d_name;
            if (name == "." || name == ".." || name == ".DS_Store")
                continue;

            // counter++;
            files.push_back(name);
        }
    closedir(dirp);
    return files;
}
void createPipe()
{

    if (pipe(studentFD) == -1)
        printf("In medu while opening the pipe an error occured.\n");

    if (mkfifo(FIFO_ADDRESS, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Could not create named pipe ");
        }
    }
    FDpipe_ = open(FIFO_ADDRESS, O_RDWR);
}
void forkInitChildren(int studentCount, char *address, int unnamedPipe, int namedPipeFd, vector<string> files)
{
    for (int i = 1; i <= studentCount; i++)
    {
        pid_t id = fork();
        if (id == 0)
        {
            char studentPath[2 * SIZE];
            strcpy(studentPath, address);
            strcat(studentPath, files[i - 1].c_str());

            char clsToStd[2];

            sprintf(clsToStd, "%d", studentFD[1]);

            char STD[SIZE]; // name.csv
            strcpy(STD, files[i - 1].c_str());
            char addr_[] = STUDENT;
            
            char *args[] = {&addr_[0], studentPath, STD, clsToStd, NULL};
            execvp(args[0], args);
            close(studentFD[1]);
        }
        else
        {
            char buff[SIZE];
            if (read(studentFD[0], buff, SIZE) == -1)
                printf("Error in reading student fd.\n");
            string buff_ = buff;
            if (buff_.substr(0, 5) == IS_VALID_DATA)
            {
                vector<string> splited = split(buff_, ' ');
                for (int j = 1; j <= 5; j++)
                    avg[j - 1] += stod(splited[j]);
            }
        }
        close(id);
    }

    pid_t childPid;
    int s = 0;
    while ((childPid = wait(&s)) > 0)
        ;

    string sol = "isSTD ";

    for (int j = 0; j < 5; j++)
    {
        sol += to_string(avg[j] /= studentCount) + " ";
    }
    sol += "\n";

    char key[SIZE];
    strcpy(key, sol.c_str());

    write(unnamedPipe, key, strlen(key));
    close(unnamedPipe);

    close(studentFD[0]);
    close(studentFD[1]);
    close(FDpipe_);
}
vector<string> split(string str, char splitor)
{
    vector<string> elems;
    stringstream ss(str);
    string item;
    while (getline(ss, item, splitor))
        elems.push_back(item);
    return elems;
}
vector<double> getScores(char *address)
{
    const string courses[] = {"Physics", "English", "Math", "Literature", "Chemistry"};

    vector<double> scores(COURSE_NUM, 0);
    string line, word;

    fstream file(address, ios::in);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            int stringDelimeter = line.find(DEL);
            string lesson = line.substr(0, stringDelimeter);
            string score = line.substr(stringDelimeter + 1);

            if (lesson == courses[0])
                scores[0] = stod(score);
            else if (lesson == courses[1])
                scores[1] = stod(score);
            else if (lesson == courses[2])
                scores[2] = stod(score);
            else if (lesson == courses[3])
                scores[3] = stod(score);
            else if (lesson == courses[4])
                scores[4] = stod(score);
        }
    }
    return scores;
}