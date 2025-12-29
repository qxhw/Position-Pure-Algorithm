/**
 * @file map_perm_algorithms.cpp
 * @brief Implementation of Linear-Time Permutation Ranking/Unranking Algorithms.
 * * This file includes:
 * 1. The classic Myrvold-Ruskey (MR) algorithm, as described in:
 * Myrvold, W., & Ruskey, F. (2001). Ranking and unranking permutations in linear time.
 * * 2. The novel Position Method and Position Pro (PP) algorithm, proposed by Yusheng Hu (2025).
 * The PP algorithm optimizes the mapping logic derived from the MR framework.
 * * Author: Yusheng Hu
 * Repository: https://github.com/Yusheng-Hu/Position-Pro-Algorithm
 */

#include <algorithm> // for std::swap
#include <chrono>    // for performance measurement
#include <cstdlib>   // for atoi
#include <cstring>   // for memcpy
#include <fstream>   // for file I/O
#include <iomanip>   // for setprecision
#include <iostream>  // for input/output
#include <random>    // for random number generation
#include <vector>    // for vector container

using namespace std;

// Myrvold-Ruskey unrank algorithm
// Converts factorial number representation (C) to permutation (D)
void MyrvoldRuskey_unrank(const vector<int> &C, vector<int> &D) {
  int n = C.size();
  for (int i = 0; i < n; ++i) {
    D[i] = i;
  }
  // Core unranking logic: Process from right to left
  for (int i = n - 1; i >= 1; --i) {
    const int k = C[i];
    const int temp = D[i];
    D[i] = D[k];
    D[k] = temp;
  }
  asm volatile("" : : : "memory");
}

// Myrvold-Ruskey rank algorithm
// Converts permutation (D) to factorial number representation (C)
void MyrvoldRuskey_rank(const vector<int> &D, vector<int> &C) {
  int n = D.size();
  vector<int> pi = D;
  vector<int> pi_inv(n);

  // 计算逆排列
  for (int i = 0; i < n; i++) {
    pi_inv[pi[i]] = i;
  }

  C.resize(n);
  int rank = 0;

  // 递归计算rank
  for (int k = n; k > 1; k--) {
    int s = pi[k - 1];
    C[k - 1] = s;

    // 交换操作
    swap(pi[k - 1], pi[pi_inv[k - 1]]);
    swap(pi_inv[s], pi_inv[k - 1]);
  }
  C[0] = 0;
}

// Position Mapping unrank algorithm
// Converts factorial index array C to permutation D
void Position_unrank(const vector<int> &C, vector<int> &D) {
  int n = C.size();
  static vector<int> M; // Static vector to avoid reallocation on each call
  if (M.size() != n) {
    M.resize(n); // Only resize if needed
  }
  // Initialize all elements to 0 without reallocating memory
  // memset(&M[0], 0, n * sizeof(int));

  // for (int i = 1; i < n; ++i) {
  //   D[i] = C[i];
  //   M[i] = M[C[i]];
  //   M[C[i]] = i;
  //   D[M[i]] = i;
  // }

  memcpy(&D[0], &C[0], n * sizeof(int));

  for (int i = 0; i < n; ++i) {
    const int C_i = C[i];
    const int M_C_i = M[C_i];
    // D[i] = a;
    M[i] = M_C_i;
    M[C_i] = i;
    D[M_C_i] = i;
    asm volatile("" : : : "memory");
  }
}

// Position Mapping rank algorithm
// Converts permutation D to factorial index array C
void Position_rank(const vector<int> &D, vector<int> &C) {
  int n = D.size();
  // C.resize(n);         // Ensure C has the correct size
  vector<int> M(n, 0); // Initialize vector with correct size
  C = D;

  // 正向遍历D，生成M数组
  for (int i = 0; i < n; ++i) {
    M[C[i]] = i;
  }
  // 反向遍历D，生成C数组
  for (int i = n - 1; i >= 0; --i) {
    // 增加调测
    C[M[i]] = C[i];
    M[C[i]] = M[i];
  }
}

// PositionPro unrank algorithm
void PositionPro_unrank(const vector<int> &C, vector<int> &D) {
  // int n = C.size();
  // D.resize(n);

  // for (int i = 0; i < C.size(); ++i) {
  //   D[i] = D[C[i]];
  //   D[C[i]] = i;
  //   asm volatile("" : : : "memory");
  // }

  for (int i = 0; i < C.size(); ++i) {
    const int a = C[i];
    D[i] = D[a];
    D[a] = i;
  }
  asm volatile("" : : : "memory");
}

// PositionPro rank algorithm
void PositionPro_rank(const vector<int> &D, vector<int> &C) {
  int n = D.size();
  C.resize(n); // Ensure C has the correct size

  // Create a copy of D for manipulation
  vector<int> D_local = D;
  vector<int> M(n, 0); // Initialize position map

  for (int i = 0; i < n; ++i) {
    M[D_local[i]] = i;
  }

  for (int i = n - 1; i >= 0; --i) {
    C[i] = M[i];
    D_local[M[i]] = D_local[i];
    M[D_local[i]] = M[i];
  }
}

/**
 * @brief LOOKUP 1: Get Value at Specific Position (Backward Tracing).
 * Answers: "What is the number stored at index k?"
 * Complexity: O(N-k) average, O(N) worst case. Read-only.
 */
int PP_get_value_at_position(const std::vector<int> &C, int k) {
  int current_target_pos = k;
  int n = C.size();

  // Backward scanning: The last person to write to a position determines its
  // value.
  for (int i = n - 1; i >= 0; --i) {
    if (i > current_target_pos) {
      // Check if loop index 'i' was injected into our target position
      if (C[i] == current_target_pos) {
        return i;
      }
    } else if (i == current_target_pos) {
      // Inheritance: The value was moved from C[i] to i at this step
      current_target_pos = C[i];
    }
  }
  return current_target_pos;
}

/**
 * @brief LOOKUP 2: Find Position of Specific Value (Forward Tracking).
 * Answers: "Where is the number X located?"
 * Complexity: O(N-X). Read-only.
 */
int PP_find_position_of_value(const std::vector<int> &C, int X) {
  int n = C.size();
  if (X < 0 || X >= n)
    return -1;

  // Forward tracking: Once injected at i=X, follow the "flow" to higher
  // indices.
  int current_pos = C[X];

  for (int i = X + 1; i < n; ++i) {
    if (C[i] == current_pos) {
      current_pos = i;
    }
  }
  return current_pos;
}
