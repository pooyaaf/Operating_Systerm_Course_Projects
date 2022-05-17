#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <vector>
#include <cmath>
#include <string.h>

using namespace std::chrono;
using namespace std;

#define NUM_THREADS_READ 5
#define NUM_THREADS_MEDIAN 8
#define NUM_THREADS_COLOR_REVERSE 5
#define NUM_THREADS_PLUS_FILTER 8

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;
//
float pic[2500][2500][3] = {0};
float out[2500][2500][3] = {0};
//
bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
    std::ifstream file(fileName);

    if (file)
    {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER)(&buffer[0]);
        info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        bufferSize = file_header->bfSize;
        return 1;
    }
    else
    {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return 0;
    }
}
//
struct picArguments
{
    int count, row, maxR, extra, end, id;
    char *fileReadBuffer;
};

void *getPixs(void *threadarg)
{
    struct picArguments *args;
    args = (struct picArguments *)threadarg;
    int count = args->count;

    for (int i = args->row; i < args->maxR; i++)
    {
        count += args->extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            for (int k = 0; k < 3; k++)
            {
                pic[i][j][k] = (unsigned char)args->fileReadBuffer[args->end - count];
                count++;
            }
        }
    }
    pthread_exit(NULL);
}

void threading(int num, int end, void *(*func)(void *), char *buf = new char[0])
{
    pthread_t threads[num];
    struct picArguments td[num + 1];
    int err;
    int parts = rows / num;
    int start = 0;
    int extra = cols % 4;
    int i;
    for (i = 0; i < num; i++)
    {
        td[i].end = end;
        td[i].extra = extra;
        td[i].row = start;
        td[i].maxR = start + parts;
        start += parts;
        td[i].count = cols * i * parts * 3 + 1;
        td[i].fileReadBuffer = buf;
        err = pthread_create(&threads[i], NULL, func, (void *)&td[i]);
        if (err)
        {
            cout << "Error:unable to create thread," << err << endl;
            exit(-1);
        }
    }

    if (rows % num != 0)
    {
        pthread_t thread;

        td[i].end = end;
        td[i].extra = extra;
        td[i].row = start;
        td[i].maxR = start + (rows % num);
        start += parts;
        td[i].count = cols * i * parts * 3 + 1;
        td[i].fileReadBuffer = buf;
        err = pthread_create(&thread, NULL, func, (void *)&td[i]);
        if (err)
        {
            cout << "Error:unable to create thread," << err << endl;
            exit(-1);
        }
        pthread_join(thread, NULL);
    }
    for (int i = 0; i < num; ++i)
    {
        pthread_join(threads[i], NULL);
    }
}
//
void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
{
    threading(NUM_THREADS_READ, end, getPixs, fileReadBuffer);
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
    std::ofstream write(nameOfFileToCreate);
    if (!write)
    {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
                case 0:
                    // write red value pic fileBuffer[bufferSize - count]
                    fileBuffer[bufferSize - count] = (unsigned char)out[i][j][k];
                    break;
                case 1:
                    // write green value pic fileBuffer[bufferSize - count]
                    fileBuffer[bufferSize - count] = (unsigned char)out[i][j][k];
                    break;
                case 2:
                    // write blue value pic fileBuffer[bufferSize - count]
                    fileBuffer[bufferSize - count] = (unsigned char)out[i][j][k];
                    break;
                    // go to the next position pic the buffer
                }
                count++;
            }
    }
    write.write(fileBuffer, bufferSize);
}

void flip_horizontally()
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            out[i][j][0] = pic[i][rows - j][0];
            out[i][j][1] = pic[i][rows - j][1];
            out[i][j][2] = pic[i][rows - j][2];
        }
}

void flip_vertically()
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            pic[i][j][0] = out[cols - i][j][0];
            pic[i][j][1] = out[cols - i][j][1];
            pic[i][j][2] = out[cols - i][j][2];
        }
}

