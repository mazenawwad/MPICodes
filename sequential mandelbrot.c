#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <math.h>

#include <time.h>



#define WIDTH 1920

#define HEIGHT 1080

#define MAX_ITER 1000

#define FILENAME "mandelbrot_sequential.bmp"



typedef struct {

    uint8_t r, g, b;

} RGBPixel;



void save_image(const char* filename, RGBPixel* image, int width, int height) {

    FILE* file = fopen(filename, "wb");

    if (!file) {

        perror("Error opening file");

        exit(1);

    }



    uint32_t header[13] = {

        0x4D42,               // BM signature

        54 + width * height * 3,  // File size

        0, 0,                 // Reserved

        54,                   // Offset to pixel data

        40,                   // DIB header size

        width, height,         // Width and height

        1, 24                // Planes and bits per pixel

    };



    fwrite(header, sizeof(uint32_t), 13, file);

    fwrite(image, sizeof(RGBPixel), width * height, file);



    fclose(file);

}



int mandelbrot(int x, int y) {

    double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;

    double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;

    double cr = real;

    double ci = imag;



    int iter = 0;

    double zr = 0, zi = 0;

    double zr2 = 0, zi2 = 0;



    while (iter < MAX_ITER && zr2 + zi2 < 4.0) {

        zi = 2.0 * zr * zi + ci;

        zr = zr2 - zi2 + cr;

        zr2 = zr * zr;

        zi2 = zi * zi;

        iter++;

    }



    return iter;

}



int main() {

    RGBPixel* image = (RGBPixel*)malloc(WIDTH * HEIGHT * sizeof(RGBPixel));

    if (!image) {

        perror("Error allocating memory for the image");

        return 1;

    }



    clock_t start_time = clock();



    for (int y = 0; y < HEIGHT; y++) {

        for (int x = 0; x < WIDTH; x++) {

            int iter = mandelbrot(x, y);

            int index = x + y * WIDTH;

            image[index].r = (iter % 8) * 32;

            image[index].g = (iter % 16) * 16;

            image[index].b = (iter % 32) * 8;

        }

    }



    clock_t end_time = clock();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    

    // Output the execution time

    printf("Mandelbrot computation took %.2f seconds.\n", elapsed_time);



    // Save the image

    save_image(FILENAME, image, WIDTH, HEIGHT);

    free(image);



    return 0;

}

