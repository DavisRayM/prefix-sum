/// Parallel Sum - Work-efficient algorithm implementation.
/// Based on [Prefix Sum Work Efficient
/// Algorithm](https://en.wikipedia.org/wiki/Prefix_sum#Algorithm_2:_Work-efficient)
/// & explanations from Seattle University CPSC 5600 F24
///
/// Note: This code could *probably* be shortened quite a bit if one decides to
/// use Vectors & std::inclusive_scan(for the sequential portions?)
/// code...
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#ifdef TIMING
#include <chrono>
using std::chrono::duration_cast;
using std::chrono::steady_clock;
using TimePoint = std::chrono::steady_clock::time_point;
using TimeSpan = std::chrono::duration<double>;
#endif

/// Generates a array containing random numbers in the range 1 to 100
int *random_array(int count);

/// Prints array to stdout without newline
void print_array(int *arr, int size);

/// Generates intermediate Sequence `z` from the source array. z0 = x0 + x1,
/// z1 = x2 + x3, ...
int *calculate_sequence_z(int *x, int sourceArrSize);

/// Generates sequence w by calculating the prefix sum of intermediate sequence
/// z
int *calculate_sequence_w(int *z, int sourceArrSize);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " [ARRAY_SIZE] [THREAD_COUNT]\n";
    std::exit(1);
  }

  int arraySize = atoi(argv[1]);
  int threadCount = atoi(argv[2]);

  omp_set_num_threads(threadCount);

  // Generate random data
  int *x = random_array(arraySize);
  std::cout << "Random array(x): ";
  print_array(x, arraySize);
  std::cout << "\n\n";

  int *z = calculate_sequence_z(x, arraySize);
  int *w = calculate_sequence_w(z, arraySize);

  // Calculate the prefix sum array using the intermediate sequences z & w
  int *y = new int[arraySize];
  y[0] = x[0];
  y[1] = z[0];

  delete[] z;

  std::cout << "Calculating sequence y in parallel\n";

#ifdef TIMING
  // Start Time 2
  TimePoint st2 = steady_clock::now();
#endif

#pragma omp parallel for
  for (int i = 2; i < arraySize; i++) {
    // If even calculate value as sum of previous value and position i in x
    if (i % 2 == 0) {
      y[i] = w[i / 2 - 1] + x[i];
    } else {
      y[i] = w[i / 2];
    }
  }

#ifdef TIMING
  // End Time 2
  TimePoint et2 = steady_clock::now();
  // Time Taken 0
  TimeSpan tt2 = duration_cast<TimeSpan>(et2 - st2);
#endif

  std::cout << "Prefix Sum Array (y): ";
  print_array(y, arraySize);
  std::cout << "\n\n";

#ifdef TIMING
  std::cout << "Time taken to calculate: " << tt2.count() << " seconds\n\n";
#endif

  delete[] x;
  delete[] y;

  return 0;
}

int *random_array(int count) {
  int *result = new int[count];
  for (int i = 0; i < count; i++) {
    result[i] = rand() % 100 + 1;
  }

  return result;
}

void print_array(int *arr, int size) {
  std::cout << '[';
  for (int i = 0; i < size; i++) {
    std::cout << arr[i];
    if (i + 1 < size) {
      std::cout << ", ";
    }
  }
  std::cout << ']';
}

int *calculate_sequence_z(int *x, int sourceArrSize) {
  int *result = new int[sourceArrSize / 2];

  std::cout << "Calculating sequence z in parallel\n";

#ifdef TIMING
  // Start Time 0
  TimePoint st0 = steady_clock::now();
#endif

#pragma omp parallel for
  for (int i = 0; i < sourceArrSize / 2; i++) {
    result[i] = x[2 * i] + x[2 * i + 1];
  }

#ifdef TIMING
  // End Time 0
  TimePoint et0 = steady_clock::now();
  // Time Taken 0
  TimeSpan tt0 = duration_cast<TimeSpan>(et0 - st0);
#endif

  std::cout << "Sequence z: ";
  print_array(result, sourceArrSize / 2);
  std::cout << "\n\n";

#ifdef TIMING
  std::cout << "Time taken to calculate: " << tt0.count() << " seconds\n\n";
#endif

  return result;
}

int *calculate_sequence_w(int *z, int sourceArrSize) {
  int *result = new int[sourceArrSize / 2];
  result[0] = z[0];

  std::cout << "Calculating sequence w in sequentially\n";

#ifdef TIMING
  // Start Time 1
  TimePoint st1 = steady_clock::now();
#endif

  for (int i = 1; i < sourceArrSize / 2; i++) {
    result[i] = result[i - 1] + z[i];
  }

#ifdef TIMING
  // End Time 1
  TimePoint et1 = steady_clock::now();
  // Time Taken 1
  TimeSpan tt1 = duration_cast<TimeSpan>(et1 - st1);
#endif

  std::cout << "Sequence w: ";
  print_array(result, sourceArrSize / 2);
  std::cout << "\n\n";

#ifdef TIMING
  std::cout << "Time taken to calculate: " << tt1.count() << " seconds\n\n";
#endif

  return result;
}
