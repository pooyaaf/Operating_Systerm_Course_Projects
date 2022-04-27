#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <charconv>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <errno.h>

#define IS_VALID_DATA "VALID"
#define FIFO_ADDRESS "/tmp/myfifo"
#define CURR_FOLDER "./"
#define IN_FOLDER "/"
#define AVERAGE "./average.out" // each lesson would use this to average
#define CLASS "./class.out"     // each class would use this to prepare data
#define STUDENT "./std.out"
#define DEL ','

#define MEDU_ID 14
#define SIZE 1024
#define COURSE_NUM 5

