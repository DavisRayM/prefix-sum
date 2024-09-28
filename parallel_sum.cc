/// Parallel Sum - Work-efficient algorithm implementation.
/// Based on the Wikipedia algorithm explanation & explanations from
/// CPSC 5600 F24 (Taught by Braxton Cuneo)
///
/// Note: This code could probably be shortened quite a bit if one decides to
/// use Vectors & std::inclusive_scan for the sequentielly portions of the
/// code... But, what's the fun of that ?
#include <chrono>
#include <iostream>
#include <omp.h>
#include <stdlib.h>

using std::chrono::duration_cast;
using std::chrono::steady_clock;
using TimePoint = std::chrono::steady_clock::time_point;
using TimeSpan = std::chrono::duration<double>;

/// Generates a array containing random numbers in the range 1 to 100
int *random_array(int count) {
  int *result = new int[count];
  for (int i = 0; i < count; i++) {
    result[i] = rand() % 100 + 1;
  }

  return result;
}

void print_array(int *arr, int size) {
  for (int i = 0; i < size; i++) {
    std::cout << arr[i] << ' ';
  }
}

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

  // Step 1: Calculate itermediate sequence z; z_0 = x_0 + x_1, z_1 = x_2 +
  // x_3...
  int *z = new int[arraySize / 2];

  std::cout << "Calculating sequence z in parallel\n";

  // Start Time 0
  TimePoint st0 = steady_clock::now();

#pragma omp parallel for
  for (int i = 0; i < arraySize / 2; i++) {
    z[i] = x[2 * i] + x[2 * i + 1];
  }

  // End Time 0
  TimePoint et0 = steady_clock::now();
  // Time Taken 0
  TimeSpan tt0 = duration_cast<TimeSpan>(et0 - st0);

  std::cout << "Sequence z: ";
  print_array(z, arraySize / 2);
  std::cout << "\n\nTime taken to calculate: " << tt0.count() << " seconds\n\n";

  // Step 2: Calculate the prefix sum of intermediate sequence z and assign to
  // sequence w
  int *w = new int[arraySize / 2];
  w[0] = z[0];

  std::cout << "Calculating sequence w in sequentially\n";

  // Start Time 1
  TimePoint st1 = steady_clock::now();

  for (int i = 1; i < arraySize / 2; i++) {
    w[i] = w[i - 1] + z[i];
  }

  // End Time 1
  TimePoint et1 = steady_clock::now();
  // Time Taken 1
  TimeSpan tt1 = duration_cast<TimeSpan>(et1 - st1);

  std::cout << "Sequence w: ";
  print_array(w, arraySize / 2);
  std::cout << "\n\nTime taken to calculate: " << tt1.count() << " seconds\n\n";

  // Step 3: Calculate the final sequence y
  int *y = new int[arraySize];
  y[0] = x[0];
  y[1] = z[0];

  delete[] z;

  std::cout << "Calculating sequence y in parallel\n";

  // Start Time 2
  TimePoint st2 = steady_clock::now();
#pragma omp parallel for
  for (int i = 2; i < arraySize; i++) {
    // If even calculate value as sum of previous value and position i in x
    if (i % 2 == 0) {
      y[i] = w[i / 2 - 1] + x[i];
    } else {
      y[i] = w[i / 2];
    }
  }

  // End Time 2
  TimePoint et2 = steady_clock::now();
  // Time Taken 0
  TimeSpan tt2 = duration_cast<TimeSpan>(et2 - st2);

  std::cout << "Prefix Sum Array (y): ";
  print_array(y, arraySize);
  std::cout << "\n\nTime taken to calculate: " << tt2.count() << " seconds\n\n";

  delete[] x;
  delete[] y;

  return 0;
}
