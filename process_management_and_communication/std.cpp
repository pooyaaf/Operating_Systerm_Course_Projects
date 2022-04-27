#include "defines.hpp"

using namespace std;
// global
char result[SIZE];
// function
vector<double> getScores(char *);
void createTable(string, vector<double>);

int main(int argc, char *argv[])
{
    string stdPath = argv[1];
    string stdName = argv[2];
    int writeToClass = stoi(argv[3]); // write

    char addr[SIZE];
    // getScores
    vector<double> studentScores = getScores(argv[1]);
    //  table
    createTable(stdName, studentScores);
    // send back result to the class 
    write(writeToClass, result, strlen(result));
    close(writeToClass);

    return 0;
}
vector<double> getScores(char *addr)
{
    vector<double> scores(5, 0);
    string line, word;
    const string courses[] = {"Physics", "English", "Math", "Literature", "Chemistry"};
    fstream file(addr, ios::in);
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
void createTable(string studentName, vector<double> studentScores)
{
    string res = "";
    res += "VALID ";

    for (int j = 0; j < COURSE_NUM; j++)
        res += to_string(studentScores[j]) + " ";
    res += studentName.substr(0, studentName.size() - 4) + " \n";
    // cout << res << endl;

    strcpy(result, res.c_str());
}