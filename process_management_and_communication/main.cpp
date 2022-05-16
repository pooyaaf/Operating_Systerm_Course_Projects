#include "defines.hpp"

using namespace std;

// Global variable
int classFD[2]; // unnamed
int FDpipe;     // named
string ClassDIR;
//
vector<double> avg(COURSE_NUM, 0);
const string courses[] = {"Physics", "English", "Math", "Literature", "Chemistry"};
// Function
int countClasses(char *);
void createPipe();
void forkInitChildren(int, char *);
vector<string> split(string, char);

int main(int argc, char *argv[])
{
    string getBaseDir = argv[1];
    int classCount = countClasses(argv[1]);
    ClassDIR = CURR_FOLDER + getBaseDir + IN_FOLDER + "class" + "i" + IN_FOLDER;
    char addr[ClassDIR.length() + 1];
    strcpy(addr, ClassDIR.c_str());
    // printf("%s\n",addr);
    // create pipe
    createPipe();
    // forkInitChildren
    forkInitChildren(classCount, addr);

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
            strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name, ".DS_Store") == 0)
            continue;
        ++count;
    }
    closedir(dir_ptr);
    return count;
}
void createPipe()
{
    /*Create pipes*/
    if (pipe(classFD) == -1)
        printf("While opening the pipe an error occured.\n");

    if (mkfifo(FIFO_ADDRESS, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Could not create named pipe ");
        }
    }

    FDpipe = open(FIFO_ADDRESS, O_RDWR);
}
void forkInitChildren(int classCount, char *addr)
{
    // first create class childs then create course childs
    
    // class childs
    for (int i = 1; i <= classCount; i++)
    {
        pid_t id = fork();
        if (id == 0) // child
        {
            close(classFD[0]);
            char buffer[2], buffer_[3];
            sprintf(buffer, "%d", classFD[1]); // is write
            // printf("1:%s\n", buffer);
            sprintf(buffer_, "%d", FDpipe);
            // printf("2:%s\n", buffer_);
            char addr_[] = CLASS;
            addr[MEDU_ID] = i + '0';
            char *args[] = {&addr_[0], buffer, buffer_, addr, NULL}; // - - > send exec - pipe - Npipe  - (1 - 3)
            // printf("arg 1:%s\n", &addr_[0]);
            // printf("arg 2:%s\n", buffer);
            // printf("arg 3:%s\n", buffer_);

            execvp(CLASS, args);
            close(classFD[1]);
        }
        else // parent
        {
            char buff[SIZE];
            if (read(classFD[0], buff, SIZE) == -1)
                printf("Error in reading student classFD.\n");
            string buff_ = buff;
            /*
            Extract scores form table: 
            if Vallid - > add each course grade in its avg .
            */
            if (buff_.substr(0, 5) == IS_VALID_DATA)
            {
                vector<string> splited = split(buff_, ' ');
                for (int j = 1; j <= COURSE_NUM; j++)
                    avg[j - 1] += stod(splited[j]);
            }
        }
        close(id);
    }
    //--
    // course childs
    pid_t childPid;
    int s = 0;
    while ((childPid = wait(&s)) > 0)
        ;

    for (int i = 0; i < COURSE_NUM; i++)
    {
        pid_t id_ = fork();
        if (id_ == 0)
        {
            string sum = to_string(avg[i]);
            string c = to_string(classCount);
            string lesson = "";
            if (i == 0)
                lesson = courses[0];
            else if (i == 1)
                lesson = courses[1];
            else if (i == 2)
                lesson = courses[2];
            else if (i == 3)
                lesson = courses[3];
            else if (i == 4)
                lesson = courses[4];
            char buffer[sum.length() + 1], buffer_[c.length() + 1], _buffer_[lesson.length() + 1];
            strcpy(buffer, sum.c_str());
            strcpy(buffer_, c.c_str());
            strcpy(_buffer_, lesson.c_str());
            char addr[] = AVERAGE;
            char *args[] = {&addr[0], buffer, buffer_, _buffer_, NULL};
            execvp(args[0], args);
        }
        else
        {
            pid_t childPid;
            int s = 0;
            while ((childPid = wait(&s)) > 0)
                ;
        }
    }

    close(classFD[0]);
    close(classFD[1]);
    close(FDpipe);
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