short median_neighbers(int i, int j, int k)
{
    if (i + 1 >= rows || j + 1 >= cols || i - 1 < 0 || j - 1 < 0)
        return pic[i][j][k];

    int arr[9];
    int cnt = 0;
    for (int p = -1; p <= 1; p++)
    {
        for (int q = -1; q <= 1; q++)

        {
            arr[cnt] = (pic[i - p][j - q][k]);
            cnt++;
        }
    }

    // odd - size vector
    return arr[4];
}

void *median(void *thread_args)
{
    struct picArguments *args;
    args = (struct picArguments *)thread_args;
    //
    for (int i = args->row; i < args->maxR; i++)
        for (int j = 0; j < cols; j++)
        {
            out[i][j][0] = median_neighbers(i, j, 0);
            out[i][j][1] = median_neighbers(i, j, 1);
            out[i][j][2] = median_neighbers(i, j, 2);
        }
    //
    pthread_exit(NULL);
}
void median_filter()
{
    // threading
    threading(NUM_THREADS_MEDIAN, 0, median);
}

void *flip_color(void *thread_args)
{
    struct picArguments *args;
    args = (struct picArguments *)thread_args;
    //
    for (int i = args->row; i < args->maxR; i++)
        for (int j = 0; j < cols; j++)
        {
            out[i][j][0] = 255 - out[i][j][0];
            out[i][j][1] = 255 - out[i][j][1];
            out[i][j][2] = 255 - out[i][j][2];
        }
    //
    pthread_exit(NULL);
}
void flip_color_filter()
{
    // threading
    threading(NUM_THREADS_COLOR_REVERSE, 0, flip_color);
}
void *puls_calc(void *thread_args)
{
    struct picArguments *args;
    args = (struct picArguments *)thread_args;
    //
    for (int i = args->row; i < args->maxR; i++)
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k <= 2; k++)
            {
                if (j == cols / 2 || i == rows / 2)
                {
                    out[i][j][k] = 255.0;
                }
            }
        }
    //
    pthread_exit(NULL);
}
void plus_filter()
{
    // threading
    threading(NUM_THREADS_PLUS_FILTER, 0, puls_calc);
}

int main(int argc, char *argv[])
{
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    int argindex = 1;
    if (argc != 2)
        argindex++;
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
    {
        cout << "File read error" << endl;
        return 1;
    }
    auto start = high_resolution_clock::now();
    // apply filters
    if (argindex != 1)
    {
        auto start = high_resolution_clock::now();
        auto start1 = high_resolution_clock::now();
        getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);
        auto finish = std::chrono::high_resolution_clock::now();
        cout << "read: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        // apply filters
        start = std::chrono::high_resolution_clock::now();
        flip_horizontally();
        finish = std::chrono::high_resolution_clock::now();
        cout << "flip_horizontally: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        start = std::chrono::high_resolution_clock::now();
        flip_vertically();
        finish = std::chrono::high_resolution_clock::now();
        cout << "flip_vertically: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        start = std::chrono::high_resolution_clock::now();
        median_filter();
        finish = std::chrono::high_resolution_clock::now();
        cout << "median_filter: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        flip_color_filter();
        finish = std::chrono::high_resolution_clock::now();
        cout << "flip_color: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        plus_filter();
        finish = std::chrono::high_resolution_clock::now();
        cout << "plus_filter: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        // write output file
        start = std::chrono::high_resolution_clock::now();
        writeOutBmp24(fileBuffer, "output.bmp", bufferSize);
        finish = std::chrono::high_resolution_clock::now();
        cout << "write: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << endl;
        cout << "Total: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start1).count() << endl;
    }
    else
    {
        auto start = high_resolution_clock::now();
        // read input file
        getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);
        // apply filters
        flip_horizontally();
        flip_vertically();
        median_filter();
        flip_color_filter();
        plus_filter();
        // write output file
        writeOutBmp24(fileBuffer, "output.bmp", bufferSize);
        auto finish = std::chrono::high_resolution_clock::now();
        auto milliseconds = chrono::duration_cast<chrono::milliseconds>(finish - start);
        cout << "Speedup: " << milliseconds.count() << endl;
    }

    return 0;
}