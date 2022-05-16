#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <vector>
#include <cmath>

using namespace std::chrono;
using namespace std;

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

float in[2500][2500][3] = {0}; // input picture
float out[2500][2500][3] = {0};

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

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
{
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
                    // fileReadBuffer[end - count] is the red value
                    in[i][j][k] = (unsigned char)fileReadBuffer[end - count];
                    break;
                case 1:
                    // fileReadBuffer[end - count] is the green value
                    in[i][j][k] = (unsigned char)fileReadBuffer[end - count];

                    break;
                case 2:
                    // fileReadBuffer[end - count] is the blue value
                    in[i][j][k] = (unsigned char)fileReadBuffer[end - count];
                    break;
                    // go to the next position in the buffer
                }
                count++;
            }
    }
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
                    // write red value in fileBuffer[bufferSize - count]
                    fileBuffer[bufferSize - count] = (unsigned char)out[i][j][k];
                    break;
                case 1:
                    // write green value in fileBuffer[bufferSize - count]
                    fileBuffer[bufferSize - count] = (unsigned char)out[i][j][k];
                    break;
                case 2:
                    // write blue value in fileBuffer[bufferSize - count]
                    fileBuffer[bufferSize - count] = (unsigned char)out[i][j][k];
                    break;
                    // go to the next position in the buffer
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
            out[i][j][0] = in[i][rows - j][0];
            out[i][j][1] = in[i][rows - j][1];
            out[i][j][2] = in[i][rows - j][2];
        }
}

void flip_vertically()
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            in[i][j][0] = out[cols - i][j][0];
            in[i][j][1] = out[cols - i][j][1];
            in[i][j][2] = out[cols - i][j][2];
        }
}

short median_neighbers(int i, int j, int k)
{
    if (i + 1 >= rows || j + 1 >= cols || i - 1 < 0 || j - 1 < 0)
        return in[i][j][k];

    int arr[9];
    int cnt = 0;
    for (int p = -1; p <= 1; p++)
    {
        for (int q = -1; q <= 1; q++)

        {
            arr[cnt] = (in[i - p][j - q][k]);
            cnt++;
        }
    }

    // odd - size vector
    return arr[4];
}

void median_filter()
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            out[i][j][0] = median_neighbers(i, j, 0);
            out[i][j][1] = median_neighbers(i, j, 1);
            out[i][j][2] = median_neighbers(i, j, 2);
        }
}
void flip_color()
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            out[i][j][0] = 255 - out[i][j][0];
            out[i][j][1] = 255 - out[i][j][1];
            out[i][j][2] = 255 - out[i][j][2];
        }
}
void plus_filter()
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k <= 2; k++)
            {
                if (j == cols / 2  || i == rows / 2)
                {
                    out[i][j][k] = 255.0;
                    // if (i != 0)
                    // {
                    //     out[i - 1][j][k] = 255.0;
                    // }
                    // if (i != rows - 1)
                    // {
                    //     out[i + 1][j][k] = 255.0;
                    // }
                }
            }
        }
}

int main(int argc, char *argv[])
{
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
    {
        cout << "File read error" << endl;
        return 1;
    }
    auto start = high_resolution_clock::now();
    // read input file
    getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);
    // apply filters
    flip_horizontally();
    flip_vertically();
    median_filter();
    flip_color();
    plus_filter();
    // write output file
    writeOutBmp24(fileBuffer, "output.bmp", bufferSize);
    auto finish = std::chrono::high_resolution_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(finish - start);
    cout << "Speedup: " << milliseconds.count() << endl;
    return 0;
}