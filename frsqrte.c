#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#define TIME_DIFF_NS(start, end) (((end.tv_sec - start.tv_sec) * 1000000000ULL + (end.tv_nsec - start.tv_nsec)))

// Function to generate a random float within a specified range
float random_float(float min, float max) {
  // 1. Generate a random number between 0 and 1 (inclusive)
  // Using (double)rand()/RAND_MAX provides better distribution than (float)rand()/RAND_MAX.
  double fraction = (double)rand() / RAND_MAX;

  // 2. Scale and shift the random number to the desired range
  return min + (max - min) * (float)fraction;
}

uint64_t getcntvtalt() {
  uint64_t cnt;
  asm volatile("isb\n mrs %0, cntvct_el0" : "=r"(cnt));
  return cnt;
}

[[clang::noinline]]
float frsqrte(float x) {
  float r;
  asm volatile("frsqrte %s[aRec], %s[aVal]\n" // 3DNow! sqrt reciprocal estimate
               : [aRec] "=w"(r)               // Output: result (floating-point)
               : [aVal] "w"(x)                // Input: x (floating-point)
  );
  return r;
}

[[clang::noinline]]
float frsqrte2(float x) {
  float r;
  float temp;
  asm volatile("frsqrte %s[aRec], %s[aVal]\n"
               "fmul %s[aTemp], %s[aRec], %s[aRec]\n"
               "frsqrts %s[aTemp], %s[aTemp], %s[aVal]\n"
               "fmul %s[aRec], %s[aTemp], %s[aRec]\n" // 3DNow! sqrt reciprocal estimate
               : [aRec] "=&w"(r), [aTemp] "=&w"(temp) // Output: result (floating-point)
               : [aVal] "w"(x)                        // Input: x (floating-point)
  );
  return r;
}

[[clang::noinline]]
float recip(float x) {
  float one = 1.0f;
  float r;
  asm volatile("fsqrt %s[aVal], %s[aVal]\n"
               "fdiv %s[aVal], %s[aOne], %s[aVal]"
               : [aVal] "+&w"(x)
               : [aOne] "w"(one));
  return x;
}

// Checks if rec is within the error bound of reference.
bool valid(float reference, float rec, float* diff) {
  float tolerance = ldexpf(1.5f, -12.0f);
  float err = fabsf(reference) * tolerance;
  *diff = fabsf(reference - rec);
  return *diff <= err;
}

