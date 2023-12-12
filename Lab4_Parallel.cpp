#include <iostream>  
#include <omp.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <thread>


using namespace std;

bool Equal(int* sequential, int* parallel, int length);
void partition(int* v, int& i, int& j, int array_size);
void quicksort(int* v, int array_size);
void quicksort_parallel(int* v, int array_size);


int main()
{
    setlocale(LC_ALL, "rus");

    int N = 1000000000; //количество символов во входном массиве

    int* original = new int[N];
    int* result = new int[N];
    srand(time(0));
    //генерация данных во входном массиве
    for (int i = 0; i < N; i++)
    {
        original[i] = rand() % 1000 + 1;

    }
    for (int i = 0; i < N; i++)
    {
        result[i] = original[i];
    }
    double start_time, end_time;

   // Последовательная сортировка
    start_time = omp_get_wtime();
    quicksort(result, N);
    end_time = omp_get_wtime();
    double time = end_time - start_time;
    cout << "Время последовательной сортировки: " << time << endl;
    
    // Параллельная сортировка
    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
        int* par_result = new int[N];
        for (int i = 0; i < N; i++)
        {
            par_result[i] = original[i];
        }
        start_time = omp_get_wtime();
        omp_set_dynamic(0);
        omp_set_num_threads(numThreads);
#pragma omp parallel
        {
#pragma omp single    
            quicksort_parallel(par_result, N);
        }
        end_time = omp_get_wtime();
        time = end_time - start_time;
        cout << "Время параллельной сортировки на "<< numThreads << " потоках: " << time << endl;
        // Сравнение результатов
        if (Equal(result, par_result, N))
            cout << "Массивы совпадают." << endl;
        else
            cout << "Массивы не совпадают" << endl;
        delete[] par_result;
    }

    delete[] result;
    delete[] original;

}


bool Equal(int* sequential, int* parallel, int length)
{
    for (int i = 0; i < length; i++)
        if (sequential[i] != parallel[i])
            return false;
    return true;
}
void partition(int* v, int& i, int& j, int array_size) {
    i = 0;
    j = array_size - 1;
    int pivot = v[array_size / 2];
    do {
        while (v[i] < pivot) i++;
        while (v[j] > pivot) j--;
        if (i <= j) {
            std::swap(v[i], v[j]);
            i++;
            j--;
        }
    } while (i <= j);
}
void quicksort(int* v, int array_size) {

    int i, j;
    partition(v, i, j, array_size);
    if (0 < j)
        quicksort(v, j + 1);
    if (i < array_size)
        quicksort(v + i, array_size - i);

}
void quicksort_parallel(int* v, int array_size) {

    int i, j;
    partition(v, i, j, array_size);

    {
        if (array_size / 2 < 50000) {
#pragma omp task shared(v)
            if (j > 0)
                quicksort(v, j + 1);
#pragma omp task shared(v)
            if (array_size > i)
                quicksort(v + i, array_size - i);
        }
        else {
#pragma omp task shared(v)
            if (j > 0)
                quicksort_parallel(v, j + 1);
#pragma omp task shared(v)
            if (array_size > i)
                quicksort_parallel(v + i, array_size - i);
        }
#pragma omp taskwait
    }
}
