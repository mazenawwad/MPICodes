#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include <mpi.h>



#define WIDTH 800

#define HEIGHT 800

#define MAX_ITER 1000



// Function to compute the Mandelbrot set for a given point

int mandelbrot(double real, double imag) {

    int iter;

    double z_real = 0.0;

    double z_imag = 0.0;



    for (iter = 0; iter < MAX_ITER; iter++) {

        double temp_real = z_real * z_real - z_imag * z_imag + real;

        double temp_imag = 2.0 * z_real * z_imag + imag;



        z_real = temp_real;

        z_imag = temp_imag;



        if (z_real * z_real + z_imag * z_imag > 4.0) {

            return iter;

        }

    }



    return MAX_ITER;

}



int main(int argc, char** argv) {

    int rank, size;

    double x, y;

    int row, col;

    int* mandelbrotSet;

    double startTime, endTime, commTime;



    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);



    // Calculate the number of rows each process will compute

    int rowsPerProcess = HEIGHT / size;

    int numRowsLastProcess = rowsPerProcess + (HEIGHT % size);



    // Allocate memory for the Mandelbrot set on each process

    if (rank == size - 1) {

        mandelbrotSet = (int*)malloc(sizeof(int) * numRowsLastProcess * WIDTH);

    } else {

        mandelbrotSet = (int*)malloc(sizeof(int) * rowsPerProcess * WIDTH);

    }



    // Start measuring elapsed time

    startTime = MPI_Wtime();



    // Compute Mandelbrot set for each process's assigned rows

    for (row = 0; row < (rank == size - 1 ? numRowsLastProcess : rowsPerProcess); row++) {

        for (col = 0; col < WIDTH; col++) {

            x = ((double)col - WIDTH / 2.0) * 4.0 / WIDTH;

            y = ((double)(rank * rowsPerProcess + row) - HEIGHT / 2.0) * 4.0 / HEIGHT;

            mandelbrotSet[row * WIDTH + col] = mandelbrot(x, y);

        }

    }



    // End measuring elapsed time

    endTime = MPI_Wtime();



    // Compute communication time

    commTime = endTime - startTime;



    // Reduce communication time across all processes

    double totalCommTime;

    MPI_Reduce(&commTime, &totalCommTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);



    if (rank == 0) {

        printf("Total elapsed time: %lf seconds\n", endTime - startTime);

        printf("Total communication time: %lf seconds\n", totalCommTime);

    }



    // Clean up and finalize MPI

    free(mandelbrotSet);

    MPI_Finalize();



    return 0;

}

