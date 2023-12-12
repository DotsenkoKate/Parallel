//#include <iostream>
//#include <vector>
//#include <thread>
//#include <cstdlib>
//#include <ctime>
//#include <omp.h>
//
//// Функция для заполнения вектора случайными числами
//void fillVectorWithRandomNumbers(std::vector<int>& vec, int size, int M) {
//    std::srand(std::time(0));
//    vec.reserve(size);
//    for (int i = 0; i < size; i++) {
//        vec.push_back(std::rand() % M);
//    }
//}
//
//// Функция для подсчета цепочек из 10 нулей в однопоточном режиме
//int countZeroChainsSingleThread(const std::vector<int>& vec) {
//    int count = 0;
//    int chainCount = 0;
//    for (int val : vec) {
//        if (val == 0) {
//            count++;
//            if (count == 10) {
//                chainCount++;
//                count = 0;
//            }
//        }
//        else {
//            count = 0;
//        }
//    }
//    return chainCount;
//}
//
//// Функция для подсчета цепочек из 10 нулей в многопоточном режиме
//int countZeroChainsMultiThread(const std::vector<int>& vec, int numThreads) {
//    int chunkSize = vec.size() / numThreads;
//
//    std::vector<std::thread> threads(numThreads);
//    std::vector<int> chainCounts(numThreads, 0);
//
//    for (int t = 0; t < numThreads; t++) {
//        threads[t] = std::thread([&, t]() {
//            int start = t * chunkSize;
//            int end = (t == numThreads - 1) ? vec.size() : (t + 1) * chunkSize;
//
//            int count = 0;
//            int chainCount = 0;
//
//            for (int i = start; i < end; i++) {
//                if (vec[i] == 0) {
//                    count++;
//                    if (count == 10) {
//                        chainCount++;
//                        count = 0;
//                    }
//                }
//                else {
//                    count = 0;
//                }
//            }
//
//            chainCounts[t] = chainCount;
//            });
//    }
//
//    for (int t = 0; t < numThreads; t++) {
//        threads[t].join();
//    }
//
//    int totalChainCount = 0;
//    for (int t = 0; t < numThreads; t++) {
//        totalChainCount += chainCounts[t];
//    }
//
//    return totalChainCount;
//}
//
//int main() {
//    setlocale(LC_ALL, "Russian");
//    const int size = 100000000000; // Размер вектора
//    std::vector<int> vec;
//
//    int M = 2; // Верхняя граница диапазона случайных чисел
//
//    fillVectorWithRandomNumbers(vec, size, M);
//
//    double startTime, endTime;
//    int chains;
//
//    // Однопоточный режим
//    startTime = omp_get_wtime();
//    chains = countZeroChainsSingleThread(vec);
//    endTime = omp_get_wtime();
//    std::cout << "Однопоточный режим: " << chains << " цепочек из 10 нулей." << std::endl;
//    std::cout << "Время выполнения в однопотоковом режиме: " << endTime - startTime << " секунд." << std::endl;
//
//    //Многопоточный режим с разным количеством потоков
//    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
//        startTime = omp_get_wtime();
//        chains = countZeroChainsMultiThread(vec, numThreads);
//        endTime = omp_get_wtime();
//        std::cout << "Многопотоковой режим: " << chains << " цепочек из 10 нулей." << std::endl;
//        std::cout << "Время выполнения в многопотоковом режиме на " << numThreads << " потоках : " << endTime - startTime << " секунд." << std::endl;
//    }
//
//    return 0;
//}


#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <omp.h>

// Функция для заполнения динамического массива случайными числами
void fillArrayWithRandomNumbers(int*& arr, int size, int M) {
    std::srand(std::time(0));
    arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = std::rand() % M;
    }
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

// Функция для подсчета цепочек из 10 нулей в многопоточном режиме
int countZeroChainsMultiThread(const int* arr, int size, int numThreads) {
    int chunkSize = size / numThreads;

    std::vector<std::thread> threads(numThreads);
    std::vector<int> chainCounts(numThreads, 0);

    for (int t = 0; t < numThreads; t++) {
        threads[t] = std::thread([&, t]() {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? size : (t + 1) * chunkSize;

            int count = 0;
            int chainCount = 0;

            for (int i = start; i < end; i++) {
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

            chainCounts[t] = chainCount;
            });
    }

    for (int t = 0; t < numThreads; t++) {
        threads[t].join();
    }

    int totalChainCount = 0;
    for (int t = 0; t < numThreads; t++) {
        totalChainCount += chainCounts[t];
    }

    return totalChainCount;
}

int main() {
    setlocale(LC_ALL, "Russian");
    const int size = 1000000; // Размер массива
    int* arr = nullptr;

    int M = 2; // Верхняя граница диапазона случайных чисел

    fillArrayWithRandomNumbers(arr, size, M);

    double startTime, endTime;
    int chains;
    int* arrCopy = new int[size];
    std::copy(arr, arr + size, arrCopy); // Создание копии исходного массива

    // Однопоточный режим
    startTime = omp_get_wtime();
    chains = countZeroChainsSingleThread(arrCopy, size);
    endTime = omp_get_wtime();
    std::cout << "Однопоточный режим: " << chains << " цепочек из 10 нулей." << std::endl;
    std::cout << "Время выполнения в однопотоковом режиме: " << endTime - startTime << " секунд." << std::endl;
    delete[] arrCopy; // Освобождаем динамически выделенную память


    // Многопоточный режим с разным количеством потоков
    for (int numThreads = 2; numThreads <=  std::thread::hardware_concurrency(); numThreads++) {
        int* arrCopyPar = new int[size];
        std::copy(arr, arr + size, arrCopyPar); // Создание копии исходного массива
        startTime = omp_get_wtime();
        chains = countZeroChainsMultiThread(arrCopyPar, size, numThreads);
        endTime = omp_get_wtime();
        std::cout << "Многопотоковой режим: " << chains << " цепочек из 10 нулей." << std::endl;
        std::cout << "Время выполнения в многопотоковом режиме на " << numThreads << " потоках : " << endTime - startTime << " секунд." << std::endl;
        delete[] arrCopyPar;// Освобождаем динамически выделенную память

    }
    delete[] arr; // Освобождаем динамически выделенную память

    return 0;
}
