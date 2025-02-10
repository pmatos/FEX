#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <sys/types.h>
#include <chrono>

uint64_t getcntvtalt() {
  uint64_t cnt;
  asm volatile("isb\n mrs %0, cntvct_el0" : "=r"(cnt));
  return cnt;
}

[[clang::noinline]]
float frecpe(float x) {
  float r;
  asm volatile("frecpe %s[aRec], %s[aVal]\n" // 3DNow! reciprocal estimate
               : [aRec] "=w"(r)              // Output: result (floating-point)
               : [aVal] "w"(x)               // Input: x (floating-point)
  );
  return r;
}

[[clang::noinline]]
float frecpe2(float x) {
  float r;
  float two = 2.0f;
  float temp;
  asm volatile("frecpe %s[aRec], %s[aVal]\n"
               "fmsub %s[aTemp], %s[aRec], %s[aVal], %s[aTwo]\n"
               "fmul %s[aRec], %s[aRec], %s[aTemp]\n" // 3DNow! reciprocal estimate
               : [aRec] "=&w"(r), [aTemp] "=&w"(temp) // Output: result (floating-point)
               : [aVal] "w"(x), [aTwo] "w"(two)       // Input: x (floating-point)
  );
  return r;
}

[[clang::noinline]]
float recip(float x) {
  float r;
  float one = 1.0f;
  asm volatile("fdiv %s[aRec], %s[aOne], %s[aVal]" : [aRec] "=w"(r) : [aVal] "w"(x), [aOne] "w"(one));
  return r;
}

// Checks if rec is within the error bound of reference.
bool valid(float reference, float rec, float& diff) {
  float tolerance = std::ldexpf(1.5f, -12.0f);
  float err = fabsf(reference) * tolerance;
  diff = fabsf(reference - rec);
  return diff <= err;
}

int main(int argc, char* argv[]) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> range(-10000.0, 10000.0);

  float ctests[] = {1.0f, -1.0f, 0.5f, -0.5f};
  for (float f : ctests) {
    float rec = frecpe2(f);
    float ref = recip(f);
    float err;
    if (!valid(ref, rec, err)) {
      fprintf(stderr, "f: %f, ref: %f, rec2: %f\n", f, ref, rec);
    }
  }

  size_t ntests = atoi(argv[1]);
  printf("Running %lu tests\n", ntests);

  // Create array with tests
  float* tests = new float[ntests];
  for (size_t i = 0; i < ntests; i++) {
    float f = range(gen);
    while (f == 0.0) {
      f = range(gen);
    }
    tests[i] = f;
  }

  // Run tests for recip measuring time
  // store results in another array
  float* recips = new float[ntests];
  // time the following block in nanoseconds
  auto t_start_recip = std::chrono::high_resolution_clock::now();
  uint64_t start_recip = getcntvtalt();
  for (size_t i = 0; i < ntests; i++) {
    recips[i] = recip(tests[i]);
  }
  uint64_t end_recip = getcntvtalt();
  auto t_end_recip = std::chrono::high_resolution_clock::now();
  auto duration_recip = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end_recip - t_start_recip);

  // Run tests for frecpe measuring time
  // store results in another array
  float* frecpes = new float[ntests];
  float* frecpe_err = new float[ntests];
  auto t_start_frecpe = std::chrono::high_resolution_clock::now();
  uint64_t start_frecpe = getcntvtalt();
  for (size_t i = 0; i < ntests; i++) {
    frecpes[i] = frecpe(tests[i]);
  }
  uint64_t end_frecpe = getcntvtalt();
  auto t_end_frecpe = std::chrono::high_resolution_clock::now();
  auto duration_frecpe = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end_frecpe - t_start_frecpe);

  // Run tests for frecpe measuring time
  // store results in another array
  float* frecpe2s = new float[ntests];
  float* frecpe2_err = new float[ntests];
  auto t_start_frecpe2 = std::chrono::high_resolution_clock::now();
  uint64_t start_fprecpe2 = getcntvtalt();
  for (size_t i = 0; i < ntests; i++) {
    frecpe2s[i] = frecpe2(tests[i]);
  }
  uint64_t end_fprecpe2 = getcntvtalt();
  auto t_end_frecpe2 = std::chrono::high_resolution_clock::now();
  auto duration_frecpe2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end_frecpe2 - t_start_frecpe2);

  for (size_t i = 0; i < ntests; i++) {
    frecpe_err[i] = fabsf(frecpes[i] - recips[i]);
    frecpe2_err[i] = fabsf(frecpe2s[i] - recips[i]);
  }

  // Check for errors
  size_t frecpe_err_count = 0;
  for (size_t i = 0; i < ntests; i++) {
    float err;
    if (!valid(recips[i], frecpes[i], err)) {
      frecpe_err_count++;
    }
  }
  printf("frecpe errors: %lu, %.4f%%\n", frecpe_err_count, (double)frecpe_err_count / ntests * 100.0);

  // Check for errors
  size_t frecpe2_err_count = 0;
  for (size_t i = 0; i < ntests; i++) {
    float err;
    if (!valid(recips[i], frecpe2s[i], err)) {
      frecpe2_err_count++;
    }
  }
  printf("frecpe2 errors: %lu, %.4f%%\n", frecpe2_err_count, (double)frecpe2_err_count / ntests * 100.0);

  // print average time for frecpe and recip
  printf("recip vcycles: %zu\n", end_recip - start_recip);
  printf("frecpe vcycles: %zu, %.2f%%\n", end_frecpe - start_frecpe, (double)(end_frecpe - start_frecpe) / (end_recip - start_recip) * 100.0);
  printf("frecpe2 vcycles: %zu, %.2f%%\n", end_fprecpe2 - start_fprecpe2,
         (double)(end_fprecpe2 - start_fprecpe2) / (end_recip - start_recip) * 100.0);

  // now print the same but with times
  printf("recip time: %lu ns\n", duration_recip.count());
  printf("frecpe time: %lu ns, %.2f%%\n", duration_frecpe.count(), (double)duration_frecpe.count() / duration_recip.count() * 100.0);
  printf("frecpe2 time: %lu ns, %.2f%%\n", duration_frecpe2.count(), (double)duration_frecpe2.count() / duration_recip.count() * 100.0);

  // Print average errors
  float frecpe_err_sum = 0.0f;
  float frecpe2_err_sum = 0.0f;
  for (size_t i = 0; i < ntests; i++) {
    frecpe_err_sum += frecpe_err[i];
    frecpe2_err_sum += frecpe2_err[i];
  }
  printf("frecpe error: %.8f\n", (frecpe_err_sum / ntests));
  printf("frecpe2 error: %.8f\n", (frecpe2_err_sum / ntests));

  delete[] tests;
  delete[] frecpes;
  delete[] frecpe2s;
  delete[] recips;
  delete[] frecpe_err;
  delete[] frecpe2_err;

  return 0;
}