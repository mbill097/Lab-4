#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>
#include <pthread.h>


static const long Num_To_Sort = 100000000;

// Sequential version of your sort.
// If you're implementing the PSRS algorithm, you may ignore this section...
void sort_s(int *arr, int left, int right)
{

    int i = left, j = right,a=0;
    int tmp;
    int pivot = arr[(left + right) / 2];

    while (i <= j)
    {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j)
        {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    if (left < j)
    {
        sort_s(arr, left, j);
    }
    if (i < right)
    {
        sort_s(arr, i, right);
    }
}

// Parallel version of your sort
void quickSort_parallel(int *array, int left, int right, int cutoff)
{
    int i = left, j = right;
    int tmp;
    int pivot = array[(left + right) / 2];
    {
        while (i <= j)
        {
            while (array[i] < pivot)
                i++;
            while (array[j] > pivot)
                j--;
            if (i <= j)
            {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
                i++;
                j--;
            }
        }
    }

    if (((right - left) < cutoff))
    {
        if (left < j)
        {
            quickSort_parallel(array, left, j, cutoff);
        }
        if (i < right)
        {
            quickSort_parallel(array, i, right, cutoff);
        }
    }
    else
    {
#pragma omp task
        {
            quickSort_parallel(array, left, j, cutoff);
        }
#pragma omp task
        {
            quickSort_parallel(array, i, right, cutoff);
        }
    }
}
void sort_p(int *arr, int lenArray, int numThreads)
{
    int cutoff = 1000;

#pragma omp parallel num_threads(numThreads)
    {
#pragma omp single nowait
        {
            quickSort_parallel(arr, 0, lenArray - 1, cutoff);
        }
    }
}
int main()
{
    int *arr_s = malloc(sizeof(int) * Num_To_Sort);
    long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int)time(NULL) + (unsigned int)p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++)
        {
            arr_s[i] = rand_r(&seed);
        }
    }

    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.
    int *arr_p = malloc(sizeof(int) * Num_To_Sort);
    memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    sort_s(arr_s, 0, Num_To_Sort - 1);
    gettimeofday(&end, NULL);
    printf("\nTook %f seconds\n\n", end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / 1000000);

    free(arr_s);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    sort_p(arr_p, Num_To_Sort, omp_get_max_threads());
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / 1000000);

    free(arr_p);

    return 0;
}
//College, ©. D., 2009. dartmouth. [Online]
//Available at: https://www.dartmouth.edu/~rc/classes/intro_openmp/Approaches_to_Parallelism_Using_OpenMP.html
//[Accessed 11 12 2019].
//geeksforgeeks, 2017. Quicksort. [Online]
//Available at: https://www.geeksforgeeks.org/quick-sort/
//[Accessed 12 2019].
//IBM, 2018. Pragma Directives for Parallel processing. [Online]
//Available at: https://www.ibm.com/support/knowledgecenter/SSGH2K_13.1.2/com.ibm.xlc131.aix.doc/compiler_ref/tuoptppp.html
//[Accessed 12 2019].
//Wiki, 2016. Quicksort. [Online]
//Available at: https://en.wikipedia.org/wiki/Quicksort
//[Accessed 12 2019].