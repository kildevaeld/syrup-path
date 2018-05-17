#include <unity.h>

extern void test_path_join();
extern void test_path_basename();
extern void test_path_dirname();
extern void test_path_extname();

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_path_join);
  RUN_TEST(test_path_basename);
  RUN_TEST(test_path_dirname);
  RUN_TEST(test_path_extname);

  return UNITY_END();
}