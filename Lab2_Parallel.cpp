#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <mutex>

std::mutex mtx; // ��������� ������� ��� ������������� ������� � ����� ��������
std::atomic<int> totalChainCountAtomic(0); // ��������� ��������� ���������� ��� �������������

// ������� ��� ���������� ������������� ������� ���������� �������
void fillArrayWithRandomNumbers(int*& arr, int size, int M) {
    std::srand(std::time(0));
    arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = std::rand() % M;
    }
}
// ������� ��� �������� ������� �� 10 ����� � ������������ ������
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

// ������� ��� �������� ������� �� 10 ����� � ������������� ������ (� ������)
int countZeroChainsMultiThreadWithMutexIncorrect(const int* arr, int size, int numThreads, int& totalChainCount) {
    int chunkSize = size / numThreads;

    std::vector<std::thread> threads(numThreads);

    for (int t = 0; t < numThreads; t++) {
        threads[t] = std::thread([&, t]() {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? size : (t + 1) * chunkSize;

            int count = 0;
            int chainCount = 0;
            int test = totalChainCount;
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

            totalChainCount = chainCount + test;
            });
    }

    for (int t = 0; t < numThreads; t++) {
        threads[t].join();
    }

    return totalChainCount;
}

// ������� ��� �������� ������� �� 10 ����� � ������������� ������ � ����������
int countZeroChainsMultiThreadWithMutex(const int* arr, int size, int numThreads, int& totalChainCount) {
    int chunkSize = size / numThreads;

    std::vector<std::thread> threads(numThreads);

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

            // ����������� ������� ����� ����������� ����� ����������
            mtx.lock();
            totalChainCount += chainCount;
            mtx.unlock();
            });
    }

    for (int t = 0; t < numThreads; t++) {
        threads[t].join();
    }

    return totalChainCount;
}



// ������� ��� �������� ������� �� 10 ����� � ������������� ������ � ���������� ����������
void countZeroChainsMultiThreadWithAtomic(const int* arr, int size, int numThreads) {
    int chunkSize = size / numThreads;

    std::vector<std::thread> threads(numThreads);

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

            // ���������� ��������� �������� ��� ���������� ����� ����������
            totalChainCountAtomic.fetch_add(chainCount, std::memory_order_relaxed);
            });
    }

    for (int t = 0; t < numThreads; t++) {
        threads[t].join();
    }
}

// ������� ��� �������� ������� �� 10 ����� � ������������� ������ � ��������������� �����������
int countZeroChainsMultiThreadWithIndividualVars(const int* arr, int size, int numThreads) {
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
    const int size = 100000; // ������ �������
    int* arr = nullptr;

    int M = 2; // ������� ������� ��������� ��������� �����

    fillArrayWithRandomNumbers(arr, size, M);

    double startTime, endTime;
    int chains;
    
    startTime = omp_get_wtime();
    chains = countZeroChainsSingleThread(arr, size);
    endTime = omp_get_wtime();
    std::cout << "������������ �����: " << chains << " ������� �� 10 �����." << std::endl;
    std::cout << "����� ���������� � ������������ ������: " << endTime - startTime << " ������." << std::endl;

    std::cout << "" << std::endl;
   // ������������� ����� � ���������� ������������
    int totalChainCount = 0;
    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
        startTime = omp_get_wtime();
        totalChainCount = 0; // �������� ����� ����������
        chains = countZeroChainsMultiThreadWithMutexIncorrect(arr, size, numThreads, totalChainCount);
        endTime = omp_get_wtime();
        std::cout << "�������������� ����� (�����): " << chains << " ������� �� 10 �����." << std::endl;
        std::cout << "����� ���������� � �������������� ������ �� " << numThreads << " ������� : " << endTime - startTime << " ������." << std::endl;
    }
    std::cout << "" << std::endl;

    // ������������� ����� � ����������
     totalChainCount = 0;
    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
        startTime = omp_get_wtime();
        totalChainCount = 0; // �������� ����� ����������
        chains = countZeroChainsMultiThreadWithMutex(arr, size, numThreads, totalChainCount);
        endTime = omp_get_wtime();
        std::cout << "�������������� ����� � ����������: " << chains << " ������� �� 10 �����." << std::endl;
        std::cout << "����� ���������� � �������������� ������ �� " << numThreads << " ������� : " << endTime - startTime << " ������." << std::endl;
    }
    std::cout << "" << std::endl;
    
    // ������������� ����� � ���������� ����������
    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
        startTime = omp_get_wtime();
        totalChainCountAtomic = 0; // �������� ��������� ����������
        countZeroChainsMultiThreadWithAtomic(arr, size, numThreads);
        endTime = omp_get_wtime();
        chains = totalChainCountAtomic.load(std::memory_order_relaxed); // �������� �������� ��������� ����������
        std::cout << "�������������� ����� � ���������� ����������: " << chains << " ������� �� 10 �����." << std::endl;
        std::cout << "����� ���������� � �������������� ������ �� " << numThreads << " �������: " << endTime - startTime << " ������." << std::endl;
    }
    std::cout <<""<< std::endl;

    // ������������� ����� � ��������������� �����������
    for (int numThreads = 2; numThreads <= std::thread::hardware_concurrency(); numThreads++) {
        startTime = omp_get_wtime();
        chains = countZeroChainsMultiThreadWithIndividualVars(arr, size, numThreads);
        endTime = omp_get_wtime();
        std::cout << "�������������� ����� � ��������������� �����������: " << chains << " ������� �� 10 �����." << std::endl;
        std::cout << "����� ���������� � �������������� ������ �� " << numThreads << " �������: " << endTime - startTime << " ������." << std::endl;
    }


    delete[] arr; // ����������� ����������� ���������� ������

    return 0;
}
