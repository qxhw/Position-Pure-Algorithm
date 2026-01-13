/**
 * @file permpure_full.cpp
 * @brief High-performance Permutation Generation Algorithm (Iterative Implementation)
 * * @copyright Copyright (c) 2024 [ Yusheng-Hu ]. All rights reserved.
 * @license This project is licensed under the MIT License - see the LICENSE file for details.
 * * Program Details:
 * - Implements an efficient iterative permutation generation logic.
 * - Features: Cross-platform CPU Affinity binding, High-precision timing (std::chrono), 
 * and optimization-resistant checksums.
 * - Performance: Designed for minimal branch misprediction and low overhead.
 * * Environment:
 * - Platform: Windows / Linux (Auto-switching headers)
 * - Compiler: GCC/MinGW (supports __builtin_LINE) or MSVC
 */

#include <stdio.h>
#include <time.h>
#include <chrono>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sched.h>
    #include <pthread.h>
#endif

#ifndef PERM_SIZE
#define PERM_SIZE 12
#endif
#define LittleNumber 5

// #define DEBUG

#ifdef DEBUG
#define COUNT_PURECESS() ProcessCount[__builtin_LINE()]++
#else
#define COUNT_PURECESS() 
#endif

int main() {
    // --- Set CPU Affinity for Accurate Benchmarking ---
#ifdef _WIN32
    DWORD_PTR mask = 8; // CPU mask for core 3
    SetThreadAffinityMask(GetCurrentThread(), mask);
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset); // Bind to Core 1 on Linux
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
#endif

    unsigned long long checksum = 0;
    unsigned long long ProcessCount[200] = {0};
    int volatile i = 0;
    int j = 0;
    int C[PERM_SIZE] = {0}, D[PERM_SIZE] = {0}, M[PERM_SIZE] = {0};

    // --- High Precision Timing ---
    auto start = std::chrono::high_resolution_clock::now();

    // Main Algorithm Loop
    while (C[0] < 1) {
        COUNT_PURECESS();
        for (; i < PERM_SIZE-1; ++i) {
            COUNT_PURECESS();
            D[i] = D[C[i]];
            D[C[i]] = i;
        }

        for (int ii = 0; ii < PERM_SIZE; ii++) {
            COUNT_PURECESS();
            D[PERM_SIZE-1] = D[ii];
            D[ii] = PERM_SIZE-1;
            checksum += D[PERM_SIZE-1];

#if PERM_SIZE <= LittleNumber
            printf("\n");
            for (int jj = 0; jj < PERM_SIZE; jj++) {
                printf("%d,", D[jj]);
            }
#endif
            D[ii] = D[PERM_SIZE-1];
        }

        D[C[PERM_SIZE - 2]] = D[PERM_SIZE - 2];

        C[PERM_SIZE - 2]++;
        for (i = PERM_SIZE - 2; (i > 0) && (C[i] > i); i--) {
            COUNT_PURECESS();
            C[i] = 0;
            C[i - 1]++;
            D[C[i - 1] - 1] = D[i - 1];
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = finish - start;
    
    // Standardized output for easy parsing
    printf("\nREPORT_START");
    printf("\nALGORITHM: permpure_full");
    printf("\nEXECUTION_TIME: %lf", duration.count());
    printf("\nCHECKSUM: %llu", checksum);
    printf("\nREPORT_END\n");

#ifdef DEBUG
    // Debug info logic preserved
    unsigned long long perm = 1;
    unsigned long long PermValue[PERM_SIZE + 1] = {0};
    unsigned long long SumPermValue[PERM_SIZE + 1] = {0};
    PermValue[0] = 1;
    SumPermValue[0] = 0;
    for (int k = 1; k <= PERM_SIZE; k++) {
        perm = perm * k;
        PermValue[k] = perm;
        SumPermValue[k] = SumPermValue[k - 1] + perm;
    }
    for (int k = 1; k <= PERM_SIZE; k++) {
        printf("\n%u !, %llu, sigma %llu", k, PermValue[k], SumPermValue[k]);
    }
#endif

    return 0;
}