int main(int argc, char* argv[]) {
  float ctests[] = {1.0f, 9.0f, 4.0f, 25.0f, 16.0f};
  for (size_t i = 0; i < sizeof(ctests) / sizeof(*ctests); i++) {
    float f = ctests[i];
    float rec = frsqrte2(f);
    float ref = recip(f);
    float err;
    if (!valid(ref, rec, &err)) {
      fprintf(stderr, "f: %f, ref: %f, rec: %f\n", f, ref, rec);
    }
  }

  size_t ntests = atoi(argv[1]);
  printf("Running %lu tests\n", ntests);

  // Create array with tests
  float* tests = malloc(ntests * sizeof(*tests));
  for (size_t i = 0; i < ntests; i++) {
    float f = random_float(0.0, 10000.0);
    while (f == 0.0) {
      f = random_float(0.0, 10000.0);
    }
    tests[i] = f;
  }

  // Run tests for recip measuring time
  // store results in another array
  float* recips = malloc(ntests * sizeof(*recips));
  // time the following block in nanoseconds
  struct timespec t_start_recip, t_end_recip;
  clock_gettime(CLOCK_MONOTONIC_RAW, &t_start_recip);
  uint64_t start_recip = getcntvtalt();
  for (size_t i = 0; i < ntests; i++) {
    recips[i] = recip(tests[i]);
  }
  uint64_t end_recip = getcntvtalt();
  clock_gettime(CLOCK_MONOTONIC_RAW, &t_end_recip);
  unsigned long long duration_recip = TIME_DIFF_NS(t_start_recip, t_end_recip);

  // Run tests for frecpe measuring time
  // store results in another array
  float* frsqrtes = malloc(ntests * sizeof(*frsqrtes));
  float* frsqrte_err = malloc(ntests * sizeof(*frsqrte_err));
  struct timespec t_start_frsqrte, t_end_frsqrte;
  clock_gettime(CLOCK_MONOTONIC_RAW, &t_start_frsqrte);
  uint64_t start_frsqrte = getcntvtalt();
  for (size_t i = 0; i < ntests; i++) {
    frsqrtes[i] = frsqrte(tests[i]);
  }
  uint64_t end_frsqrte = getcntvtalt();
  clock_gettime(CLOCK_MONOTONIC_RAW, &t_end_frsqrte);
  unsigned long long duration_frsqrte = TIME_DIFF_NS(t_start_frsqrte, t_end_frsqrte);

  // Run tests for frecpe measuring time
  // store results in another array
  float* frsqrte2s = malloc(ntests * sizeof(*frsqrte2s));
  float* frsqrte2_err = malloc(ntests * sizeof(*frsqrte2_err));
  struct timespec t_start_frsqrte2, t_end_frsqrte2;
  clock_gettime(CLOCK_MONOTONIC_RAW, &t_start_frsqrte2);
  uint64_t start_frsqrte2 = getcntvtalt();
  for (size_t i = 0; i < ntests; i++) {
    frsqrte2s[i] = frsqrte2(tests[i]);
  }
  uint64_t end_frsqrte2 = getcntvtalt();
  clock_gettime(CLOCK_MONOTONIC_RAW, &t_end_frsqrte2);
  unsigned long long duration_frsqrte2 = TIME_DIFF_NS(t_start_frsqrte2, t_end_frsqrte2);

  for (size_t i = 0; i < ntests; i++) {
    frsqrte_err[i] = fabsf(frsqrtes[i] - recips[i]);
    frsqrte2_err[i] = fabsf(frsqrte2s[i] - recips[i]);
  }

  // Check for errors
  size_t frsqrte_err_count = 0;
  for (size_t i = 0; i < ntests; i++) {
    float err;
    if (!valid(recips[i], frsqrtes[i], &err)) {
      frsqrte_err_count++;
    }
  }
  printf("frsqrte errors: %lu, %.4f%%\n", frsqrte_err_count, (double)frsqrte_err_count / ntests * 100.0);

  // Check for errors
  size_t frsqrte2_err_count = 0;
  for (size_t i = 0; i < ntests; i++) {
    float err;
    if (!valid(recips[i], frsqrte2s[i], &err)) {
      frsqrte2_err_count++;
    }
  }
  printf("frsqrte2 errors: %lu, %.4f%%\n", frsqrte2_err_count, (double)frsqrte2_err_count / ntests * 100.0);

  // print average time for frecpe and recip
  printf("recip vcycles: %zu\n", end_recip - start_recip);
  printf("frsqrte vcycles: %zu, %.2f%%\n", end_frsqrte - start_frsqrte, (double)(end_frsqrte - start_frsqrte) / (end_recip - start_recip) * 100.0);
  printf("frsqrte2 vcycles: %zu, %.2f%%\n", end_frsqrte2 - start_frsqrte2,
         (double)(end_frsqrte2 - start_frsqrte2) / (end_recip - start_recip) * 100.0);

  // now print the same but with times
  printf("recip time: %llu ns\n", duration_recip);
  printf("frsqrte time: %llu ns, %.2f%%\n", duration_frsqrte, (double)duration_frsqrte / duration_recip * 100.0);
  printf("frsqrte2 time: %llu ns, %.2f%%\n", duration_frsqrte2, (double)duration_frsqrte2 / duration_recip * 100.0);

  // Print average errors
  float frsqrte_err_sum = 0.0f;
  float frsqrte2_err_sum = 0.0f;
  for (size_t i = 0; i < ntests; i++) {
    frsqrte_err_sum += frsqrte_err[i];
    frsqrte2_err_sum += frsqrte2_err[i];
  }
  printf("frsqrte error: %.8f\n", (frsqrte_err_sum / ntests));
  printf("frsqrte2 error: %.8f\n", (frsqrte2_err_sum / ntests));

  free(tests);
  free(frsqrtes);
  free(frsqrte2s);
  free(recips);
  free(frsqrte_err);
  free(frsqrte2_err);

  return 0;
}