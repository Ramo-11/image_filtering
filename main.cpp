#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#define WIDTH 750 // second image width = 450
#define HEIGHT 500 // second image width = 339
#define DATA_SIZE WIDTH * HEIGHT
#define FILTER_SIZE_3_3 3
#define FILTER_SIZE_5_5 5

const char *fileName = "unesco750-1.raw"; // second image name = L.raw

// Sobel Operators
int sobelHorizontalFilter3x3[FILTER_SIZE_3_3][FILTER_SIZE_3_3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int sobelVerticalFilter3x3[FILTER_SIZE_3_3][FILTER_SIZE_3_3] = {
    {-1, -2, -1},
    {0,  0,  0},
    {1,  2,  1}
};

// 5x5 Sobel-like filters
int sobelHorizontalFilter5x5[FILTER_SIZE_5_5][FILTER_SIZE_5_5] = {
    {-1, -2, 0, 2, 1},
    {-2, -4, 0, 4, 2},
    {-4, -8, 0, 8, 4},
    {-2, -4, 0, 4, 2},
    {-1, -2, 0, 2, 1}
};

int sobelVerticalFilter5x5[FILTER_SIZE_5_5][FILTER_SIZE_5_5] = {
    {-1, -2, -4, -2, -1},
    {-2, -4, -8, -4, -2},
    {0, 0, 0, 0, 0},
    {2, 4, 8, 4, 2},
    {1, 2, 4, 2, 1}
};

int getMin(int, int);
int getMax(int, int);

void applySobel3x3(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH], int filter[FILTER_SIZE_3_3][FILTER_SIZE_3_3]);
void applySobel5x5(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH], int filter[FILTER_SIZE_5_5][FILTER_SIZE_5_5]);


int main() {
    FILE *inputFile;
    FILE *outputFileH_3_3, *outputFileV_3_3;
    FILE *outputFileH_5_5, *outputFileV_5_5;
    unsigned char inputData[DATA_SIZE];
    unsigned char outputDataH_3_3[DATA_SIZE], outputDataV_3_3[DATA_SIZE];
    unsigned char outputDataH_5_5[DATA_SIZE], outputDataV_5_5[DATA_SIZE];
    unsigned char image_input[HEIGHT][WIDTH];
    unsigned char image_outputH_3_3[HEIGHT][WIDTH], image_outputV_3_3[HEIGHT][WIDTH];
    unsigned char image_outputH_5_5[HEIGHT][WIDTH], image_outputV_5_5[HEIGHT][WIDTH];

    inputFile = fopen(fileName, "rb");
    fread(inputData, 1, DATA_SIZE, inputFile);
    fclose(inputFile);

    // Convert linear array to 2D array for processing
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            image_input[i][j] = inputData[i * WIDTH + j];
        }
    }

    applySobel5x5(image_input, image_outputH_5_5, sobelHorizontalFilter5x5);
    applySobel5x5(image_input, image_outputV_5_5, sobelVerticalFilter5x5);
    applySobel3x3(image_input, image_outputH_3_3, sobelHorizontalFilter3x3);
    applySobel3x3(image_input, image_outputV_3_3, sobelVerticalFilter3x3);

    // Convert 2D array back to linear array for horizontal output
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            outputDataH_3_3[i * WIDTH + j] = image_outputH_3_3[i][j];
            outputDataH_5_5[i * WIDTH + j] = image_outputH_5_5[i][j];
        }
    }

    // Convert 2D array back to linear array for vertical output
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            outputDataV_3_3[i * WIDTH + j] = image_outputV_3_3[i][j];
            outputDataV_5_5[i * WIDTH + j] = image_outputV_5_5[i][j];
        }
    }

    outputFileH_3_3 = fopen("output_vertical_3_3.raw", "wb");
    outputFileH_5_5 = fopen("output_vertical_5_5.raw", "wb");
    fwrite(outputDataH_3_3, 1, DATA_SIZE, outputFileH_3_3);
    fwrite(outputDataH_5_5, 1, DATA_SIZE, outputFileH_5_5);
    fclose(outputFileH_3_3);
    fclose(outputFileH_5_5);

    outputFileV_3_3 = fopen("output_horizontal_3_3.raw", "wb");
    outputFileV_5_5 = fopen("output_horizontal_5_5.raw", "wb");
    fwrite(outputDataV_3_3, 1, DATA_SIZE, outputFileV_3_3);
    fwrite(outputDataV_5_5, 1, DATA_SIZE, outputFileV_5_5);
    fclose(outputFileV_3_3);
    fclose(outputFileV_5_5);

    return 0;
}

int getMin(int a, int b) {
    return (a < b) ? a : b;
}

int getMax(int a, int b) {
    return (a > b) ? a : b;
}

void applySobel3x3(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH], int filter[FILTER_SIZE_3_3][FILTER_SIZE_3_3]) {
    int gradient;
    int offset = 1;
    for (int y = offset; y < HEIGHT - offset; y++) {
        for (int x = offset; x < WIDTH - offset; x++) {
            gradient = 0;
            for (int i = -offset; i <= offset; i++) {
                for (int j = -offset; j <= offset; j++) {
                    gradient += src[y + i][x + j] * filter[i + offset][j + offset];
                }
            }
            dst[y][x] = (unsigned char)(getMin(getMax((abs(gradient) / 8) + 128, 0), 255));
        }
    }
}

void applySobel5x5(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH], int filter[FILTER_SIZE_5_5][FILTER_SIZE_5_5]) {
    int gradient;
    int offset = FILTER_SIZE_5_5 / 2;
    for (int y = offset; y < HEIGHT - offset; y++) {
        for (int x = offset; x < WIDTH - offset; x++) {
            gradient = 0;
            for (int i = -offset; i <= offset; i++) {
                for (int j = -offset; j <= offset; j++) {
                    gradient += src[y + i][x + j] * filter[i + offset][j + offset];
                }
            }
            // Adjust to prevent overflow/underflow, using a constant divisor
            int result = gradient / 32;
            dst[y][x] = (unsigned char)getMin(getMax(result + 128, 0), 255);
        }
    }
}