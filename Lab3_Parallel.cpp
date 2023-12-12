//#include <iostream>
//#include <vector>
//#include <cstdlib>
//#include <ctime>
//#include <algorithm>
//#include <omp.h>
//#include <thread>
//
//void selectionSort(int* arr, int n) {
//    for (int i = 0; i < n - 1; ++i) {
//        int minIndex = i;
//        for (int j = i + 1; j < n; ++j) {
//            if (arr[j] < arr[minIndex]) {
//                minIndex = j;
//            }
//        }
//        if (minIndex != i) {
//            std::swap(arr[i], arr[minIndex]);
//        }
//    }
//}
//
//void selectionSortParallel(int* arr, int n, int numThreads) {
//#pragma omp parallel for shared(arr, n) num_threads(numThreads)
//    for (int i = 0; i < n - 1; ++i) {
//        int minIndex = i;
//
//        // Находим минимальный элемент в части массива
//        for (int j = i + 1; j < n; ++j) {
//            if (arr[j] < arr[minIndex]) {
//                minIndex = j;
//            }
//        }
//
//        // Переставляем элементы массива
//        if (minIndex != i) {
//#pragma omp critical
//            {
//                std::swap(arr[i], arr[minIndex]);
//            }
//        }
//    }
//}
//
//
//// Функция для проверки отсортированного массива
//bool isSorted(int* arr, int n) {
//    for (int i = 0; i < n - 1; ++i) {
//        if (arr[i] > arr[i + 1]) {
//            return false;
//        }
//    }
//    return true;
//}
//
//// Функция для заполнения динамического массива случайными числами
//void fillArrayWithRandomNumbers(int*& arr, int size, int M) {
//    std::srand(static_cast<unsigned>(std::time(nullptr)));
//    arr = new int[size];
//    for (int i = 0; i < size; i++) {
//        arr[i] = std::rand() % M;
//    }
//}
//
//int main() {
//    setlocale(LC_ALL, "Russian");
//
//    const int N = 100000; // Размер массива
//    int* arr = nullptr;
//
//    int M = 100; // Верхняя граница диапазона случайных чисел
//
//    fillArrayWithRandomNumbers(arr, N, M);
//    int* arrCopy = new int[N];
//    std::copy(arr, arr + N, arrCopy); // Создание копии исходного массива
//
//
//    // Измерение времени выполнения последовательной сортировки выбором
//    double startTime = omp_get_wtime();
//    selectionSort(arrCopy, N);
//    double endTime = omp_get_wtime();
//    double elapsedTime = endTime - startTime;
//
//    std::cout << "Время выполнения сортировки выбором: " << elapsedTime << " секунд" << std::endl;
//
//    // Проверка корректности сортировки после последовательной сортировки
//    if (isSorted(arrCopy, N)) {
//        std::cout << "Массив после последовательной сортировки упорядочен верно." << std::endl;
//    }
//    else {
//        std::cout << "Массив после последовательной сортировки НЕ упорядочен верно." << std::endl;
//    }
//
//    
//    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
//       
//        int* arrCopyPar = new int[N];
//        std::copy(arr, arr + N, arrCopyPar); // Создание копии исходного массива
//
//		startTime = omp_get_wtime();
//
//        selectionSortParallel(arrCopyPar, N, numThreads);
//		endTime = omp_get_wtime();
//		elapsedTime = endTime - startTime;
//		std::cout << "Время выполнения параллельной сортировки выбором на " << numThreads << " потоках: " << elapsedTime << " секунд" << std::endl;
//
//
//        // Проверка корректности сортировки после параллельной сортировки
//        if (isSorted(arrCopyPar, N)) {
//            std::cout << "Массив после параллельной сортировки на " << numThreads << " потоках упорядочен верно." << std::endl;
//        }
//        else {
//            std::cout << "Массив после параллельной сортировки на " << numThreads << " потоках НЕ упорядочен верно." << std::endl;
//        }
//        delete[] arrCopyPar;
//
//    }
//
//    // Освобождение памяти, выделенной для массива
//    delete[] arr;
//    delete[] arrCopy;
//
//    return 0;
//}

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include <thread>


void fillArrayWithRandomNumbers(int*& arr, int size, int M) {
    std::srand(std::time(0));
    arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = std::rand() % M;
    }
}

// Функция для подсчета цепочек из 10 нулей с использованием OpenMP
int countZeroChainsMultiThread(const int* arr, int size, int threads) {
    int chainCount = 0;

    omp_set_num_threads(threads);

#pragma omp parallel reduction(+:chainCount)
    {
        int localCount = 0;
#pragma omp for
        for (int i = 0; i < size; i++) {
            if (arr[i] == 0) {
                localCount++;
                if (localCount == 10) {
                    chainCount++;
                    localCount = 0;
                }
            }
            else {
                localCount = 0;
            }
        }
    }

    return chainCount;
}
// Функция для подсчета цепочек из 10 нулей в однопоточном режиме
int countZeroChainsSingleThread(const int* arr, int size) {
    int count = 0;
    int chainCount = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] == 0) {
            count++;
            if (count == 10) {
                chainCount++;
                count = 0;
            }
        }
        else {
            count = 0;
        }
    }
    return chainCount;
}


int main() {

        setlocale(LC_ALL, "Russian");
        const int size = 10000000000; // Размер массива
        int* arr = nullptr;

        int M = 2; // Верхняя граница диапазона случайных чисел

        fillArrayWithRandomNumbers(arr, size, M);


        int chains;

        
        // Однопоточный режим
        double startTime = omp_get_wtime();
        chains = countZeroChainsSingleThread(arr, size);
        double endTime = omp_get_wtime();
        std::cout << "Однопоточный режим: " << chains << " цепочек из 10 нулей." << std::endl;
        std::cout << "Время выполнения в однопотоковом режиме: " << endTime - startTime << " секунд." << std::endl;


    // Многопоточный режим с разным количеством потоков
    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
        std::copy(arr, arr + size, arr); // Создание копии исходного массива
        double startTime = omp_get_wtime();
        chains = countZeroChainsMultiThread(arr, size, numThreads);
        double endTime = omp_get_wtime();

        std::cout << "Многопотоковой режим: " << chains << " цепочек из 10 нулей." << std::endl;
        std::cout << "Время выполнения в многопотоковом режиме на " << numThreads << " потоках: " << endTime - startTime << " секунд." << std::endl;
    }
    delete[] arr;

    return 0;
}
