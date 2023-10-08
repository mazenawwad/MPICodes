#include <stdio.h>

#include <stdlib.h>

#include <mpi.h>

#include <math.h>

#include <sys/time.h>



#define WIDTH 801

#define HEIGHT 800

#define MAX_ITER 1000



int mandelbrot(double x, double y) {

    double real = x;

    double imag = y;

    int n;

    for (n = 0; n < MAX_ITER; n++) {

        double real2 = real * real;

        double imag2 = imag * imag;

        if (real2 + imag2 > 4.0) {

            return n;

        }

        imag = 2 * real * imag + y;

        real = real2 - imag2 + x;

    }

    return MAX_ITER;

}



int main(int argc, char *argv[]) {

    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);



    if (WIDTH % size != 0) {

        if (rank == 0) {

            printf("Error: WIDTH must be divisible by the number of processes.\n");

        }

        MPI_Finalize();

        return 1;

    }



    int local_width = WIDTH / size;

    int local_height = HEIGHT;

    int local_start = rank * local_width;

    int local_end = local_start + local_width;



    double x_min = -2.0;

    double x_max = 1.0;

    double y_min = -1.5;

    double y_max = 1.5;

    double x_step = (x_max - x_min) / WIDTH;

    double y_step = (y_max - y_min) / HEIGHT;



    struct timeval start_time, end_time;

    double computation_time, communication_time;



    int* local_data = (int*)malloc(local_width * local_height * sizeof(int));



    gettimeofday(&start_time, NULL);



    for (int i = local_start; i < local_end; i++) {

        for (int j = 0; j < HEIGHT; j++) {

            double x = x_min + i * x_step;

            double y = y_min + j * y_step;

            int value = mandelbrot(x, y);

            local_data[(i - local_start) * local_height + j] = value;

        }

    }



    gettimeofday(&end_time, NULL);



    computation_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1e6;



    int* global_data = NULL;



    if (rank == 0) {

        global_data = (int*)malloc(WIDTH * HEIGHT * sizeof(int));

    }



    // Measure communication time

    double start_comm_time, end_comm_time;

    start_comm_time = MPI_Wtime();



    MPI_Gather(local_data, local_width * local_height, MPI_INT, global_data, local_width * local_height, MPI_INT, 0, MPI_COMM_WORLD);



    end_comm_time = MPI_Wtime();



    communication_time = end_comm_time - start_comm_time;



    if (rank == 0) {

        // Calculate and print elapsed time and communication time

        printf("Elapsed Time: %.4f seconds\n", computation_time + communication_time);

        printf("Computation Time: %.4f seconds\n", computation_time);

        printf("Communication Time: %.4f seconds\n", communication_time);



        // You can further process or save the global_data here

        // (e.g., save it as an image).

        free(global_data);

    }



    free(local_data);



    MPI_Finalize();

    return 0;

}

