#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Matrix Dimensions
int mat1Rows, mat1Cols, mat2Rows, mat2Cols;

// Declaring Matrices
float mat1[2000][2000], mat2[2000][2000];
float singleThreadMatAns[2000][2000], multiThreadMatAns[2000][2000];

// Multiply matrix
void matMultiply() {
    for (int i = 0; i < mat1Rows; ++i)
        for (int j = 0; j < mat2Cols; ++j)
            for (int k = 0; k < mat1Cols; ++k)
                singleThreadMatAns[i][j] += mat1[i][k] * mat2[k][j];
}

void *multiThreadMultiply(void *arg) {
    // Get the thread ID assigned sequentially
    int tid = *(int *) (arg);
    int portion_size = mat2Rows / mat1Cols;
    int row_start = tid * portion_size;
    int row_end = (tid + 1) * portion_size;

    for (int i = row_start; i < row_end; ++i) {
        for (int j = 0; j < mat2Cols; ++j) {
            double sum = 0;
            for (int k = 0; k < mat1Cols; ++k)
                sum += mat1[i][k] * mat2[k][j];
            multiThreadMatAns[i][j] = sum;
        }
    }
}

void matMultiThread() {
    pthread_t *threads;
    int num_threads = mat1Rows;
    threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; ++i) {
        int *tid;
        tid = (int *) malloc(sizeof(int));
        *tid = i;
        pthread_create(&threads[i], NULL, multiThreadMultiply, (void *) tid);
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}

void showAns() {
    printf("\n\n---------Multiplied Matrix---------\n\n");
    for (int i = 0; i < mat1Rows; i++) {
        for (int j = 0; j < mat2Cols; j++)
            printf(multiThreadMatAns[i][j] - (int) multiThreadMatAns[i][j] > 0 ? "\t%f " : "\t%0.f\t ",
                   multiThreadMatAns[i][j]);
        printf("\n");
    }
}

// Calculate time taken by the program and print
void calcTime() {
    double timeSingleThread, timeMultiThread;

    // Starting time
    clock_t startTime;
    // Ending time
    clock_t endTime;

    startTime = clock();
    matMultiply();
    endTime = clock();

    // Execution time in milliseconds for single thread
    timeSingleThread = (double) (endTime - startTime) * 1000 / CLOCKS_PER_SEC;

    startTime = clock();
    matMultiThread();
    endTime = clock();

    // Execution time in milliseconds for multi thread
    timeMultiThread = (double) (endTime - startTime) * 1000 / CLOCKS_PER_SEC;

    showAns();
    printf("\n No. of Elements in Matrix\t:  %d\n ", mat1Rows * mat2Cols);
    printf("Time taken for Single Thread\t:  %.3lf %s\n ", timeSingleThread / 1000.0, "s");
    printf("Time taken for Multi Thread\t:  %.3lf %s\n ", timeMultiThread / 1000.0, "s");

    // Print values to file
    FILE *outFile;

    outFile = fopen("output.txt", "a+");
    fprintf(outFile, "\t%d", mat1Rows * mat2Cols);
    fprintf(outFile, "\t\t");
    fprintf(outFile, "%f\t\t%f", timeSingleThread / 1000, timeMultiThread / 1000);
    fprintf(outFile, "\n");
}

// Print Matrices
void showMatrix() {
    printf("\n%s\n", "-------- 1st Matrix ------- \n");
    for (int i = 0; i < mat1Rows; i++) {
        for (int j = 0; j < mat1Cols; j++)
            printf(mat1[i][j] - (int) mat1[i][j] > 0 ? "\t%f " : "\t%0.f\t ", mat1[i][j]);
        printf("\n");
    }

    printf("\n%s\n", "-------- 2nd Matrix ------- \n");
    for (int i = 0; i < mat2Rows; i++) {
        for (int j = 0; j < mat2Cols; j++)
            printf(mat2[i][j] - (int) mat2[i][j] > 0 ? "\t%f " : "\t%0.f\t ", mat2[i][j]);
        printf("\n");
    }
}

// Random Matrix Generator
void createMatrix() {
    srand(time(0));

    // Matrix 1
    for (int i = 0; i < mat1Rows; i++)
        for (int j = 0; j < mat1Cols; j++)
            mat1[i][j] = rand() % 2 == 1 ? (float) (rand() % 100) / RAND_MAX * 10000 : rand() % 100;

    // Matrix 2
    for (int i = 0; i < mat2Rows; i++)
        for (int j = 0; j < mat2Cols; j++)
            mat2[i][j] = rand() % 2 == 1 ? (float) (rand() % 100) / RAND_MAX * 10000 : rand() % 100;
}

// Read Matrix from file
void readFile() {
    FILE *file;
    file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("\n%s\n", "File not found");
        exit(1);
    }

    for (int i = 0; i < mat1Rows; i++)
        for (int j = 0; j < mat1Cols; j++)
            fscanf(file, "%f,", &mat1[i][j]);

    for (int i = 0; i < mat2Rows; i++)
        for (int j = 0; j < mat2Cols; j++)
            fscanf(file, "%f,", &mat2[i][j]);

    fclose(file);
}


int main() {
    // Getting values for Rows and Column
    printf("Enter Matrix 1 size (ex. 3 4) : ");
    scanf("%d", &mat1Rows);
    scanf("%d", &mat1Cols);

    printf("\nEnter Matrix 2 size (ex. 4 3) : ");
    scanf("%d", &mat2Rows);
    scanf("%d", &mat2Cols);

    if (mat1Cols == mat2Rows) {
        printf("\n[%d][%d] x [%d][%d] are Multipliable \n", mat1Rows, mat1Cols, mat2Rows, mat2Cols);
        printf("==================================================\n");
        printf("\n\t1 - Fill Matrix with Text File\n");
        printf("\t2 - Fill Matrix with random values\n\n");
        printf("\tSelect your option : ");
        int choice;
        scanf("%d", &choice);

        printf("\n==================================================\n");

        if (choice == 1) {
            printf("\n%s\n", "Text Read Matrices");
            readFile();
            showMatrix();
            calcTime();
        } else if (choice == 2) {
            createMatrix(); // Generate random matrix
            showMatrix();
            calcTime();

            // output.txt file with 100 matrices
            FILE *outFile;
            outFile = fopen("output.txt", "w");
            fprintf(outFile, "No. of Elements");
            fprintf(outFile, "\t\t");
            fprintf(outFile, "Single Thread");
            fprintf(outFile, "\t\t");
            fprintf(outFile, "Multi Thread");
            fprintf(outFile, "\n");

            for (int i = 0; i <= 100; i++) {
                mat1Rows = mat1Cols = mat2Rows = mat2Cols = i * 10;
                createMatrix(); // Generate random matrix
                calcTime();
            }
        } else printf("%s\n", " Invalid Input ");
    } else printf("\n[%d][%d]  x [%d][%d]  aren't Multipliable !!!\n", mat1Rows, mat1Cols, mat2Rows, mat2Cols);

    return 0;
}

