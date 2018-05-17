#include <limits.h>
#include <syrup/path.h>
#include <unity.h>

void test_path_join() {
  char buffer[PATH_MAX];
  sy_path_join(buffer, "/Test/subdirectory", "test/me", NULL);
  TEST_ASSERT_EQUAL_STRING("/Test/subdirectory/test/me", buffer);

  sy_path_join(buffer, "/Test/subdirectory/", "/test/me", NULL);
  TEST_ASSERT_EQUAL_STRING("/Test/subdirectory/test/me", buffer);

  sy_path_join(buffer, "/Test/subdirectory///", "//test/me", NULL);
  TEST_ASSERT_EQUAL_STRING("/Test/subdirectory/test/me", buffer);

  sy_path_join(buffer, "/Test/subdirectory", "./test/me", NULL);
  TEST_ASSERT_EQUAL_STRING("/Test/subdirectory/test/me", buffer);

  const char *a[] = {"/Test/subdirectory", "../test/me2", NULL};
  char out[PATH_MAX];
  sy_path_join_array(out, a);
  TEST_ASSERT_EQUAL_STRING("/Test/test/me2", out);

  sy_path_join(buffer, "/Test/subdirectory", "../../me", NULL);
  TEST_ASSERT_EQUAL_STRING("/me", buffer);

  sy_path_join(buffer, "/Test/subdirectory", "../../../me", NULL);
  TEST_ASSERT_EQUAL_STRING("../me", buffer);

  sy_path_join(buffer, "/Test///subdirectory", "../me", NULL);
  TEST_ASSERT_EQUAL_STRING("/Test/me", buffer);
}

void test_path_basename() {
  char buffer[PATH_MAX];
  size_t idx;
  size_t len = sy_path_base("/test/min/finger.png", &idx);
  TEST_ASSERT_EQUAL_INT(10, len);
  TEST_ASSERT_EQUAL_INT(10, idx);

  len = sy_path_base("test/min/finger", &idx);
  TEST_ASSERT_EQUAL_INT(6, len);
  TEST_ASSERT_EQUAL_INT(9, idx);

  len = sy_path_base("finger", &idx);
  TEST_ASSERT_EQUAL_INT(6, len);
  TEST_ASSERT_EQUAL_INT(0, idx);
}
void test_path_dirname() {
  char buffer[PATH_MAX];
  size_t idx;
  idx = sy_path_dir("/test/min/numse");
  TEST_ASSERT_EQUAL_INT(10, idx);

  idx = sy_path_dir("/test/min");
  TEST_ASSERT_EQUAL_INT(6, idx);
}
void test_path_extname() {
  char buffer[PATH_MAX];
  size_t len, idx;
  len = sy_path_ext("test.png", &idx);
  TEST_ASSERT_EQUAL_INT(4, idx);
  TEST_ASSERT_EQUAL_INT(4, len);

  len = sy_path_ext("test/mig/test.px", &idx);
  TEST_ASSERT_EQUAL_INT(13, idx);
  TEST_ASSERT_EQUAL_INT(3, len);

  len = sy_path_ext("/test/mig/test.px", &idx);
  TEST_ASSERT_EQUAL_INT(14, idx);
  TEST_ASSERT_EQUAL_INT(3, len);
}