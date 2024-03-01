#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 750
#define HEIGHT 500
#define DATA_SIZE WIDTH*HEIGHT
#define FILTER_SIZE_3_3 3
#define FILTER_SIZE_5_5 5

// Sobel Operators
// Adjusted 3x3 Sobel-like filters with weights
int sobelHorizontalFilter3X3[FILTER_SIZE_3_3][FILTER_SIZE_3_3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int sobelVerticalFilter3X3[FILTER_SIZE_3_3][FILTER_SIZE_3_3] = {
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

void applySobelHorizontal(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH]);
void applySobelVertical(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH]);

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
    int i, j;

    inputFile = fopen("unesco750-1.raw", "rb");
    fread(inputData, 1, DATA_SIZE, inputFile);
    fclose(inputFile);

    // Convert linear array to 2D array for processing
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            image_input[i][j] = inputData[i * WIDTH + j];
        }
    }

    applySobel5x5(image_input, image_outputH_5_5, sobelHorizontalFilter5x5);
    applySobel5x5(image_input, image_outputV_5_5, sobelVerticalFilter5x5);
    applySobelHorizontal(image_input, image_outputH_3_3);
    applySobelVertical(image_input, image_outputV_3_3);

    // Convert 2D array back to linear array for horizontal output
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            outputDataH_3_3[i * WIDTH + j] = image_outputH_3_3[i][j];
            outputDataH_5_5[i * WIDTH + j] = image_outputH_5_5[i][j];
        }
    }

    // Convert 2D array back to linear array for vertical output
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
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

void applySobelHorizontal(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH]) {
    int x, y, i, j, sobelHorizontal;
    for (y = 1; y < HEIGHT - 1; y++) {
        for (x = 1; x < WIDTH - 1; x++) {
            sobelHorizontal = 0;
            for (i = -1; i <= 1; i++) {
                for (j = -1; j <= 1; j++) {
                    sobelHorizontal += src[y + i][x + j] * sobelHorizontalFilter3X3[i + 1][j + 1];
                }
            }
            // Adjust the scaling factor here to soften the filter effect
            dst[y][x] = (unsigned char)(getMin(getMax((abs(sobelHorizontal) / 8) + 128, 0), 255));
        }
    }
}

void applySobelVertical(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH]) {
    int x, y, i, j, sobelVertical;
    for (y = 1; y < HEIGHT - 1; y++) {
        for (x = 1; x < WIDTH - 1; x++) {
            sobelVertical = 0;
            for (i = -1; i <= 1; i++) {
                for (j = -1; j <= 1; j++) {
                    sobelVertical += src[y + i][x + j] * sobelVerticalFilter3X3[i + 1][j + 1];
                }
            }
            // Adjust the scaling factor here to soften the filter effect
            dst[y][x] = (unsigned char)(getMin(getMax((abs(sobelVertical) / 8) + 128, 0), 255));
        }
    }
}

void applySobel5x5(unsigned char src[HEIGHT][WIDTH], unsigned char dst[HEIGHT][WIDTH], int filter[FILTER_SIZE_5_5][FILTER_SIZE_5_5]) {
    int x, y, i, j, gradient;
    int offset = FILTER_SIZE_5_5 / 2;
    for (y = offset; y < HEIGHT - offset; y++) {
        for (x = offset; x < WIDTH - offset; x++) {
            gradient = 0;
            for (i = -offset; i <= offset; i++) {
                for (j = -offset; j <= offset; j++) {
                    gradient += src[y + i][x + j] * filter[i + offset][j + offset];
                }
            }
            // Adjust to prevent overflow/underflow, using a constant divisor
            int result = gradient / 32;
            dst[y][x] = (unsigned char)getMin(getMax(result + 128, 0), 255);
        }
    }
}